/****************************************************************************
*                                                                           *
*   SentHistory module for Kadu                                             *
*   Copyright (C) 2008-2010  Piotr Dąbrowski ultr@ultr.pl                   *
*                                                                           *
*   This program is free software: you can redistribute it and/or modify    *
*   it under the terms of the GNU General Public License as published by    *
*   the Free Software Foundation, either version 3 of the License, or       *
*   (at your option) any later version.                                     *
*                                                                           *
*   This program is distributed in the hope that it will be useful,         *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
*   GNU General Public License for more details.                            *
*                                                                           *
*   You should have received a copy of the GNU General Public License       *
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
*                                                                           *
****************************************************************************/




#include "chat/chat.h"
#include "chat/chat-manager.h"
#include "configuration/configuration-file.h"
#include "gui/widgets/chat-edit-box.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/windows/kadu-window.h"
#include "gui/hot-key.h"
#include "misc/misc.h"
#include "debug.h"

#include "senthistory.h"




SentHistory *senthistory;




extern "C" int senthistory_init()
{
	kdebugf();
	senthistory = new SentHistory();
	MainConfigurationWindow::registerUiFile( dataPath("kadu/modules/configuration/senthistory.ui") );
	kdebugf2();
	return 0;
}


extern "C" void senthistory_close()
{
	kdebugf();
	MainConfigurationWindow::unregisterUiFile( dataPath("kadu/modules/configuration/senthistory.ui") );
	delete senthistory;
	senthistory = NULL;
	kdebugf2();
}


QList< QPair<Chat,QString> > SentHistory::sentmessages;


SentHistory::SentHistory() : QObject()
{
	// configuration handling
	createDefaultConfiguration();
	// read the configuration and force its usage
	configurationUpdated();
	// variables' values
	lastChatWidget = NULL;
	message_n = 0;
	thischatonly = true;
	// connect chat widgets events and handle opened ones
	connect( ChatWidgetManager::instance(), SIGNAL(chatWidgetCreated(ChatWidget*))   , this, SLOT(chatCreated(ChatWidget*))    );
	connect( ChatWidgetManager::instance(), SIGNAL(chatWidgetDestroying(ChatWidget*)), this, SLOT(chatDestroying(ChatWidget*)) );
	foreach( ChatWidget *chatwidget, ChatWidgetManager::instance()->chats() )
	{
		chatCreated( chatwidget );
	}
}


SentHistory::~SentHistory()
{
	// disconnect chat widgets events
	disconnect( ChatWidgetManager::instance(), SIGNAL(chatWidgetCreated(ChatWidget*))   , this, SLOT(chatCreated(ChatWidget*))    );
	disconnect( ChatWidgetManager::instance(), SIGNAL(chatWidgetDestroying(ChatWidget*)), this, SLOT(chatDestroying(ChatWidget*)) );
}


void SentHistory::mainConfigurationWindowCreated( MainConfigurationWindow *mainConfigurationWindow )
{
	Q_UNUSED( mainConfigurationWindow );
}


void SentHistory::createDefaultConfiguration()
{
	config_file.addVariable( "SentHistory", "PreviousMessage"            , SENTHISTORY_DEFAULTSHORTCUT_PREVIOUSMESSAGE             );
	config_file.addVariable( "SentHistory", "NextMessage"                , SENTHISTORY_DEFAULTSHORTCUT_NEXTMESSAGE                 );
	config_file.addVariable( "SentHistory", "PreviousMessageFromAllChats", SENTHISTORY_DEFAULTSHORTCUT_PREVIOUSMESSAGEFROMALLCHATS );
	config_file.addVariable( "SentHistory", "NextMessageFromAllChats"    , SENTHISTORY_DEFAULTSHORTCUT_NEXTMESSAGEFROMALLCHATS     );
}


void SentHistory::configurationUpdated()
{
}


void SentHistory::chatCreated( ChatWidget *chatwidget )
{
	// connect new chat's events
	connect( chatwidget                  , SIGNAL( messageSendRequested( ChatWidget* ) )          , this, SLOT( messageSendRequested( ChatWidget* ) )               );
	connect( chatwidget->getChatEditBox(), SIGNAL( keyPressed( QKeyEvent*, CustomInput*, bool& ) ), this, SLOT( editKeyPressed( QKeyEvent*, CustomInput*, bool& ) ) );
}

void SentHistory::chatDestroying( ChatWidget *chatwidget )
{
	// disconnect chat's events
	disconnect( chatwidget                  , SIGNAL( messageSendRequested( ChatWidget* ) )          , this, SLOT( messageSendRequested( ChatWidget* ) )               );
	disconnect( chatwidget->getChatEditBox(), SIGNAL( keyPressed( QKeyEvent*, CustomInput*, bool& ) ), this, SLOT( editKeyPressed( QKeyEvent*, CustomInput*, bool& ) ) );
}


void SentHistory::messageSendRequested( ChatWidget *chatwidget )
{
	// read contacts and message
	Chat chat = chatwidget->chat();
	QString sentmessage = chatwidget->edit()->toHtml();
	// reset message number
	message_n = 0;
	// find last message in this chat
	QListIterator< QPair<Chat,QString> > it( sentmessages );
	while( it.hasNext() )
	{
		if( it.peekNext().first == chat )
		{
			if( it.peekNext().second != sentmessage )  // the previous message in this chat is not equal to current one
			{
				// insert current message
				sentmessages.push_front( QPair<Chat,QString>( chat, sentmessage ) );
			}
			return;
		}
		it.next();
	}
	// this user's sent history is empty - insert current message
	sentmessages.push_front( QPair<Chat,QString>( chat, sentmessage ) );
}


void SentHistory::editKeyPressed( QKeyEvent* e, CustomInput* custominput, bool &handled )
{
	Q_UNUSED( custominput );
	ChatEditBox *chateditbox = dynamic_cast<ChatEditBox*>( sender() );
	if( chateditbox == NULL )
		return;
	ChatWidget *chatwidget = chateditbox->chatWidget();
	// check chatwidget
	if( chatwidget != lastChatWidget )
	{
		lastChatWidget = chatwidget;
		message_n = 0;
	}
	// local sent messages' history
	if( HotKey::shortCut( e, "SentHistory", "PreviousMessage") )
	{
		if( thischatonly == false ) message_n = 0;  // start from the begining
		thischatonly = true;
		message_n++;  // next message ( "1" is first )
		inputMessage( chatwidget );
		handled = true;
		return;
	}
	if( HotKey::shortCut( e, "SentHistory", "NextMessage") )
	{
		if( thischatonly == false ) message_n = 0;  // start from the begining
		thischatonly = true;
		message_n--;  // previous message
		inputMessage( chatwidget );
		handled = true;
		return;
	}
	// global sent messages' history
	else if( HotKey::shortCut( e, "SentHistory", "PreviousMessageFromAllChats") )
	{
		if( thischatonly == true ) message_n = 0;  // start from the begining
		thischatonly = false;
		message_n++;  // next message ( "1" is first )
		inputMessage( chatwidget );
		handled = true;
		return;
	}
	else if( HotKey::shortCut( e, "SentHistory", "NextMessageFromAllChats") )
	{
		if( thischatonly == true ) message_n = 0;  // start from the begining
		thischatonly = false;
		message_n--;  // previous message
		inputMessage( chatwidget );
		handled = true;
		return;
	}
	// any other key
	else
	{
		// reset message number
		message_n = 0;
	}
}


void SentHistory::inputMessage( ChatWidget* chatwidget )
{
	Chat thischat = chatwidget->chat();
	if( message_n <= 0 )  // message out of range
	{
		// last, empty message
		chatwidget->edit()->setHtml( "" );
		message_n = 0;
		return;
	}
	if( message_n > (int)(sentmessages.count()) )  // message out of range
	{
		// previous, valid message
		message_n = message_n - 1;
		return;
	}
	if( thischatonly )  // input only messages sent to this chat
	{
		// find requested message in chat's sentmessages
		int foundmessage = 0;
		QListIterator< QPair<Chat,QString> > it( sentmessages );
		while( it.hasNext() )
		{
			if( it.peekNext().first == thischat )
			{
				foundmessage++;
				if( foundmessage == message_n )
				{
					chatwidget->edit()->setHtml( it.peekNext().second );
					chatwidget->edit()->moveCursor( QTextCursor::End );
					break;
				}
			}
			it.next();
		}
		if( foundmessage < message_n )
		{
			message_n = message_n - 1;
			return;
		}
	}
	else  // input all sent messages
	{
		// message_n-1 is in range, checked alraedy
		chatwidget->edit()->setHtml( sentmessages[ message_n - 1 ].second );  // message_n==1 is the first message
		chatwidget->edit()->moveCursor( QTextCursor::End );
	}
}
