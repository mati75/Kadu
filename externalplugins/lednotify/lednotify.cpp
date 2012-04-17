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




#include "chat/chat-manager.h"
#include "configuration/configuration-file.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/custom-input.h"
#include "message/message-manager.h"
#include "misc/misc.h"
#include "notify/chat-notification.h"
#include "notify/notification-manager.h"
#include "activate.h"
#include "debug.h"
#include "exports.h"

#include "leddriver.h"

#include "lednotify.h"




int LedNotify::init( bool firstLoad )
{
	Q_UNUSED( firstLoad );
	return 0;
}


void LedNotify::done()
{
}


LedNotify::LedNotify() :
	Notifier( "lednotify", QT_TRANSLATE_NOOP( "@default", "LED" ), KaduIcon( "kadu_icons/notify-led" ) ),
	chatBlinking_( false ), msgBlinking_( false )
{
	config_file.addVariable( "LedNotify", "LEDdiode", LedDriver::DiodeScrollLock );
	config_file.addVariable( "LedNotify", "LEDdelay",                        500 );
	config_file.addVariable( "LedNotify", "LEDcount",                          3 );
	MainConfigurationWindow::registerUiFile( dataPath( "kadu/plugins/configuration/lednotify.ui" ) );
	NotificationManager::instance()->registerNotifier( this );
	connect( MessageManager::instance()   , SIGNAL(unreadMessageRemoved(Message))    , this, SLOT(messageReceived(Message))          );
	connect( ChatManager::instance()      , SIGNAL(chatUpdated(const Chat&))         , this, SLOT(chatUpdated(const Chat&))          );
	connect( ChatWidgetManager::instance(), SIGNAL(chatWidgetDestroying(ChatWidget*)), this, SLOT(chatWidgetDestroying(ChatWidget*)) );
}


LedNotify::~LedNotify()
{
	disconnect( MessageManager::instance()   , SIGNAL(unreadMessageRemoved(Message))    , this, SLOT(messageReceived(Message))          );
	disconnect( ChatManager::instance()      , SIGNAL(chatUpdated(const Chat&))         , this, SLOT(chatUpdated(const Chat&))          );
	disconnect( ChatWidgetManager::instance(), SIGNAL(chatWidgetDestroying(ChatWidget*)), this, SLOT(chatWidgetDestroying(ChatWidget*)) );
	NotificationManager::instance()->unregisterNotifier( this );
	MainConfigurationWindow::unregisterUiFile( dataPath( "kadu/plugins/configuration/lednotify.ui" ) );
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
			Chat chat = chatnotification->chat();
			ChatWidget* chatwidget = ChatWidgetManager::instance()->byChat( chat, false );
			if( chatwidget != NULL )
			{
				if( ! _isActiveWindow( chatwidget->window() ) )
				{
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
	if( chatBlinking_ && ( ! MessageManager::instance()->hasUnreadMessages() ) )
	{
		chatBlinking_ = false;
		// ...and make sure "NewMessage" blinking is not running
		if( ! msgBlinking_ )
			blinker_.stop();
	}
	kdebugf2();
}


void LedNotify::chatUpdated( const Chat &chat )
{
	kdebugf();
	if( chat.unreadMessagesCount() == 0 )
		chatRead( chat );
	kdebugf2();
}


void LedNotify::chatWidgetDestroying( ChatWidget *chatwidget )
{
	kdebugf();
	chatRead( chatwidget->chat() );
	kdebugf2();
}


void LedNotify::chatRead( const Chat &chat )
{
	kdebugf();
	msgChats_.remove( chat );
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




Q_EXPORT_PLUGIN2( lednotify, LedNotify )
