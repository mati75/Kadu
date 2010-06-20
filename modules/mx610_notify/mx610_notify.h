/**
*
* Project:     Logitech MX610 Notify for Kadu
* File:        mx610_notify.h
* Copyright:   (C) 2006-2008 by Korneliusz Jrzebski
* Author:      Korneliusz Jarzebski <korneliusz at jarzebski dot pl>
* Modified by: Tomasz Rostanski <rozteck at interia dot pl>
* Link:        http://www.jarzebski.pl/
* Version:     0.4.1
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

#ifndef MX610_NOTIFY_H
#define MX610_NOTIFY_H

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QStringList>

#include "../notify/notify.h"
#include "../notify/notification.h"

#include "pending_msgs.h"

class ChatWidget;

class MX610ConfigurationWidget : public NotifierConfigurationWidget
{
	Q_OBJECT

	QString currentNotifyEvent;

	QMap<QString, int> LEDSelects;
	QMap<QString, int> ModeSelects;
	
	QComboBox *LEDComboBox;
	QComboBox *ModeComboBox;	

public:
	MX610ConfigurationWidget(QWidget *parent = 0, char *name = 0);
	virtual ~MX610ConfigurationWidget();

	virtual void loadNotifyConfigurations() {}
	virtual void saveNotifyConfigurations();
	virtual void switchToEvent(const QString &event);
};

class MX610Notify : public Notifier, public ConfigurationUiHandler
{
	Q_OBJECT

public:
	MX610Notify(QObject *parent = 0, const char *name = 0);
	~MX610Notify();

	virtual NotifierConfigurationWidget *createConfigurationWidget(QWidget *parent = 0, char *name = 0);

        virtual void copyConfiguration( const QString &fromEvent, const QString &toEvent ) {}
        virtual void mainConfigurationWindowCreated(MainConfigurationWindow*);
	virtual void notify(Notification *notification);

private:

	QTimer* timer;
	
	bool IM_LED_On, EM_LED_On, 
	     IM_LED_Disable, EM_LED_Disable;

	QString IM_LED_Owner, EM_LED_Owner;
	
	QTime IM_LED_Timestamp, EM_LED_Timestamp;

	QList<ChatWidget*> msgChats_;
	
	static unsigned char MX_Confirm[6];
        static unsigned char MX_IM_On[6];
        static unsigned char MX_IM_FastOn[6];
        static unsigned char MX_IM_Blink[6];
        static unsigned char MX_IM_Pulse[6];
        static unsigned char MX_IM_Off[6];
        static unsigned char MX_IM_FastOff[6];
        static unsigned char MX_EM_On[6];
        static unsigned char MX_EM_FastOn[6];
        static unsigned char MX_EM_Blink[6];
        static unsigned char MX_EM_Pulse[6];
        static unsigned char MX_EM_Off[6];
        static unsigned char MX_EM_FastOff[6];
 
	void SendToMX610( unsigned char *command );
	
private slots:

	void LEDControl(void);
	void BatteryControl(void);
	void ModuleSelfTest(void);
	
	void chatWidgetActivated( ChatWidget* chat );
        void messageReceived( UserListElement user );

public slots:

};

#endif // MX610_NOTIFY_H
