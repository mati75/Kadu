/***************************************************************************
 *   Copyright (C) 2008 by Michał Małek                                    *
 *   michalm at the jabster.pl                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef LED_NOTIFY_H
#define LED_NOTIFY_H

#include "led_blinker.h"
#include "main_configuration_window.h"
#include "modules/notify/notify.h"
#include <QSet>

class ChatWidget;

class LedNotify : public Notifier, ConfigurationUiHandler
{
	Q_OBJECT
	
	public:
		LedNotify(QObject* parent = 0, const char* name = 0);
		~LedNotify();
		
		virtual void copyConfiguration( const QString &fromEvent, const QString &toEvent ) {}
		virtual void mainConfigurationWindowCreated(MainConfigurationWindow*) {}
		virtual NotifierConfigurationWidget* createConfigurationWidget(QWidget*, char*);
		virtual void notify( Notification *notification );

	private:
		typedef QSet<ChatWidget*> ChatWidgets;
		LedBlinker blinker_;
		ChatWidgets msgChats_;
		bool chatBlinking_;
		bool msgBlinking_;
		
	private slots:
		/**
		 *	Checks if all new chats are opened
		 *	so we could stop blinking
		 */
		void messageReceived( UserListElement user );
		
		/**
		 *	Checks if all chats with new messages are activated
		 *	so we could stop blinking.
		 */
		void chatWidgetActivated( ChatWidget* chat );
};

extern LedNotify* led_notify;

#endif // LED_NOTIFY_H
