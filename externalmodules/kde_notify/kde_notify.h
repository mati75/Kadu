/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KDE_NOTIFY_H
#define KDE_NOTIFY_H

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QQueue>
#include <QtCore/QRegExp>
#include <QtCore/QStringList>

#include "userlist.h"
#include "../notify/notify.h"
#include "../notify/notification.h"


class QDBusInterface;


class KdeNotify : public Notifier, public ConfigurationUiHandler
{
	Q_OBJECT

public:
	KdeNotify(QObject *parent = 0, const char *name = 0);
	~KdeNotify();

	virtual NotifierConfigurationWidget *createConfigurationWidget(QWidget *parent = 0, char *name = 0);
	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);
	virtual void notify(Notification *notification);

	void copyConfiguration(const QString&, const QString&) {}

private:
	QDBusInterface* knotify;
	QRegExp stripHTML;
	QMap <unsigned int, UserListElements> idMap;
	QQueue<unsigned int> idQueue;

	void createDefaultConfiguration();

private slots:
	void actionInvoked(unsigned int id, QString action);
	void deleteMapItem();
};

extern KdeNotify *kde_notify;

#endif // KDE_NOTIFY_H
