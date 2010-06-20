/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QDBusInterface>
#include <QDBusReply>
#include <QTimer>

#include "chat_manager.h"
#include "config_file.h"
#include "debug.h"
#include "gadu.h"
#include "icons_manager.h"
#include "main_configuration_window.h"

#include "kde_notify.h"


extern "C" KADU_EXPORT int kde_notify_init(bool firstLoad)
{
	kdebugf();

	kde_notify = new KdeNotify();

	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/kde_notify.ui"), kde_notify);

	kdebugf2();
	return 0;
}

extern "C" KADU_EXPORT void kde_notify_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/kde_notify.ui"), kde_notify);
	delete kde_notify;
	kde_notify = 0;

	kdebugf2();
}


KdeNotify::KdeNotify(QObject *parent, const char *name) : Notifier(parent, name)
{
	kdebugf();

	stripHTML.setPattern(QString::fromLatin1("<.*>"));
	stripHTML.setMinimal(true);

	knotify = new QDBusInterface("org.kde.VisualNotifications",
			"/VisualNotifications", "org.kde.VisualNotifications");

	/* Dorr: maybe we're using patched version of KDE */
	if (!knotify->isValid())
	{
		delete (knotify);
		knotify = new QDBusInterface("org.freedesktop.Notifications",
				"/org/freedesktop/Notifications", "org.freedesktop.Notifications");
	}

	knotify->connection().connect(knotify->service(), knotify->path(), knotify->interface(), 
		"ActionInvoked", this, SLOT(actionInvoked(unsigned int, QString)));

	notification_manager->registerNotifier("KNotify", this);
	createDefaultConfiguration();

	kdebugf2();
}

KdeNotify::~KdeNotify()
{
	kdebugf();

	notification_manager->unregisterNotifier("KNotify");
	delete knotify;
	knotify = 0;

	kdebugf2();
}


NotifierConfigurationWidget *KdeNotify::createConfigurationWidget(QWidget *parent, char *name)
{
	return 0;
}


void KdeNotify::createDefaultConfiguration()
{
	config_file.addVariable("KDENotify", "Timeout", 10);
	config_file.addVariable("KDENotify", "ShowContentMessage", true);
	config_file.addVariable("KDENotify", "CiteSign", 100);
}


void KdeNotify::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	connect(mainConfigurationWindow->widgetById("kdenotify/showContent"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widgetById("kdenotify/showContentCount"), SLOT(setEnabled(bool)));
}


void KdeNotify::notify(Notification *notification)
{
	QList<QVariant> args;
	args.append("Kadu");
	args.append(0U);
	args.append("");
	args.append(icons_manager->iconPath(notification->icon()));

	if (((notification->type() == "NewMessage") || (notification->type() == "NewChat")) && config_file.readBoolEntry("KDENotify", "ShowContentMessage"))
	{
		args.append(notification->text().remove(stripHTML));
		if (notification->details().remove(stripHTML).length() > config_file.readNumEntry("KDENotify", "CiteSign", 10))
			args.append(notification->details().remove(stripHTML).left(config_file.readNumEntry("KDENotify", "CiteSign", 10)) + "...");
		else
			args.append(notification->details().remove(stripHTML));
	}
	else
	{
		args.append("Kadu");
		args.append(notification->text());
	}

	QStringList actions;
	if ((notification->type() == "NewMessage") || (notification->type() == "NewChat"))
		actions << "1" << tr("View");

	if ((notification->type() == "StatusChanged/ToOnline") || (notification->type() == "StatusChanged/ToBusy") || (notification->type() == "StatusChanged/ToInvisible"))
		actions << "2" << tr("Chat");

	actions << "3" << tr("Ignore");
	args.append(actions);
	args.append(QVariantMap());
	args.append(config_file.readNumEntry("KDENotify", "Timeout", 10) * 1000);
	
	QDBusReply<unsigned int> reply = knotify->callWithArgumentList(QDBus::Block, "Notify", args);
	if (reply.isValid())
	{
		idMap.insert(reply.value(), notification->userListElements());
		idQueue.enqueue(reply.value());
		QTimer::singleShot(config_file.readNumEntry("KDENotify", "Timeout", 10) * 1000 + 2000, this, SLOT(deleteMapItem()));
	}
}


void KdeNotify::actionInvoked(unsigned int id, QString action)
{
	/* Dorr: do not process noto ours notifications */
	if (!idQueue.contains(id))
		return;

	if (action == "1")
	{
		chat_manager->openPendingMsgs(idMap.value(id), true);
		chat_manager->deletePendingMsgs(idMap.value(id));
		ChatWidget* window = chat_manager->findChatWidget(idMap.value(id));
	}
	else if (action == "2")
		chat_manager->openChatWidget(gadu, idMap.value(id), true);

	QList<QVariant> args;
	args.append(id);
	knotify->callWithArgumentList(QDBus::Block, "CloseNotification", args);
}


void KdeNotify::deleteMapItem()
{
	idMap.remove(idQueue.dequeue());
}

KdeNotify *kde_notify = NULL;
