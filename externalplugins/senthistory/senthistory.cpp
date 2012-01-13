/****************************************************************************
*                                                                           *
*   SentHistory plugin for Kadu                                             *
*   Copyright (C) 2008-2011  Piotr Dąbrowski ultr@ultr.pl                   *
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
#include "gui/windows/main-configuration-window.h"
#include "gui/hot-key.h"
#include "misc/misc.h"
#include "debug.h"

#include "senthistory.h"




int SentHistory::init( bool firstLoad )
{
	Q_UNUSED( firstLoad );
	kdebugf();
	MainConfigurationWindow::registerUiFile( dataPath("kadu/plugins/configuration/senthistory.ui") );
	kdebugf2();
	return 0;
}


void SentHistory::done()
{
	kdebugf();
	MainConfigurationWindow::unregisterUiFile( dataPath("kadu/plugins/configuration/senthistory.ui") );
	kdebugf2();
}


QList< QPair<Chat,QString> > SentHistory::sentmessages;


SentHistory::SentHistory()
{
	// configuration handling
	createDefaultConfiguration();
	// read the configuration and force its usage
	configurationUpdated();
	// connect chat widgets events and handle opened ones
	connect( ChatWidgetManager::instance(), SIGNAL(chatWidgetCreated(ChatWidget*))   , this, SLOT(chatCreated(ChatWidget*))    );
	connect( ChatWidgetManager::instance(), SIGNAL(chatWidgetDestroying(ChatWidget*)), this, SLOT(chatDestroying(ChatWidget*)) );
	foreach( ChatWidget *chatwidget, ChatWidgetManager::instance()->chats() )
		chatCreated( chatwidget );
}


SentHistory::~SentHistory()
{
	// disconnect chat widgets events
	disconnect( ChatWidgetManager::instance(), SIGNAL(chatWidgetCreated(ChatWidget*))   , this, SLOT(chatCreated(ChatWidget*))    );
	disconnect( ChatWidgetManager::instance(), SIGNAL(chatWidgetDestroying(ChatWidget*)), this, SLOT(chatDestroying(ChatWidget*)) );
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
	Chat chat = chatwidget->chat();
	if( ! messagen.contains( chat ) )
		messagen[chat] = 0;
	if( ! thischatonly.contains( chat ) )
		thischatonly[chat] = true;
	if( ! currentmessage.contains( chat ) )
		currentmessage[chat] = QString();
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
	messagen[chat] = 0;
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
	Chat chat = chatwidget->chat();
	// local sent messages' history
	if( HotKey::shortCut( e, "SentHistory", "PreviousMessage") )
	{
		if( messagen[chat] == 0 )
			currentmessage[chat] = chatwidget->edit()->toHtml();
		if( thischatonly[chat] == false )
			messagen[chat] = 0;  // start from the begining
		thischatonly[chat] = true;
		messagen[chat]++;  // previous message ( 1 is first )
		inputMessage( chatwidget );
		handled = true;
		return;
	}
	if( HotKey::shortCut( e, "SentHistory", "NextMessage") )
	{
		if( thischatonly[chat] == false )
			messagen[chat] = 0;  // start from the begining
		thischatonly[chat] = true;
		messagen[chat]--;  // next message
		inputMessage( chatwidget );
		handled = true;
		return;
	}
	// global sent messages' history
	if( HotKey::shortCut( e, "SentHistory", "PreviousMessageFromAllChats") )
	{
		if( messagen[chat] == 0 )
			currentmessage[chat] = chatwidget->edit()->toHtml();
		if( thischatonly[chat] == true )
			messagen[chat] = 0;  // start from the begining
		thischatonly[chat] = false;
		messagen[chat]++;  // previous message ( 1 is first )
		inputMessage( chatwidget );
		handled = true;
		return;
	}
	if( HotKey::shortCut( e, "SentHistory", "NextMessageFromAllChats") )
	{
		if( thischatonly[chat] == true )
			messagen[chat] = 0;  // start from the begining
		thischatonly[chat] = false;
		messagen[chat]--;  // next message
		inputMessage( chatwidget );
		handled = true;
		return;
	}
}


void SentHistory::inputMessage( ChatWidget* chatwidget )
{
	Chat chat = chatwidget->chat();
	if( messagen[chat] <= 0 )  // message out of range
	{
		// current message
		messagen[chat] = 0;
		chatwidget->edit()->setHtml( currentmessage[chat] );
		chatwidget->edit()->moveCursor( QTextCursor::End );
		return;
	}
	if( messagen[chat] > (int)(sentmessages.count()) )  // message out of range
	{
		// previous, valid message
		messagen[chat]--;
		return;
	}
	if( thischatonly[chat] )  // input only messages sent to this chat
	{
		// find requested message in chat's sentmessages
		int foundmessage = 0;
		QListIterator< QPair<Chat,QString> > it( sentmessages );
		while( it.hasNext() )
		{
			if( it.peekNext().first == chat )
			{
				foundmessage++;
				if( foundmessage == messagen[chat] )
				{
					chatwidget->edit()->setHtml( it.peekNext().second );
					chatwidget->edit()->moveCursor( QTextCursor::End );
					break;
				}
			}
			it.next();
		}
		if( foundmessage < messagen[chat] )
		{
			// previous, valid message
			messagen[chat]--;
			return;
		}
	}
	else  // input all sent messages
	{
		chatwidget->edit()->setHtml( sentmessages[ messagen[chat] - 1 ].second );  // 1 is the first message
		chatwidget->edit()->moveCursor( QTextCursor::End );
	}
}




Q_EXPORT_PLUGIN2( senthistory, SentHistory )
