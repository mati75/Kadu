/***************************************************************************
 *   Copyright (C) 2008-2011                                               *
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




#include "chat/message/pending-messages-manager.h"
#include "configuration/configuration-file.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/custom-input.h"
#include "misc/misc.h"
#include "notify/chat-notification.h"
#include "notify/notification-manager.h"
#include "activate.h"
#include "debug.h"
#include "exports.h"

#include "lednotify.h"




LedNotify *lednotify;




extern "C" KADU_EXPORT int lednotify_init( bool firstLoad )
{
	Q_UNUSED( firstLoad );
	lednotify = new LedNotify();
	return 0;
}


extern "C" KADU_EXPORT void lednotify_close()
{
	delete lednotify;
	lednotify = NULL;
}


LedNotify::LedNotify( QObject *parent ) : QObject( parent ), Notifier( "lednotify", "Scroll Lock LED", IconsManager::instance()->iconByPath("kadu_icons/notify-led") ), chatBlinking_( false ), msgBlinking_( false )
{
	config_file.addVariable( "Led Notify", "LEDdelay", 500 );
	config_file.addVariable( "Led Notify", "LEDcount",   3 );
	MainConfigurationWindow::registerUiFile( dataPath( "kadu/modules/configuration/lednotify.ui" ) );
	NotificationManager::instance()->registerNotifier( this );
	connect( PendingMessagesManager::instance(), SIGNAL(messageRemoved(Message))          , this, SLOT(messageReceived(Message))         );
	connect( ChatWidgetManager::instance()     , SIGNAL(chatWidgetActivated(ChatWidget*)) , this, SLOT(chatWidgetActivated(ChatWidget*)) );
	connect( ChatWidgetManager::instance()     , SIGNAL(chatWidgetDestroying(ChatWidget*)), this, SLOT(chatWidgetActivated(ChatWidget*)) );
}


LedNotify::~LedNotify()
{
	disconnect( ChatWidgetManager::instance()     , SIGNAL(chatWidgetActivated(ChatWidget*)) , this, SLOT(chatWidgetActivated(ChatWidget*)) );
	disconnect( ChatWidgetManager::instance()     , SIGNAL(chatWidgetDestroying(ChatWidget*)), this, SLOT(chatWidgetActivated(ChatWidget*)) );
	disconnect( PendingMessagesManager::instance(), SIGNAL(messageRemoved(Message))          , this, SLOT(messageReceived(Message))         );
	NotificationManager::instance()->unregisterNotifier( this );
	MainConfigurationWindow::unregisterUiFile( dataPath( "kadu/modules/configuration/lednotify.ui" ) );
}


NotifierConfigurationWidget* LedNotify::createConfigurationWidget( QWidget *widget )
{
	Q_UNUSED( widget );
	return 0;
}


void LedNotify::notify( Notification *notification )
{
	kdebugf();
	if( notification->type() == "NewChat" )
	{
		// Don't blink, if "OpenChatOnMessage" is "true" - chat is already open
		if( ! config_file.readBoolEntry( "Chat", "OpenChatOnMessage" ) )	
		{
			chatBlinking_ = true;
			blinker_.startInfinite();
		}
	}
	else if( notification->type() == "NewMessage" )
	{
		ChatNotification *chatnotification = dynamic_cast<ChatNotification*>( notification );
		if( chatnotification != NULL )
		{
			ChatWidget* chat = ChatWidgetManager::instance()->byChat( chatnotification->chat(), false );
			if( chat != NULL )
			{
				printf( "1\n" );
				if( ! _isActiveWindow( chat->window() ) )
				{
					printf( "2\n" );
					msgChats_.insert( chat );
					msgBlinking_ = true;
					blinker_.startInfinite();
				}
				else if( ! config_file.readBoolEntry( "Notify", "NewMessageOnlyIfInactive" ) )
				{
					blinker_.startFinite();
				}
			}
		}
	}
	else
	{
		blinker_.startFinite();
	}
	kdebugf2();
}


void LedNotify::messageReceived( Message message )
{
	Q_UNUSED( message );
	kdebugf();
	// Check if we can stop blinking from "NewChat" event...
	if( chatBlinking_ && ( ! PendingMessagesManager::instance()->hasPendingMessages() ) )
	{
		chatBlinking_ = false;
		// ...and make sure "NewMessage" blinking is not running
		if( ! msgBlinking_ )
			blinker_.stop();
	}
	kdebugf2();
}


void LedNotify::chatWidgetActivated( ChatWidget *chatwidget )
{
	kdebugf();
	msgChats_.remove( chatwidget );
	// Check if we can stop blinking from "NewMessage" event...
	if( msgBlinking_ && msgChats_.empty() )
	{
		msgBlinking_ = false;
		// ...and make sure "NewChat" blinking is not running
		if( ! chatBlinking_ )
			blinker_.stop();
	}
	kdebugf2();
}


void LedNotify::copyConfiguration( const QString &fromEvent, const QString &toEvent )
{
	Q_UNUSED( fromEvent );
	Q_UNUSED( toEvent );
}


void LedNotify::mainConfigurationWindowCreated(MainConfigurationWindow* mainconfigurationwindow)
{
	Q_UNUSED( mainconfigurationwindow );
}
