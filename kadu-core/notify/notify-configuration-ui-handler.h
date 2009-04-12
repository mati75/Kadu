/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef NOTIFY_CONFIGURATION_UI_HANDLER
#define NOTIFY_CONFIGURATION_UI_HANDLER

#include "main_configuration_window.h"

#include "notification-manager.h"

class QListWidget;

class NotifierConfigurationWidget;
class NotifyGroupBox;
class NotifyTreeWidget;

class NotifyConfigurationUiHandler : public ConfigurationUiHandler
{
	Q_OBJECT

public:
	struct NotifierGuiItem
	{
		NotifierConfigurationWidget *ConfigurationWidget;
		NotifyGroupBox *ConfigurationGroupBox;
		QMap<QString, bool> Events;
	};

private:
	QMap<Notifier *, NotifierGuiItem> NotifierGui;

	QListWidget *allUsers;
	QListWidget *notifiedUsers;
	ConfigComboBox *notifications;
	ConfigGroupBox *notificationsGroupBox;

	NotifyTreeWidget *notifyTreeWidget;

	QString CurrentEvent;

	void addConfigurationWidget(Notifier *notifier);
	void removeConfigurationWidget(Notifier *notifier);

private slots:
	void notifierRegistered(Notifier *notifier);
	void notifierUnregistered(Notifier *notifier);

	void configurationWindowApplied();
	void mainConfigurationWindowDestroyed();

	void moveToNotifyList();
	void moveToAllList();

	void eventSwitched();
	void notifierToggled(Notifier *notifier, bool toggled);

protected:
	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

public:
	explicit NotifyConfigurationUiHandler(QObject *parent = 0);
	virtual ~NotifyConfigurationUiHandler();

};

#endif // NOTIFY_CONFIGURATION_UI_HANDLER
