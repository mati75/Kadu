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

#include "led_notify.h"
#include "chat_manager.h"
#include "config_file.h"
#include "configuration_window_widgets.h"
#include "custom_input.h"
#include "debug.h"
#include "exports.h"
#include "pending_msgs.h"

LedNotify* led_notify;

extern "C" KADU_EXPORT int led_notify_init( bool firstLoad )
{
	led_notify = new LedNotify();
	return 0;
}

extern "C" KADU_EXPORT void led_notify_close()
{
	delete led_notify;
	led_notify = 0;
}

LedNotify::LedNotify(QObject* parent, const char* name)
:
	Notifier(parent, name),
	chatBlinking_( false ),
	msgBlinking_( false )
{
	config_file.addVariable("Led Notify", "LEDdelay", 500);
	config_file.addVariable("Led Notify", "LEDcount", 3);
	
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/led_notify.ui"), this);
	notification_manager->registerNotifier( "Scroll Lock LED", this );
	connect( &pending, SIGNAL(messageFromUserDeleted(UserListElement)), this, SLOT(messageReceived(UserListElement)) );
	connect( chat_manager, SIGNAL(chatWidgetActivated(ChatWidget*)), this, SLOT(chatWidgetActivated(ChatWidget*)) );
	connect( chat_manager, SIGNAL(chatWidgetDestroying(ChatWidget*)), this, SLOT(chatWidgetActivated(ChatWidget*)) );
}

LedNotify::~LedNotify()
{
	disconnect( chat_manager, SIGNAL(chatWidgetActivated(ChatWidget*)), this, SLOT(chatWidgetActivated(ChatWidget*)) );
	disconnect( chat_manager, SIGNAL(chatWidgetDestroying(ChatWidget*)), this, SLOT(chatWidgetActivated(ChatWidget*)) );
	disconnect( &pending, SIGNAL(messageFromUserDeleted(UserListElement)), this, SLOT(messageReceived(UserListElement)) );
	notification_manager->unregisterNotifier("Scroll Lock LED");
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/led_notify.ui"), this);
}

NotifierConfigurationWidget* LedNotify::createConfigurationWidget(QWidget*, char*)
{
	return 0;
}

void LedNotify::notify( Notification *notification )
{
	kdebugf();
	if( "NewChat" == notification->type() )
	{
		// Don't blink, if "OpenChatOnMessage" is "true" - chat is already open
		if( !config_file.readBoolEntry( "Chat", "OpenChatOnMessage" ) )
		{
			chatBlinking_ = true;
			blinker_.startInfinite();
		}
	}
	else if( "NewMessage" == notification->type() )
	{
		const UserListElements& senders = notification->userListElements();
		ChatWidget* chat = chat_manager->findChatWidget( senders );
		if( chat != 0 )
		{
			if( !chat->edit()->hasFocus() )
			{
				msgChats_.insert( chat );
				msgBlinking_ = true;
				blinker_.startInfinite();
			}
			else if( !config_file.readBoolEntry("Notify", "NewMessageOnlyIfInactive") )
			{
				blinker_.startFinite();
			}
		}
	}
	else
	{
		blinker_.startFinite();
	}
	kdebugf2();
}

void LedNotify::messageReceived( UserListElement user )
{
	kdebugf();
	// Check if we can stop blinking from "NewChat" event...
	if( chatBlinking_ && !pending.pendingMsgs() )
	{
		chatBlinking_ = false;
		// ...and make sure "NewMessage" blinking is not running
		if( !msgBlinking_ )
		{
			blinker_.stop();
		}
	}
	kdebugf2();
}

void LedNotify::chatWidgetActivated( ChatWidget* chat )
{
	kdebugf();
	msgChats_.remove( chat );
	
	// Check if we can stop blinking from "NewMessage" event...
	if( msgBlinking_ && msgChats_.empty() )
	{
		msgBlinking_ = false;
		// ...and make sure "NewChat" blinking is not running
		if( !chatBlinking_ )
		{
			blinker_.stop();
		}
	}
	kdebugf2();
}
