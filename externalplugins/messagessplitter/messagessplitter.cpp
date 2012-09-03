/****************************************************************************
*                                                                           *
*   MessagesSplitter plugin for Kadu                                        *
*   Copyright (C) 2011-2012  Piotr Dąbrowski ultr@ultr.pl                   *
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




#include <unistd.h>

#include <QApplication>
#include <QTextDocumentFragment>

#include "accounts/account.h"
#include "configuration/configuration-file.h"
#include "gui/widgets/chat-edit-box.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/custom-input.h"
#include "misc/kadu-paths.h"
#include "misc/misc.h"
#include "debug.h"

#include "plugins/encryption_ng/encryption-chat-data.h"
#include "plugins/encryption_ng/encryption-manager.h"
#include "plugins/encryption_ng/encryption-provider-manager.h"

#include "messagessplitter.h"

#include "defines.h"




int MessagesSplitter::init( bool firstLoad )
{
	Q_UNUSED( firstLoad );
	kdebugf();
	MainConfigurationWindow::registerUiFile( KaduPaths::instance()->dataPath() + "plugins/configuration/messagessplitter.ui" );
	kdebugf2();
	return 0;
}


void MessagesSplitter::done()
{
	kdebugf();
	MainConfigurationWindow::unregisterUiFile( KaduPaths::instance()->dataPath() + "plugins/configuration/messagessplitter.ui" );
	kdebugf2();
}


MessagesSplitter::MessagesSplitter()
{
	partssendinglock = false;
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


MessagesSplitter::~MessagesSplitter()
{
	// disconnect chat widgets events
	disconnect( ChatWidgetManager::instance(), SIGNAL(chatWidgetCreated(ChatWidget*))   , this, SLOT(chatCreated(ChatWidget*))    );
	disconnect( ChatWidgetManager::instance(), SIGNAL(chatWidgetDestroying(ChatWidget*)), this, SLOT(chatDestroying(ChatWidget*)) );
}


void MessagesSplitter::mainConfigurationWindowCreated( MainConfigurationWindow *mainConfigurationWindow )
{
	Q_UNUSED( mainConfigurationWindow );
}


void MessagesSplitter::createDefaultConfiguration()
{
	config_file.addVariable( "MessagesSplitter", "SmartMessagesSplitting", true                           );
	config_file.addVariable( "MessagesSplitter", "Delay"                 , MESSAGESSPLITTER_DEFAULT_DELAY );
	config_file.addVariable( "MessagesSplitter", "RemoveWhitespaces"     , false                          );
}


void MessagesSplitter::configurationUpdated()
{
}


void MessagesSplitter::chatCreated( ChatWidget *chatwidget )
{
	connect( chatwidget, SIGNAL( messageSendRequested( ChatWidget* ) ), this, SLOT( messageSendRequested( ChatWidget* ) ) );
}


void MessagesSplitter::chatDestroying( ChatWidget *chatwidget )
{
	disconnect( chatwidget, SIGNAL( messageSendRequested( ChatWidget* ) ), this, SLOT( messageSendRequested( ChatWidget* ) ) );
}


void MessagesSplitter::messageSendRequested( ChatWidget *chatwidget )
{
	// check parts sending
	if( partssendinglock )
		return;
	CustomInput *edit = chatwidget->edit();
	// whitespace removal
	if( config_file.readBoolEntry( "MessagesSplitter", "RemoveWhitespaces", false ) )
	{
		edit->setUpdatesEnabled( false );
		QTextOption::WrapMode wordwrapmode = edit->wordWrapMode();
		edit->setWordWrapMode( QTextOption::NoWrap );
		QTextEdit::LineWrapMode linewrapmode = edit->lineWrapMode();
		edit->setLineWrapMode( QTextEdit::NoWrap );
		edit->moveCursor( QTextCursor::Start );
		while( true )
		{
			edit->moveCursor( QTextCursor::EndOfLine );
			do
			{
				edit->moveCursor( QTextCursor::Left, QTextCursor::KeepAnchor );
			} while(
				( ! edit->textCursor().selectedText().contains( QChar(0x2028) ) ) &&
				( ! edit->textCursor().selectedText().contains( QChar(0x2029) ) ) &&
				edit->textCursor().selectedText().trimmed().isEmpty()
			);
			edit->moveCursor( QTextCursor::Right, QTextCursor::KeepAnchor );
			if( edit->textCursor().hasSelection() )
				edit->textCursor().removeSelectedText();
			edit->moveCursor( QTextCursor::EndOfLine );
			if( edit->textCursor().atEnd() )
				break;
			edit->moveCursor( QTextCursor::Down );
		}
		edit->setWordWrapMode( wordwrapmode );
		edit->setLineWrapMode( linewrapmode );
		edit->setUpdatesEnabled( true );
	}
	// check protocol
	if( chatwidget->chat().chatAccount().protocolName() != "gadu" )
		return;
	// data
	int maxlength = MESSAGESSPLITTER_MAXIMUM_LENGTH_NORMAL;
	int maxbackward = MESSAGESSPLITTER_MAXIMUM_BACKWARD;
	// check encryption
	if( EncryptionProviderManager::instance()->canEncrypt( chatwidget->chat() ) && EncryptionManager::instance()->chatEncryption( chatwidget->chat() )->encrypt() )
		maxlength = MESSAGESSPLITTER_MAXIMUM_LENGTH_ENCRYPTED;
	// start
	edit->setUpdatesEnabled( false );
	QTextOption::WrapMode wordwrapmode = edit->wordWrapMode();
	edit->setWordWrapMode( QTextOption::NoWrap );
	QTextEdit::LineWrapMode linewrapmode = edit->lineWrapMode();
	edit->setLineWrapMode( QTextEdit::NoWrap );
	QList<QTextDocumentFragment> messages;
	while( ! edit->toPlainText().isEmpty() )
	{
		// move cursor to maxlength
		edit->moveCursor( QTextCursor::Start );
		while( true )
		{
			edit->moveCursor( QTextCursor::Right, QTextCursor::KeepAnchor );
			int l = edit->textCursor().selectedText().toUtf8().length();
			if( ( l >= maxlength ) || edit->textCursor().atEnd() )
			{
				if( l > maxlength )
					edit->moveCursor( QTextCursor::Left, QTextCursor::KeepAnchor );
				break;
			}
		}
		QTextCursor tc;
		tc = edit->textCursor();
		tc.clearSelection();
		edit->setTextCursor( tc );
		// smart splitting
		if( ( ! edit->textCursor().atEnd() ) && config_file.readBoolEntry( "MessagesSplitter", "SmartMessagesSplitting", false ) )
		{
			while( true )
			{
				int position = edit->textCursor().position();
				if( position <= maxbackward )
					break;
				QTextCursor tc;
				// block
				edit->moveCursor( QTextCursor::StartOfBlock );
				if( ( edit->textCursor().position() > 0 ) && ( position - edit->textCursor().position() < maxbackward ) )
				{
					edit->textCursor().deletePreviousChar();
					break;
				}
				tc = edit->textCursor();
				tc.setPosition( position );
				edit->setTextCursor( tc );
				// line
				edit->moveCursor( QTextCursor::StartOfLine );
				if( ( edit->textCursor().position() > 0 ) && ( position - edit->textCursor().position() < maxbackward ) )
				{
					edit->textCursor().deletePreviousChar();
					break;
				}
				tc = edit->textCursor();
				tc.setPosition( position );
				edit->setTextCursor( tc );
				// word
				edit->moveCursor( QTextCursor::StartOfWord );
				if( ( edit->textCursor().position() > 0 ) && ( position - edit->textCursor().position() < maxbackward ) )
				{
					break;
				}
				tc = edit->textCursor();
				tc.setPosition( position );
				edit->setTextCursor( tc );
				break;
			}
		}
		// select part, save it and remove
		edit->moveCursor( QTextCursor::Start, QTextCursor::KeepAnchor );
		messages.append( edit->textCursor().selection() );
		edit->textCursor().removeSelectedText();
	}
	// send
	partssendinglock = true;
	for( int k = 0; k < messages.count(); k++ )
	{
		edit->clear();
		edit->textCursor().insertFragment( messages[k] );
		if( k < messages.count() - 1 )
		{
			chatwidget->sendMessage();
			wait( config_file.readNumEntry( "MessagesSplitter", "Delay", MESSAGESSPLITTER_DEFAULT_DELAY ) );
		}
	}
	partssendinglock = false;
	// done
	edit->setWordWrapMode( wordwrapmode );
	edit->setLineWrapMode( linewrapmode );
	edit->setUpdatesEnabled( true );
}


void MessagesSplitter::wait( int milliseconds )
{
	int t = 100;
	do
	{
		usleep( qMin( t, milliseconds ) * 1000 );
		qApp->blockSignals( true );
		qApp->processEvents( QEventLoop::ExcludeUserInputEvents );
		qApp->blockSignals( false );
		milliseconds -= t;
	} while( milliseconds > 0 );
}




Q_EXPORT_PLUGIN2( messagessplitter, MessagesSplitter )
