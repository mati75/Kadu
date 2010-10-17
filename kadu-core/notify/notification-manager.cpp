/*
 * %kadu copyright begin%
 * Copyright 2007, 2008, 2009 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004 Tomasz Jarzynka (tomee@cpi.pl)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtGui/QX11Info>
#include <QtGui/QApplication>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-shared.h"
#include "buddies/group.h"
#include "buddies/group-manager.h"
#include "chat/message/message.h"
#include "configuration/configuration-file.h"
#include "contacts/contact.h"
#include "contacts/contact-manager.h"
#include "core/core.h"
#include "gui/actions/action.h"
#include "gui/widgets/buddies-list-view-menu-manager.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/custom-input.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/main-configuration-window.h"
#include "gui/windows/main-window.h"
#include "gui/windows/message-dialog.h"
#include "notify/contact-notify-data.h"
#include "notify/notifier.h"
#include "notify/notify-configuration-ui-handler.h"
#include "notify/window-notifier.h"
#include "status/status-container-manager.h"

#include "debug.h"
#include "misc/misc.h"

#include "new-message-notification.h"
#include "status-changed-notification.h"

#include "x11tools.h" // this should be included as last one,
#undef Status         // and Status defined by Xlib.h must be undefined

#define FULLSCREENCHECKTIMER_INTERVAL 2000 /*ms*/

NotificationManager *NotificationManager::Instance = 0;

NotificationManager * NotificationManager::instance()
{
	if (!Instance)
	{
		Instance = new NotificationManager();
		Instance->init();

		MessageNotification::registerEvents();
		StatusChangedNotification::registerEvents();
	}

	return Instance;
}

NotificationManager::NotificationManager()
{
}

void NotificationManager::init()
{
    	kdebugf();

	UiHandler = new NotifyConfigurationUiHandler(this);
	MainConfigurationWindow::registerUiHandler(UiHandler);

	SilentMode = false;

	FullScreenCheckTimer.setInterval(FULLSCREENCHECKTIMER_INTERVAL);
	connect(&FullScreenCheckTimer, SIGNAL(timeout()), this, SLOT(checkFullScreen()));
	IsFullScreen = false;

	createDefaultConfiguration();
	configurationUpdated();
	AutoSilentMode = false;
	//TODO 0.6.6:
	//triggerAllAccountsRegistered();

	notifyAboutUserActionDescription = new ActionDescription(this,
		ActionDescription::TypeUser, "notifyAboutUserAction",
		this, SLOT(notifyAboutUserActionActivated(QAction *, bool)),
		"kadu_icons/kadu-notifyaboutuser", "kadu_icons/kadu-notifyaboutuser", tr("Notify about user"), true, "",
		checkNotify
	);

	SilentModeActionDescription = new ActionDescription(this,
		ActionDescription::TypeGlobal, "silentModeAction",
		this, SLOT(silentModeActionActivated(QAction *, bool)),
		"kadu_icons/silent-mode-off", "kadu_icons/silent-mode-off", tr("Enable notifications"), true, tr("Enable notifications")
	);
	connect(SilentModeActionDescription, SIGNAL(actionCreated(Action *)), this, SLOT(silentModeActionCreated(Action *)));

	connect(StatusContainerManager::instance(), SIGNAL(statusChanged()), this, SLOT(statusChanged()));

	foreach (Group group, GroupManager::instance()->items())
		groupAdded(group);

	new WindowNotifier(this);
	kdebugf2();
}

NotificationManager::~NotificationManager()
{
	kdebugf();

	FullScreenCheckTimer.stop();

	MainConfigurationWindow::unregisterUiHandler(UiHandler);

	StatusChangedNotification::unregisterEvents();
	MessageNotification::unregisterEvents();

	triggerAllAccountsUnregistered();

	while (!Notifiers.isEmpty())
	{
		kdebugm(KDEBUG_WARNING, "WARNING: not unregistered notifiers found! (%u)\n", Notifiers.size());
		unregisterNotifier(Notifiers[0]);
	}

	kdebugf2();
}

void NotificationManager::setSilentMode(bool silentMode)
{
	if (silentMode != SilentMode)
	{
		SilentMode = silentMode;
		emit silentModeToggled(SilentMode);
	}
}

bool NotificationManager::silentMode()
{
	return SilentMode || (IsFullScreen && config_file.readBoolEntry("Notify", "FullscreenSilentMode", false));
}

void NotificationManager::notifyAboutUserActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	kdebugf();

	Action *action = dynamic_cast<Action *>(sender);
	if (!action)
		return;

	BuddySet buddies = action->buddies();

	bool on = true;
	foreach (const Buddy buddy, buddies)
	{
		ContactNotifyData *cnd = 0;
		if (buddy.data())
			cnd = buddy.data()->moduleStorableData<ContactNotifyData>("notify");

		if (!cnd || !cnd->notify())
		{
			on = false;
			break;
		}
	}

	if (NotifyAboutAll)
	{
		NotifyAboutAll = false;
		config_file.writeEntry("Notify", "NotifyAboutAll", false);
	}

	foreach (const Buddy buddy, buddies)
	{
		if (buddy.isNull() || buddy.isAnonymous())
			continue;

		ContactNotifyData *cnd = 0;
		if (buddy.data())
			cnd = buddy.data()->moduleStorableData<ContactNotifyData>("notify", true);
		if (!cnd)
			continue;

		if (cnd->notify() == on)
		{
			cnd->setNotify(!on);
			cnd->store();
		}
	}

	foreach (Action *action, notifyAboutUserActionDescription->actions())
		if (action->contacts().toBuddySet() == buddies)
			action->setChecked(!on);

	kdebugf2();
}

void NotificationManager::silentModeActionCreated(Action *action)
{
	action->setChecked(!SilentMode);
}

void NotificationManager::silentModeActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)

	setSilentMode(!toggled);
	foreach (Action *action, SilentModeActionDescription->actions())
		action->setChecked(toggled);

	config_file.writeEntry("Notify", "SilentMode", SilentMode);
}

void NotificationManager::statusChanged()
{
	if (SilentModeWhenDnD && !silentMode() && StatusContainerManager::instance()->status().type() == "DoNotDisturb")
	{
		foreach (Action *action, SilentModeActionDescription->actions())
			action->setChecked(false);

		AutoSilentMode = true;
	}
	else if (!silentMode() && AutoSilentMode)
	{
		foreach (Action *action, SilentModeActionDescription->actions())
			action->setChecked(true);

		AutoSilentMode = false;
	}
}

void NotificationManager::accountRegistered(Account account)
{
	Protocol *protocol = account.protocolHandler();
	if (!protocol)
		return;
// 	TODO: 0.6.6
// 	connect(protocol, SIGNAL(connectionError(Account, const QString &, const QString &)),
// 			this, SLOT(connectionError(Account, const QString &, const QString &)));
	connect(account, SIGNAL(buddyStatusChanged(Contact, Status)),
			this, SLOT(contactStatusChanged(Contact, Status)));
	connect(account, SIGNAL(connected()), this, SLOT(accountConnected()));

	ChatService *chatService = protocol->chatService();
	if (chatService)
	{
		connect(chatService, SIGNAL(messageReceived(const Message &)),
				this, SLOT(messageReceived(const Message &)));
	}
}

void NotificationManager::accountUnregistered(Account account)
{
	Protocol *protocol = account.protocolHandler();

	if (!protocol)
		return;

// 	disconnect(protocol, SIGNAL(connectionError(Account, const QString &, const QString &)),
// 			this, SLOT(connectionError(Account, const QString &, const QString &))); // TODO: 0.6.6 fix
	disconnect(account, SIGNAL(buddyStatusChanged(Contact, Status)),
			this, SLOT(contactStatusChanged(Contact, Status)));
	disconnect(account, SIGNAL(connected()), this, SLOT(accountConnected()));

	ChatService *chatService = protocol->chatService();
	if (chatService)
	{
		disconnect(chatService, SIGNAL(messageReceived(const Message &)),
				this, SLOT(messageReceived(const Message &)));
	}
}

void NotificationManager::accountConnected()
{
	Account account(sender());
	if (!account)
		return;

	if (config_file.readBoolEntry("Notify", "NotifyIgnoreOnConnection"))
	{
		QDateTime *dateTime = account.data()->moduleData<QDateTime>("notify-account-connected", true);
		*dateTime = QDateTime::currentDateTime().addSecs(10);
	}
}

void NotificationManager::contactStatusChanged(Contact contact, Status oldStatus)
{
	kdebugf();

	if (!contact.contactAccount())
		return;

	Protocol *protocol = contact.contactAccount().protocolHandler();
	if (!protocol || Protocol::NetworkConnected != protocol->state())
		return;

	if (config_file.readBoolEntry("Notify", "NotifyIgnoreOnConnection"))
	{
		QDateTime *dateTime = contact.contactAccount().data()->moduleData<QDateTime>("notify-account-connected");
		if (dateTime && (*dateTime >= QDateTime::currentDateTime()))
			return;
	}

	bool notify_contact = true;
	ContactNotifyData *cnd = 0;
	cnd = contact.ownerBuddy().data()->moduleStorableData<ContactNotifyData>("notify");

	if (!cnd || !cnd->notify())
		notify_contact = false;

	if (!notify_contact && !NotifyAboutAll)
	{
		kdebugmf(KDEBUG_FUNCTION_END, "end: not notifying user AND not notifying all users\n");
		return;
	}

	if (!contact.contactAccount())
		return;

	if (contact == contact.contactAccount().accountContact()) // myself
		return;

	Status status = contact.currentStatus();
	if (oldStatus == status)
		return;

	if (config_file.readBoolEntry("Notify", "IgnoreOnlineToOnline") &&
			!status.isDisconnected() &&
			!oldStatus.isDisconnected())
		return;

	QString changedTo = "/To" + Status::name(status, false);

	ContactSet contacts(contact);

	StatusChangedNotification *statusChangedNotification;
	statusChangedNotification = new StatusChangedNotification(changedTo, contacts);

	notify(statusChangedNotification);

	kdebugf2();
}

void NotificationManager::messageReceived(const Message &message)
{
	kdebugf();

	ChatWidget *chatWidget = ChatWidgetManager::instance()->byChat(message.messageChat());
	if (!chatWidget) // new chat
		notify(new MessageNotification(MessageNotification::NewChat, message));
	else // new message in chat
		if (!chatWidget->edit()->hasFocus() || !config_file.readBoolEntry("Notify", "NewMessageOnlyIfInactive"))
			notify(new MessageNotification(MessageNotification::NewMessage, message));

	kdebugf2();
}

void NotificationManager::registerNotifyEvent(NotifyEvent *notifyEvent)
{
	kdebugf();

	NotifyEvents.append(notifyEvent);
	emit notifyEventRegistered(notifyEvent);

	kdebugf2();
}

void NotificationManager::unregisterNotifyEvent(NotifyEvent *notifyEvent)
{
	kdebugf();

	NotifyEvents.removeAll(notifyEvent);
	emit notifyEventUnregistered(notifyEvent);

	kdebugf2();
}

void NotificationManager::registerNotifier(Notifier *notifier)
{
	kdebugf();
	if (Notifiers.contains(notifier))
	{
		kdebugm(KDEBUG_WARNING, "WARNING: '%s' already exists in notifiers! "
		"strange... unregistering old Notifier\n", qPrintable(notifier->name()));

		unregisterNotifier(notifier);
	}

	Notifiers.append(notifier);
	emit notiferRegistered(notifier);

	kdebugf2();
}

void NotificationManager::unregisterNotifier(Notifier *notifier)
{
	kdebugf();

	if (!Notifiers.contains(notifier))
	{
		kdebugm(KDEBUG_WARNING, "WARNING: '%s' not registered!\n", qPrintable(notifier->name()));
		return;
	}

	emit notiferUnregistered(notifier);
	Notifiers.removeAll(notifier);

	kdebugf2();
}

QList<Notifier *> NotificationManager::notifiers()
{
	return Notifiers;
}

QList<NotifyEvent *> NotificationManager::notifyEvents()
{
	return NotifyEvents;
}

bool NotificationManager::ignoreNotifications()
{
	if (silentMode())
		return true;

	if (AutoSilentMode)
		return true;

	return false;
}

void NotificationManager::notify(Notification *notification)
{
	kdebugf();

	QString notifyType = notification->key();
	bool foundNotifier = false;
	bool foundNotifierWithCallbackSupported = !notification->requireCallback();

	if (ignoreNotifications())
	{
		notification->callbackDiscard();
		return;
	}

	notification->acquire();

	foreach (Notifier *notifier, Notifiers)
	{
		if (config_file.readBoolEntry("Notify", notifyType + '_' + notifier->name()))
		{
			notifier->notify(notification);
			foundNotifier = true;
			foundNotifierWithCallbackSupported = foundNotifierWithCallbackSupported ||
					(Notifier::CallbackSupported == notifier->callbackCapacity());
		}
	}

	if (!foundNotifierWithCallbackSupported)
		foreach (Notifier *notifier, Notifiers)
		{
			if (Notifier::CallbackSupported == notifier->callbackCapacity())
			{
				notifier->notify(notification);
				foundNotifier = true;
				foundNotifierWithCallbackSupported = true;
				break;
			}
		}

	if (!foundNotifier)
		notification->callbackDiscard();

	notification->release();

	if (!foundNotifierWithCallbackSupported)
		MessageDialog::msg(tr("Unable to find notifier for %1 event").arg(notification->type()), true, "dialog-warning");

	kdebugf2();
}

void NotificationManager::groupAdded(Group group)
{
	connect(group, SIGNAL(updated()), this, SLOT(groupUpdated()));
}

// TODO 0.6.6:
void NotificationManager::groupUpdated()
{
	Group group = sender();
	if (group.isNull())
		return;

	bool notify = group.notifyAboutStatusChanges();

	if (NotifyAboutAll && !notify)
	{
		NotifyAboutAll = false;
		config_file.writeEntry("Notify", "NotifyAboutAll", false);
	}

	foreach (const Buddy buddy, BuddyManager::instance()->items())
	{
		if (buddy.isNull() || buddy.isAnonymous() || buddy.groups().contains(group))
			continue;

		ContactNotifyData *cnd = 0;
		if (buddy.data())
			buddy.data()->moduleStorableData<ContactNotifyData>("notify", true);
		if (!cnd)
			continue;

		cnd->setNotify(notify);
		cnd->store();
	}
}

void NotificationManager::configurationUpdated()
{
	NotifyAboutAll = config_file.readBoolEntry("Notify", "NotifyAboutAll");
	SilentModeWhenDnD = config_file.readBoolEntry("Notify", "AwaySilentMode", false);
	SilentModeWhenFullscreen = config_file.readBoolEntry("Notify", "FullscreenSilentMode", false);
	setSilentMode(config_file.readBoolEntry("Notify", "SilentMode", false));
#if defined(Q_WS_X11) && !defined(Q_WS_MAEMO_5)
	if (SilentModeWhenFullscreen)
		FullScreenCheckTimer.start();
	else
	{
		FullScreenCheckTimer.stop();
		IsFullScreen = false;
	}
#endif
}

void NotificationManager::createDefaultConfiguration()
{
	config_file.addVariable("Notify", "IgnoreOnlineToOnline", false);
	config_file.addVariable("Notify", "NewMessageOnlyIfInactive", true);
	config_file.addVariable("Notify", "NotifyAboutAll", true);
	config_file.addVariable("Notify", "NotifyIgnoreOnConnection", true);
}

QString NotificationManager::notifyConfigurationKey(const QString &eventType)
{
	QString event = eventType;

	while (true)
	{
		int slashPosition = event.lastIndexOf('/');
		if (-1 == slashPosition)
			return event;

		if (config_file.readBoolEntry("Notify", event + "_UseCustomSettings"))
			return event;

		event = event.left(slashPosition);
	}
}

ConfigurationUiHandler * NotificationManager::configurationUiHandler()
{
	return UiHandler;
}

void NotificationManager::checkFullScreen()
{
#if defined(Q_WS_X11) && !defined(Q_WS_MAEMO_5)
	bool wasSilent = silentMode();
	IsFullScreen = X11_checkFullScreen(QX11Info::display());
	if (silentMode() != wasSilent)
		emit silentModeToggled(silentMode());
#endif
}

void checkNotify(Action *action)
{
	kdebugf();

	action->setEnabled(true);

	bool on = true;
	foreach (const Buddy buddy, action->contacts().toBuddySet())
	{
		ContactNotifyData *cnd = 0;
		if (buddy.data())
			cnd = buddy.data()->moduleStorableData<ContactNotifyData>("notify");

		if (!cnd || !cnd->notify())
		{
			on = false;
			break;
		}
	}

	action->setChecked(on);

	kdebugf2();
}
