/***************************************************************************
 *   Copyright (C) 2008-2010                                               *
 *     Michał Małek  michalm@jabster.pl                                    *
 *     Piotr Dąbrowski  ultr@ultr.pl                                       *
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


#ifndef LEDNOTIFY_H
	#define LEDNOTIFY_H


#include <QSet>

#include "gui/widgets/chat-widget.h"
#include "gui/windows/main-configuration-window.h"
#include "notify/notification.h"
#include "notify/notifier.h"

#include "ledblinker.h"


class LedNotify : public Notifier, ConfigurationUiHandler
{
	Q_OBJECT
	public:
		LedNotify( QObject *parent = 0 );
		~LedNotify();
		virtual void copyConfiguration( const QString &fromEvent, const QString &toEvent );
		virtual void mainConfigurationWindowCreated( MainConfigurationWindow *mainconfigurationwindow );
		virtual NotifierConfigurationWidget *createConfigurationWidget( QWidget *widget );
		virtual void notify( Notification *notification );
	private:
		typedef QSet<ChatWidget*> ChatWidgets;
		LedBlinker blinker_;
		ChatWidgets msgChats_;
		bool chatBlinking_;
		bool msgBlinking_;
	private slots:
		/**
		 *	Checks if all new messages are received so we could stop blinking
		 */
		void messageReceived( Message messaget );
		/**
		 *	Checks if all chats with new messages are activated so we could stop blinking
		 */
		void chatWidgetActivated( ChatWidget* chatwidget );
};


extern LedNotify *lednotify;


#endif
