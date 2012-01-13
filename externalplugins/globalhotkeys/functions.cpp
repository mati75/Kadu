/****************************************************************************
*                                                                           *
*   GlobalHotkeys plugin for Kadu                                           *
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




#include <QApplication>
#include <QDesktopWidget>

#include "functions.h"

#include "buddies/buddy-manager.h"
#include "buddies/group-manager.h"
#include "chat/message/pending-messages-manager.h"
#include "chat/chat-manager.h"
#include "chat/recent-chat-manager.h"
#include "core/core.h"
#include "configuration/configuration-file.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/status-menu.h"
#include "gui/windows/add-buddy-window.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/multilogon-window.h"
#include "gui/windows/search-window.h"
#include "gui/windows/your-accounts.h"
#include "file-transfer/file-transfer-manager.h"
#include "icons/icons-manager.h"
#include "misc/path-conversion.h"
#include "notify/notification-manager.h"
#include "plugins/plugins-manager.h"
#include "status/status-container-manager.h"
#include "status/status-type.h"
#include "status/status-type-manager.h"
#include "activate.h"
#include "debug.h"

#include "globalhotkeys.h"
#include "globalwidgetmanager.h"
#include "statusesmenu.h"




Functions *Functions::INSTANCE = NULL;


Functions::Functions( QObject *parent ) : QObject( parent )
{
	INSTANCE = this;
}


Functions::~Functions()
{
	INSTANCE = NULL;
}


Functions *Functions::instance()
{
	if( INSTANCE == NULL )
		new Functions();
	return INSTANCE;
}


void Functions::functionShowKadusMainWindow( ConfHotKey *confhotkey )
{
	Q_UNUSED( confhotkey );
	_activateWindow( Core::instance()->kaduWindow() );
}


void Functions::functionHideKadusMainWindow( ConfHotKey *confhotkey )
{
	Q_UNUSED( confhotkey );
	if( Core::instance()->kaduWindow()->isVisible() )
	{
		// hide Kadu's main window
		Core::instance()->kaduWindow()->hide();
	}
}


void Functions::functionShowHideKadusMainWindow( ConfHotKey *confhotkey )
{
	Q_UNUSED( confhotkey );
	QWidget *window = Core::instance()->kaduWindow();
	if( ! window->isVisible() )
	{
		// show and activate Kadu's main window
		window->show();
		_activateWindow( window );
	}
	else if( window->isVisible() && ( ! _isActiveWindow( window ) ) )
	{
		// activate Kadu's main window
		_activateWindow( window );
	}
	else if( window->isVisible() && ( _isActiveWindow( window ) ) )
	{
		// hide Kadu's main window
		window->hide();
	}
}


void Functions::functionOpenIncomingChatWindow( ConfHotKey *confhotkey )
{
	Q_UNUSED( confhotkey );
	if( PendingMessagesManager::instance()->hasPendingMessages() )
	{
		// open window for pending message(s)
		ChatWidgetManager::instance()->openPendingMessages( true );
		// activate it
		QWidget *win = ChatWidgetManager::instance()->chats().values().last();  // last created chat widget
		win = win->window();
		_activateWindow( win );
	}
	else
	{
		// show window with new unread message(s)
		foreach( ChatWidget *chatwidget, ChatWidgetManager::instance()->chats() )
		{
			if( chatwidget->newMessagesCount() > 0 )
			{
				chatwidget->show();
				_activateWindow( chatwidget->window() );
				// done - only one window
				break;
			}
		}
	}
}


void Functions::functionOpenAllIncomingChatWindows( ConfHotKey *confhotkey )
{
	Q_UNUSED( confhotkey );
	// open all windows for pending message(s)
	while( PendingMessagesManager::instance()->hasPendingMessages() )
	{
		// open the window
		ChatWidgetManager::instance()->openPendingMessages( true );
		// activate it
		QWidget *win = ChatWidgetManager::instance()->chats().values().last();  // last created chat widget
		win = win->window();
		_activateWindow( win );
	}
	// show all windows with new unread message(s)
	foreach( ChatWidget *chatwidget, ChatWidgetManager::instance()->chats() )
	{
		if( chatwidget->newMessagesCount() > 0 )
		{
			chatwidget->show();
			_activateWindow( chatwidget->window() );
		}
	}
}


void Functions::functionMinimizeOpenedChatWindows( ConfHotKey *confhotkey )
{
	Q_UNUSED( confhotkey );
	// minimize all windows (if needed)
	foreach( ChatWidget *chatwidget, ChatWidgetManager::instance()->chats() )
	{
		if( ! chatwidget->window()->isMinimized() )
			chatwidget->window()->showMinimized();
	}
}


void Functions::functionRestoreMinimizedChatWindows( ConfHotKey *confhotkey )
{
	Q_UNUSED( confhotkey );
	// restore all windows (if needed) and activate them
	foreach( ChatWidget *chatwidget, ChatWidgetManager::instance()->chats() )
	{
		if( chatwidget->window()->isMinimized() )
			chatwidget->window()->showNormal();
	}
}


void Functions::functionMinimizeRestoreChatWindows( ConfHotKey *confhotkey )
{
	Q_UNUSED( confhotkey );
	// check if all windows are minimized already
	bool allwindowsminimized = true;
	foreach( ChatWidget *chatwidget, ChatWidgetManager::instance()->chats() )
	{
		if( ! chatwidget->window()->isMinimized() )
			allwindowsminimized = false;
	}
	if( ! allwindowsminimized )
	{
		// minimize all windows (if needed)
		foreach( ChatWidget *chatwidget, ChatWidgetManager::instance()->chats() )
		{
			if( ! chatwidget->window()->isMinimized() )
				chatwidget->window()->showMinimized();
		}
	}
	else
	{
		// restore all windows (if needed) and activate them
		foreach( ChatWidget *chatwidget, ChatWidgetManager::instance()->chats() )
		{
			if( chatwidget->window()->isMinimized() )
				chatwidget->window()->showNormal();
			_activateWindow( chatwidget->window() );
		}
	}
}


void Functions::functionCloseAllChatWindows( ConfHotKey *confhotkey )
{
	Q_UNUSED( confhotkey );
	// list of windows to close (needed by Tabs module - we cannot close the same window multiple times!)
	QList<QWidget*> wins;
	foreach( ChatWidget *chatwidget, ChatWidgetManager::instance()->chats() )
	{
		// add the window to the list
		if( wins.contains( chatwidget->window() ) == 0 )  // if this window is not on the list yet
			wins.append( chatwidget->window() );
	}
	// close the windows from the list
	for( QList<QWidget*>::Iterator I = wins.begin(); I != wins.end(); I++ )
	{
		(*I)->close();
	}
}


void Functions::functionOpenChatWith( ConfHotKey *confhotkey )
{
	// close previous global widget, if any
	if( ! GlobalHotkeys::instance()->SHOWNGLOBALWIDGET.isNull() )
	{
		GlobalHotkeys::instance()->SHOWNGLOBALWIDGET->close();
		if( confhotkey->hotKey() == GlobalHotkeys::instance()->SHOWNGLOBALWIDGETHOTKEY )
		{
			// last widget was this one - don't show it again
			GlobalHotkeys::instance()->SHOWNGLOBALWIDGET = NULL;
			GlobalHotkeys::instance()->SHOWNGLOBALWIDGETHOTKEY = HotKey();
			return;
		}
	}
	// open the window
	OpenChatWith *window = OpenChatWith::instance();
	window->move(
			( qApp->desktop()->screenGeometry().width()  - window->width()  ) / 2,
			( qApp->desktop()->screenGeometry().height() - window->height() ) / 2
		);
	window->show();
	new GlobalWidgetManager( window ); // popup in the center of the screen
	// global data
	GlobalHotkeys::instance()->SHOWNGLOBALWIDGET = window;
	GlobalHotkeys::instance()->SHOWNGLOBALWIDGETHOTKEY = confhotkey->hotKey();
}


void Functions::functionTurnSilentModeOn( ConfHotKey *confhotkey )
{
	Q_UNUSED( confhotkey );
	NotificationManager::instance()->setSilentMode( true );
}


void Functions::functionTurnSilentModeOff( ConfHotKey *confhotkey )
{
	Q_UNUSED( confhotkey );
	NotificationManager::instance()->setSilentMode( false );
}


void Functions::functionToggleSilentMode( ConfHotKey *confhotkey )
{
	Q_UNUSED( confhotkey );
	NotificationManager::instance()->setSilentMode( ! NotificationManager::instance()->silentMode() );
}


void Functions::functionQuitKadu( ConfHotKey *confhotkey )
{
	Q_UNUSED( confhotkey );
	qApp->quit();
}


void Functions::functionChangeStatus( ConfHotKey *confhotkey )
{
	// close previous global widget, if any
	if( ! GlobalHotkeys::instance()->SHOWNGLOBALWIDGET.isNull() )
	{
		GlobalHotkeys::instance()->SHOWNGLOBALWIDGET->close();
		if( confhotkey->hotKey() == GlobalHotkeys::instance()->SHOWNGLOBALWIDGETHOTKEY )
		{
			// last widget was this one - don't show it again
			GlobalHotkeys::instance()->SHOWNGLOBALWIDGET = NULL;
			GlobalHotkeys::instance()->SHOWNGLOBALWIDGETHOTKEY = HotKey();
			return;
		}
	}
	// create menu
	StatusesMenu *statusesmenu = new StatusesMenu();
	// popup in the center of the screen
	statusesmenu->popup();
	// global data
	GlobalHotkeys::instance()->SHOWNGLOBALWIDGET = statusesmenu;
	GlobalHotkeys::instance()->SHOWNGLOBALWIDGETHOTKEY = confhotkey->hotKey();
}


void Functions::functionChangeDescription( ConfHotKey *confhotkey )
{
	// close previous global widget, if any
	if( ! GlobalHotkeys::instance()->SHOWNGLOBALWIDGET.isNull() )
	{
		GlobalHotkeys::instance()->SHOWNGLOBALWIDGET->close();
		if( confhotkey->hotKey() == GlobalHotkeys::instance()->SHOWNGLOBALWIDGETHOTKEY )
		{
			// last widget was this one - don't show it again
			GlobalHotkeys::instance()->SHOWNGLOBALWIDGET = NULL;
			GlobalHotkeys::instance()->SHOWNGLOBALWIDGETHOTKEY = HotKey();
			return;
		}
	}
	StatusContainer *statuscontainer =
		( StatusContainerManager::instance()->statusContainers().count() == 1 ) ?
		StatusContainerManager::instance()->statusContainers()[0] : StatusContainerManager::instance();
	ChooseDescription *dialog = ChooseDescription::showDialog( statuscontainer );
	new GlobalWidgetManager( dialog );
	// global data
	GlobalHotkeys::instance()->SHOWNGLOBALWIDGET = dialog;
	GlobalHotkeys::instance()->SHOWNGLOBALWIDGETHOTKEY = confhotkey->hotKey();
}


void Functions::functionAddANewBuddy( ConfHotKey *confhotkey )
{
	Q_UNUSED( confhotkey );
	AddBuddyWindow *addbuddywindow = new AddBuddyWindow( NULL );
	addbuddywindow->show();
	_activateWindow( addbuddywindow );
}


void Functions::functionSearchForBuddy( ConfHotKey *confhotkey )
{
	Q_UNUSED( confhotkey );
	SearchWindow *searchwindow = new SearchWindow( NULL );
	searchwindow->show();
	_activateWindow( searchwindow );
}


void Functions::functionFileTransfersWindow( ConfHotKey *confhotkey )
{
	Q_UNUSED( confhotkey );
	FileTransferManager::instance()->showFileTransferWindow();
}


void Functions::functionMultilogonWindow( ConfHotKey *confhotkey )
{
	Q_UNUSED( confhotkey );
	MultilogonWindow::instance()->show();
}


void Functions::functionConfigurationWindow( ConfHotKey *confhotkey )
{
	Q_UNUSED( confhotkey );
	MainConfigurationWindow::instance()->show();
	_activateWindow( MainConfigurationWindow::instance() );
}


void Functions::functionAccountManagerWindow( ConfHotKey *confhotkey )
{
	Q_UNUSED( confhotkey );
	YourAccounts::instance()->show();
	_activateWindow( YourAccounts::instance() );
}


void Functions::functionPluginsWindow( ConfHotKey *confhotkey )
{
	Q_UNUSED( confhotkey );
	PluginsManager::instance()->showWindow( NULL, false );
}
