/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "chat_manager.h"
#include "config_file.h"
#include "debug.h"
#include "hot_key.h"
#include "kadu.h"
#include "misc.h"

#include "senthistory.h"


SentHistory *senthistory;


extern "C" int senthistory_init()
{
	kdebugf();
	senthistory = new SentHistory();
	MainConfigurationWindow::registerUiFile( dataPath("kadu/modules/configuration/senthistory.ui"), senthistory );
	kdebugf2();
	return 0;
}


extern "C" void senthistory_close()
{
	kdebugf();
	MainConfigurationWindow::unregisterUiFile( dataPath("kadu/modules/configuration/senthistory.ui"), senthistory );
	delete senthistory;
	senthistory = NULL;
	kdebugf2();
}


QList< QPair<UserListElements,QString> > SentHistory::sentmessages;


SentHistory::SentHistory() : QObject( NULL, "senthistory" )
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
	connect( chat_manager, SIGNAL(chatWidgetCreated(ChatWidget*)), this, SLOT(chatCreated(ChatWidget*)));
	connect( chat_manager, SIGNAL(chatWidgetDestroying(ChatWidget*)), this, SLOT(chatDestroying(ChatWidget*)));
	foreach( ChatWidget *it, chat_manager->chats() )
	{
		chatCreated( it );
	}
}


SentHistory::~SentHistory()
{
	// disconnect chat widgets events
	disconnect( chat_manager, SIGNAL(chatWidgetCreated(ChatWidget*)), this, SLOT(chatCreated(ChatWidget*)));
	disconnect( chat_manager, SIGNAL(chatWidgetDestroying(ChatWidget*)), this, SLOT(chatDestroying(ChatWidget*)));
}


void SentHistory::mainConfigurationWindowCreated( MainConfigurationWindow *mainConfigurationWindow )
{
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
	connect( chatwidget, SIGNAL( messageSendRequested( ChatWidget* ) ), this, SLOT( messageSendRequested( ChatWidget* ) ) );
	connect( chatwidget, SIGNAL( keyPressed( QKeyEvent*, ChatWidget*, bool& ) ), this, SLOT( editKeyPressed( QKeyEvent*, ChatWidget*, bool& )) );
}


void SentHistory::chatDestroying( ChatWidget *chatwidget )
{
	// disconnect chat's events
	disconnect( chatwidget, SIGNAL( messageSendRequested( ChatWidget* ) ), this, SLOT( messageSendRequested( ChatWidget* ) ) );
	disconnect( chatwidget, SIGNAL( keyPressed( QKeyEvent*, ChatWidget*, bool& ) ), this, SLOT( editKeyPressed( QKeyEvent*, ChatWidget*, bool& )) );
}


void SentHistory::messageSendRequested( ChatWidget *chat )
{
	// read users and message
	UserListElements users = chat->users()->toUserListElements();
	QString sentmessage = chat->edit()->text();
	// reset message number
	message_n = 0;
	// find last message in this chat
	QListIterator< QPair<UserListElements,QString> > it( sentmessages );
	while( it.hasNext() )
	{
		if( it.peekNext().first.equals( users ) )
		{
			if( it.peekNext().second != sentmessage )  // the previous message in this chat is not equal to current one
			{
				// insert current message
				sentmessages.push_front( QPair<UserListElements,QString>( users, sentmessage ) );
			}
			return;
		}
		it.next();
	}
	// this user's sent history is empty - insert current message
	sentmessages.push_front( QPair<UserListElements,QString>( users, sentmessage ) );
}


void SentHistory::editKeyPressed( QKeyEvent* e, ChatWidget* sender, bool &handled )
{
	// check sender
	if( sender != lastChatWidget )
	{
		lastChatWidget = sender;
		message_n = 0;
	}
	// local sent messages' history
	if( HotKey::shortCut( e, "SentHistory", "PreviousMessage") )
	{
		if( thischatonly == false ) message_n = 0;  // start from the begining
		thischatonly = true;
		message_n++;  // next message ( "1" is first )
		inputMessage( sender );
		handled = true;
		return;
	}
	if( HotKey::shortCut( e, "SentHistory", "NextMessage") )
	{
		if( thischatonly == false ) message_n = 0;  // start from the begining
		thischatonly = true;
		message_n--;  // previous message
		inputMessage( sender );
		handled = true;
		return;
	}
	// global sent messages' history
	else if( HotKey::shortCut( e, "SentHistory", "PreviousMessageFromAllChats") )
	{
		if( thischatonly == true ) message_n = 0;  // start from the begining
		thischatonly = false;
		message_n++;  // next message ( "1" is first )
		inputMessage( sender );
		handled = true;
		return;
	}
	else if( HotKey::shortCut( e, "SentHistory", "NextMessageFromAllChats") )
	{
		if( thischatonly == true ) message_n = 0;  // start from the begining
		thischatonly = false;
		message_n--;  // previous message
		inputMessage( sender );
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
	UserListElements thischatusers = chatwidget->users()->toUserListElements();
	if( message_n <= 0 )  // message out of range
	{
		// last, empty message
		chatwidget->edit()->setText( "" );
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
		QListIterator< QPair<UserListElements,QString> > it( sentmessages );
		while( it.hasNext() )
		{
			if( it.peekNext().first.equals( thischatusers ) )
			{
				foundmessage++;
				if( foundmessage == message_n )
				{
					chatwidget->edit()->setText( it.peekNext().second );
					chatwidget->edit()->moveCursor( QTextEdit::MoveEnd, false );
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
		chatwidget->edit()->setText( sentmessages[ message_n - 1 ].second );  // message_n==1 is the first message
		chatwidget->edit()->moveCursor( QTextEdit::MoveEnd, false );
	}
}
