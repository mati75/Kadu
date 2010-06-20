/**
*
* Project:     Water Notify for Kadu
* File:        water_notify.h
* Copyright:   (C) 2008 by Korneliusz Jrzebski
* Author:      Korneliusz Jarzebski <korneliusz at jarzebski dot pl>
* Modified by: Tomasz Rostanski <rozteck at interia dot pl>
* Link:        http://www.jarzebski.pl/
* Version:     0.2.1
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*
*/

#ifndef WATER_NOTIFY_H
#define WATER_NOTIFY_H

#include <QtCore/QList>

#include "dbus/dbus.h"
#include "main_configuration_window.h"
#include "modules/notify/notify.h"
#include "pending_msgs.h"

class ChatWidget;

class WaterNotify : public Notifier, ConfigurationUiHandler
{
	Q_OBJECT

	public:

	    WaterNotify(QObject *parent = 0, const char *name = 0);
	    ~WaterNotify();

    	    virtual void copyConfiguration( const QString &fromEvent, const QString &toEvent ) {}
    	    virtual void mainConfigurationWindowCreated(MainConfigurationWindow*);
	    virtual NotifierConfigurationWidget* createConfigurationWidget(QWidget*, char*);
	    virtual void notify(Notification *notification);

	private:

	    QTimer* timer;
	
	    bool WaterDrop_On, WaterDrop_Disable;
	    int RootWindow_ID;
	    QString WaterDrop_Owner;

	    QList<ChatWidget*> msgChats_;
	 	
	    void AppendArgument_STRING(DBusMessageIter *iter, const char *value);
	    void AppendArgument_INT32(DBusMessageIter *iter, int value);
	    void AppendArgument_DOUBLE(DBusMessageIter *iter, double value);

	    void WaterDrop(bool isTest);

	signals:
    	    void searchingForTrayPosition(QPoint& pos);
	
	private slots:

	    void WaterControl(void);
	    void DetermineRootWindow(void);
	    void ModuleSelfTest(void);	
	    void chatWidgetActivated( ChatWidget* chat );
    	    void messageReceived( UserListElement user );
};

#endif // WATER_NOTIFY_H
