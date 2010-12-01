/****************************************************************************
*                                                                           *
*   GlobalHotkeys module for Kadu                                           *
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




#include <QApplication>
#include <QDesktopWidget>
#include <QMap>
#include <QMessageBox>
#include <QMenu>
#include <QLayout>
#include <QList>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QX11Info>

#include "buddies/buddy.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-preferred-manager.h"
#include "buddies/group-manager.h"
#include "chat/message/pending-messages-manager.h"
#include "chat/chat-manager.h"
#include "chat/recent-chat-manager.h"
#include "core/core.h"
#include "configuration/configuration-file.h"
#include "configuration/main-configuration.h"
#include "contacts/contact.h"
#include "contacts/contact-shared.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/status-menu.h"
#include "gui/windows/add-buddy-window.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/your-accounts.h"
#include "file-transfer/file-transfer-manager.h"
#include "misc/path-conversion.h"
#include "notify/notification-manager.h"
#include "status/status-container-manager.h"
#include "status/status-type.h"
#include "status/status-type-manager.h"
#include "activate.h"
#include "debug.h"
#include "icons-manager.h"
#include "modules.h"

#include "globalhotkeys.h"

#include "api.h"
#include "conf.h"
#include "functions.h"
#include "statusesmenu.h"

#include <X11/Xlib.h>
#include <X11/Xatom.h>




GlobalHotkeys *globalhotkeys;




extern "C" KADU_EXPORT int globalhotkeys_init( bool firstload )
{
	Q_UNUSED( firstload );
	kdebugf();
	// create new globalhotkeys object
	globalhotkeys = new GlobalHotkeys();
	// register UI file and handler
	MainConfigurationWindow::registerUiFile( dataPath("kadu/modules/configuration/globalhotkeys.ui") );
	MainConfigurationWindow::registerUiHandler( globalhotkeys );
	// done
	kdebugf2();
	return 0;
}


extern "C" KADU_EXPORT void globalhotkeys_close()
{
	kdebugf();
	// unregister UI file and handler
	MainConfigurationWindow::unregisterUiFile( dataPath("kadu/modules/configuration/globalhotkeys.ui") );
	MainConfigurationWindow::unregisterUiHandler( globalhotkeys );
	// delete globalhotkeys object
	delete globalhotkeys;
	globalhotkeys = NULL;
	kdebugf2();
}




void EmptyMsgHandler( QtMsgType type, const char *msg )
{
	Q_UNUSED( type );
	Q_UNUSED( msg );
	GlobalHotkeys::messageshandled = 1;
}




GlobalHotkeys::GlobalHotkeys() : QObject( 0 )
{
	INSTANCE = this;
	// create Functions
	new Functions();
	// create simple hotkeys
	new ConfHotKey( this, "Kadu"             , "Show Kadu's main window"           , "ShowKadusMainWindow"        , "functionShowKadusMainWindow"                  );
	new ConfHotKey( this, "Kadu"             , "Hide Kadu's main window"           , "HideKadusMainWindow"        , "functionHideKadusMainWindow"                  );
	new ConfHotKey( this, "Kadu"             , "Show/hide Kadu's main window"      , "ShowHideKadusMainWindow"    , "functionShowHideKadusMainWindow"    , "Alt+S" );
	new ConfHotKey( this, "Kadu"             , "Turn silent mode on"               , "TurnSilentModeOn"           , "functionTurnSilentModeOn"                     );
	new ConfHotKey( this, "Kadu"             , "Turn silent mode off"              , "TurnSilentModeOff"          , "functionTurnSilentModeOff"                    );
	new ConfHotKey( this, "Kadu"             , "Toggle silent mode"                , "ToggleSilentMode"           , "functionToggleSilentMode"                     );
	new ConfHotKey( this, "Kadu"             , "Quit Kadu"                         , "QuitKadu"                   , "functionQuitKadu"                             );
	new ConfHotKey( this, "Chats"            , "Open incoming chat window"         , "OpenIncomingChatWindow"     , "functionOpenIncomingChatWindow"     , "Alt+E" );
	new ConfHotKey( this, "Chats"            , "Open all incoming chat windows"    , "OpenAllIncomingChatWindows" , "functionOpenAllIncomingChatWindows"           );
	new ConfHotKey( this, "Chats"            , "Minimize all opened chat windows"  , "MinimizeOpenedChatWindows"  , "functionMinimizeOpenedChatWindows"            );
	new ConfHotKey( this, "Chats"            , "Restore all minimized chat windows", "RestoreMinimizedChatWindows", "functionRestoreMinimizedChatWindows"          );
	new ConfHotKey( this, "Chats"            , "Minimize/restore all chat windows" , "MinimizeRestoreChatWindows" , "functionMinimizeRestoreChatWindows"           );
	new ConfHotKey( this, "Chats"            , "Close all chat windows"            , "CloseAllChatWindows"        , "functionCloseAllChatWindows"                  );
	new ConfHotKey( this, "Chats"            , "Open chat with ..."                , "OpenChatWith"               , "functionOpenChatWith"               , "Alt+W" );
	new ConfHotKey( this, "Status"           , "Change status"                     , "ChangeStatus"               , "functionChangeStatus"               , "Alt+F" );
	new ConfHotKey( this, "Status"           , "Change description"                , "ChangeDescription"          , "functionChangeDescription"          , "Alt+D" );
	new ConfHotKey( this, "Buddies"          , "Add a new buddy"                   , "AddANewBuddy"               , "functionAddANewBuddy"                         );
	new ConfHotKey( this, "Buddies"          , "Search for buddy"                  , "SearchForBuddy"             , "functionSearchForBuddy"                       );
	new ConfHotKey( this, "Windows shortcuts", "File transfers window"             , "FileTransfersWindow"        , "functionFileTransfersWindow"                  );
	new ConfHotKey( this, "Windows shortcuts", "Configuration window"              , "ConfigurationWindow"        , "functionConfigurationWindow"                  );
	new ConfHotKey( this, "Windows shortcuts", "Account manager window"            , "AccountManagerWindow"       , "functionAccountManagerWindow"                 );
	new ConfHotKey( this, "Windows shortcuts", "Plugins window"                    , "PluginsWindow"              , "functionPluginsWindow"                        );
	// create config manager for BuddiesShotrcuts and BuddiesMenus
	new ConfManager( this );
	// create default configuration for BuddiesMenus
	if( config_file.readEntry( "GlobalHotkeys", "BuddiesMenus", QString::null ).isNull() )
	{
		ConfBuddiesMenu *confbuddiesmenu;
		confbuddiesmenu = new ConfBuddiesMenu( this, "Buddies menus", false );
		confbuddiesmenu->HOTKEY                       = HotKey( "Alt+Q" );
		confbuddiesmenu->CURRENTCHATS                 = true;
		confbuddiesmenu->PENDINGCHATS                 = true;
		confbuddiesmenu->RECENTCHATS                  = true;
		confbuddiesmenu->ONLINEBUDDIES                = false;
		confbuddiesmenu->ONLINEBUDDIESGROUPS          = QStringList();
		confbuddiesmenu->ONLINEBUDDIESINCLUDEBLOCKING = false;
		confbuddiesmenu->BUDDIES                      = QStringList();
		confbuddiesmenu->GROUPS                       = QStringList();
		confbuddiesmenu->EXCLUDEBUDDIES               = QStringList();
		confbuddiesmenu = new ConfBuddiesMenu( this, "Buddies menus", false );
		confbuddiesmenu->HOTKEY                       = HotKey( "Alt+A" );
		confbuddiesmenu->CURRENTCHATS                 = false;
		confbuddiesmenu->PENDINGCHATS                 = false;
		confbuddiesmenu->RECENTCHATS                  = false;
		confbuddiesmenu->ONLINEBUDDIES                = true;
		confbuddiesmenu->ONLINEBUDDIESGROUPS          = QStringList();
		confbuddiesmenu->ONLINEBUDDIESINCLUDEBLOCKING = true;
		confbuddiesmenu->BUDDIES                      = QStringList();
		confbuddiesmenu->GROUPS                       = QStringList();
		confbuddiesmenu->EXCLUDEBUDDIES               = QStringList();
	}
	// data
	DISPLAY = NULL;
	SHOWNGLOBALWIDGET = NULL;
	// create and connect() the hotkeys timer
	HOTKEYSTIMER = new QTimer();
	HOTKEYSTIMER->setSingleShot( true );
	connect( HOTKEYSTIMER, SIGNAL(timeout()), this, SLOT(checkPendingHotkeys()) );
	// start
	configurationUpdated();
}


GlobalHotkeys::~GlobalHotkeys()
{
	// stop the hotkeys timer
	HOTKEYSTIMER->stop();
	// close X11 display
	if( DISPLAY != NULL )
	{
		XCloseDisplay( DISPLAY );
	}
	// remove Conf*
	foreach( ConfHotKey* confhotkey, ConfHotKey::instances() )
		confhotkey->deleteLater();
	foreach( ConfBuddiesShortcut* confbuddiesshortcut, ConfBuddiesShortcut::instances() )
		confbuddiesshortcut->deleteLater();
	foreach( ConfBuddiesMenu* confbuddiesmenu, ConfBuddiesMenu::instances() )
		confbuddiesmenu->deleteLater();
	// remove configuration widgets and groups
	if( ! BUDDIESSHORTCUTSADDNEWBUTTON.isNull() )
		BUDDIESSHORTCUTSADDNEWBUTTON->deleteLater();
	if( ! BUDDIESMENUSADDNEWBUTTON.isNull() )
		BUDDIESMENUSADDNEWBUTTON->deleteLater();
	ConfGroups::deleteGroups();
	// remove Functions
	Functions::instance()->deleteLater();
}


int GlobalHotkeys::messageshandled = 0;


GlobalHotkeys *GlobalHotkeys::INSTANCE = NULL;


GlobalHotkeys *GlobalHotkeys::instance()
{
	return INSTANCE;
}


void GlobalHotkeys::mainConfigurationWindowCreated( MainConfigurationWindow *mainConfigurationWindow )
{
	// create groups boxes in the right order
	ConfGroups::createGroups();
	// add BUDDIESSHORTCUTSADDNEWBUTTON button
	if( BUDDIESSHORTCUTSADDNEWBUTTON == NULL )
	{
		ConfigGroupBox *groupBuddiesShortcuts = mainConfigurationWindow->widget()->configGroupBox( "Shortcuts", "Global hotkeys", "Buddies shortcuts" );
		BUDDIESSHORTCUTSADDNEWBUTTON = new ConfigActionButton( "Add new shortcut ...", "", groupBuddiesShortcuts, NULL );
		groupBuddiesShortcuts->widget()->layout()->removeWidget( BUDDIESSHORTCUTSADDNEWBUTTON );
		groupBuddiesShortcuts->addWidget( BUDDIESSHORTCUTSADDNEWBUTTON, true );  // re-insert the button so that it takes full available horizontal space
		connect( BUDDIESSHORTCUTSADDNEWBUTTON, SIGNAL(clicked()), this, SLOT(buddiesShortcutsAddNewButtonPressed()));
	}
	// add BUDDIESMENUSADDNEWBUTTON button
	if( BUDDIESMENUSADDNEWBUTTON == NULL )
	{
		ConfigGroupBox *groupBuddiesMenus = mainConfigurationWindow->widget()->configGroupBox( "Shortcuts", "Global hotkeys", "Buddies menus" );
		BUDDIESMENUSADDNEWBUTTON = new ConfigActionButton( "Add new menu ...", "", groupBuddiesMenus, NULL );
		groupBuddiesMenus->widget()->layout()->removeWidget( BUDDIESMENUSADDNEWBUTTON );
			groupBuddiesMenus->addWidget( BUDDIESMENUSADDNEWBUTTON, true );  // re-insert the button so that it takes full available horizontal space
		connect( BUDDIESMENUSADDNEWBUTTON, SIGNAL(clicked()), this, SLOT(buddiesMenusAddNewButtonPressed()) );
	}
	// emit signal
	emit( mainConfigurationWindowCreatedSignal( mainConfigurationWindow ) );
}


void GlobalHotkeys::configurationUpdated()
{
	// stop the hotkeys timer
	HOTKEYSTIMER->stop();
	// new X11 display
	if( DISPLAY != NULL )
	{
		XCloseDisplay( DISPLAY );
	}
	DISPLAY = XOpenDisplay( 0 );
	// grab hotkeys
	foreach( ConfHotKey* confhotkey, ConfHotKey::instances() )
	{
		grabHotKey( confhotkey->hotKey() );
	}
	foreach( ConfBuddiesShortcut* confbuddiesshortcut, ConfBuddiesShortcut::instances() )
		grabHotKey( confbuddiesshortcut->hotKey() );
	foreach( ConfBuddiesMenu* confbuddiesmenu, ConfBuddiesMenu::instances() )
		grabHotKey( confbuddiesmenu->hotKey() );
	// single shot the hotkeys timer
	HOTKEYSTIMER->start( GLOBALHOTKEYS_HOTKEYSTIMERINTERVAL );
}


void GlobalHotkeys::grabHotKey( HotKey hotkey )
{
	uint modifiers;
	if( ! hotkey.isNull() )
	{
		modifiers = 0;
		modifiers |= ( hotkey.shift()   ? GLOBALHOTKEYS_X11SHIFTMASK   : 0 );
		modifiers |= ( hotkey.control() ? GLOBALHOTKEYS_X11CONTROLMASK : 0 );
		modifiers |= ( hotkey.alt()     ? GLOBALHOTKEYS_X11ALTMASK     : 0 );
		modifiers |= ( hotkey.altGr()   ? GLOBALHOTKEYS_X11ALTGRMASK   : 0 );
		modifiers |= ( hotkey.super()   ? GLOBALHOTKEYS_X11SUPERMASK   : 0 );
		// install empty messages handler to avoid warning being printed to the output
		messageshandled = 0;
		QtMsgHandler previousmsghandler = qInstallMsgHandler( EmptyMsgHandler );
		// pure hotkey
		XGrabKey(
			DISPLAY, hotkey.keyCode(), modifiers,
			DefaultRootWindow( DISPLAY ), False, GrabModeAsync, GrabModeAsync
			);
		// hotkey with CapsLock
		XGrabKey(
			DISPLAY, hotkey.keyCode(), modifiers | GLOBALHOTKEYS_X11CAPSLOCKMASK,
			DefaultRootWindow( DISPLAY ), False, GrabModeAsync, GrabModeAsync
			);
		// hotkey with NumLock
		XGrabKey(
			DISPLAY, hotkey.keyCode(), modifiers | GLOBALHOTKEYS_X11NUMLOCKMASK,
			DefaultRootWindow( DISPLAY ), False, GrabModeAsync, GrabModeAsync
			);
		// hotkey with CapsLock and NumLock
		XGrabKey(
			DISPLAY, hotkey.keyCode(), modifiers | GLOBALHOTKEYS_X11CAPSLOCKMASK | GLOBALHOTKEYS_X11NUMLOCKMASK,
			DefaultRootWindow( DISPLAY ), False, GrabModeAsync, GrabModeAsync
			);
		// sync the X11 connection, so that we are sure the X11 errors will be handled now
		XSync( DISPLAY, False );
		// install the previous messages handler
		qInstallMsgHandler( previousmsghandler );
		// check for X11 errors
		if( messageshandled > 0 )
		{
			QMessageBox *messagebox = new QMessageBox(
					qApp->translate( "@default", "Kadu - Global hotkeys" ),
					qApp->translate( "@default", "Hotkey %% is being used by another application." ).replace( "%%", hotkey.string() ),
					QMessageBox::Warning,
					QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton,
					qApp->activeWindow()
				);
			messagebox->show();
			messageshandled = 0;
		}
	}
}


void GlobalHotkeys::buddiesShortcutsAddNewButtonPressed()
{
	ConfBuddiesShortcut *confbuddiesshortcut = new ConfBuddiesShortcut( this, "Buddies shortcuts", true );
	confbuddiesshortcut->focus();
}


void GlobalHotkeys::buddiesMenusAddNewButtonPressed()
{
	ConfBuddiesMenu *confbuddiesmenu = new ConfBuddiesMenu( this, "Buddies menus", true );
	confbuddiesmenu->focus();
}


void GlobalHotkeys::checkPendingHotkeys()
{
	// process pending hotkeys
	while( XPending( DISPLAY ) > 0 )
	{
		// get the next event
		XEvent event;
		XNextEvent( DISPLAY, &event );
		// is it KeyPress event?
		if( event.type == KeyPress )
		{
			// read modifiers state from the current event
			bool mod_shift   = ( ( event.xkey.state & GLOBALHOTKEYS_X11SHIFTMASK   ) != 0 );
			bool mod_control = ( ( event.xkey.state & GLOBALHOTKEYS_X11CONTROLMASK ) != 0 );
			bool mod_alt     = ( ( event.xkey.state & GLOBALHOTKEYS_X11ALTMASK     ) != 0 );
			bool mod_altgr   = ( ( event.xkey.state & GLOBALHOTKEYS_X11ALTGRMASK   ) != 0 );
			bool mod_super   = ( ( event.xkey.state & GLOBALHOTKEYS_X11SUPERMASK   ) != 0 );
			// create the hotkey
			HotKey hotkey( mod_shift, mod_control, mod_alt, mod_altgr, mod_super, event.xkey.keycode );
			// and process it
			processHotKey( hotkey );
		}
	}
	// single shot the hotkeys timer again
	HOTKEYSTIMER->start( GLOBALHOTKEYS_HOTKEYSTIMERINTERVAL );
}


bool GlobalHotkeys::processHotKey( HotKey hotkey )
{
	ConfHotKey *confhotkey = ConfHotKey::findByHotkey( hotkey );
	if( confhotkey != NULL )
	{
		QMetaObject::invokeMethod( Functions::instance(), confhotkey->function().toUtf8().data(), Q_ARG( ConfHotKey*, confhotkey ) );
		return true;
	}
	ConfBuddiesShortcut *confbuddiesshortcut = ConfBuddiesShortcut::findByHotkey( hotkey );
	if( confbuddiesshortcut != NULL )
	{
		processConfBuddiesShortcut( confbuddiesshortcut );
		return true;
	}
	ConfBuddiesMenu *confbuddiesmenu = ConfBuddiesMenu::findByHotkey( hotkey );
	if( confbuddiesmenu != NULL )
	{
		processConfBuddiesMenu( confbuddiesmenu );
		return true;
	}
	return false;
}


void GlobalHotkeys::processConfBuddiesShortcut( ConfBuddiesShortcut *confbuddiesshortcut )
{
	QList<Buddy> buddies;
	foreach( QString buddyname, confbuddiesshortcut->buddies() )
	{
		Buddy buddy = BuddyManager::instance()->byDisplay( buddyname, ActionReturnNull );
		if( ! buddy.isNull() )
		{
			buddies.append( buddy );
		}
	}
	if( buddies.count() == 0 )
		return;
	BuddySet buddyset;
	buddyset.unite( buddies.toSet() );
	QList<Account> accounts = Api::commonAccounts( buddyset );
	if( accounts.count() == 0 )
		return;
	if(
		( ! confbuddiesshortcut->showMenu() ) ||
		( ( buddyset.count() > 1 ) && ( accounts.count() == 1 ) ) ||
		( ( buddyset.count() == 1 ) && ( (*buddyset.begin()).contacts().count() == 1 ) )
	)
	{
		ContactSet contactset;
		foreach( Buddy buddy, buddyset )
		{
			contactset.insert( BuddyPreferredManager::instance()->preferredContact( buddy, accounts.first() ) );
		}
		Chat chat = ChatManager::instance()->findChat( contactset, true );
		ChatWidgetManager::instance()->openPendingMsgs( chat, true );
	}
	else
	{
		// close previous global widget, if any
		if( ! SHOWNGLOBALWIDGET.isNull() )
		{
			SHOWNGLOBALWIDGET->close();
			if( confbuddiesshortcut->hotKey() == SHOWNGLOBALWIDGETHOTKEY )
			{
				// last widget was this one - don't show it again
				SHOWNGLOBALWIDGET = NULL;
				SHOWNGLOBALWIDGETHOTKEY = HotKey();
				return;
			}
		}
		// create menu
		BuddiesMenu *menu = new BuddiesMenu();
		menu->setContactsSubmenu( false );
		if( buddyset.count() == 1 )
		{
			foreach( Contact contact, (*buddyset.begin()).contacts() )
				menu->add( contact );
		}
		else
		{
			foreach( Account account, accounts )
			{
				ContactSet contactset;
				foreach( Buddy buddy, buddyset )
				{
					contactset.insert( BuddyPreferredManager::instance()->preferredContact( buddy, account ) );
				}
				menu->add( contactset );
			}
		}
		// popup in the center of the screen
		menu->popup();
		// global data
		SHOWNGLOBALWIDGET = menu;
		SHOWNGLOBALWIDGETHOTKEY = confbuddiesshortcut->hotKey();
	}
}


void GlobalHotkeys::processConfBuddiesMenu( ConfBuddiesMenu *confbuddiesmenu )
{
	// close previous global widget, if any
	if( ! SHOWNGLOBALWIDGET.isNull() )
	{
		SHOWNGLOBALWIDGET->close();
		if( confbuddiesmenu->hotKey() == SHOWNGLOBALWIDGETHOTKEY )
		{
			// last widget was this one - don't show it again
			SHOWNGLOBALWIDGET = NULL;
			SHOWNGLOBALWIDGETHOTKEY = HotKey();
			return;
		}
	}
	// create menu
	BuddiesMenu *menu = new BuddiesMenu();
	menu->setContactsSubmenu( ! MainConfiguration::instance()->simpleMode() );
	// add currently open chats to the menu
	if( confbuddiesmenu->currentChats() && ( ! ChatWidgetManager::instance()->chats().isEmpty() ) )
	{
		// for each currently open chat
		foreach( ChatWidget *chatwidget, ChatWidgetManager::instance()->chats() )
		{
			QList<Contact> contacts = chatwidget->chat().contacts().toContactList();
			menu->add( contacts );
		}
	}
	// add chats with pending messages to the menu
	if( confbuddiesmenu->pendingChats() && ( PendingMessagesManager::instance()->hasPendingMessages() ) )
	{
		QList<ContactSet> contactsetlist;
		// for each pending message
		foreach( Message message, PendingMessagesManager::instance()->pendingMessages() )
		{
			ContactSet contactset = message.messageChat().contacts();
			menu->add( contactset.toContactList() );
		}
	}
	// add recently closed chats to the menu
	if( confbuddiesmenu->recentChats() && ( ! RecentChatManager::instance()->recentChats().isEmpty() ) )
	{
		// for each recently closed chat
		foreach( Chat chat, RecentChatManager::instance()->recentChats() )
		{
			menu->add( chat.contacts().toContactList() );
		}
	}
	// add online contatcts to the menu
	if( confbuddiesmenu->onlineBuddies() )
	{
		QStringList onlinestatusgroups;
		onlinestatusgroups << "Online" << "Away" << "Invisible";
		foreach( Buddy buddy, BuddyManager::instance()->items() )
		{
			Contact contact = BuddyPreferredManager::instance()->preferredContact( buddy );
			if( confbuddiesmenu->onlineBuddiesGroups().count() > 0 )
			{
				bool include = false;
				foreach( Group group, buddy.groups() )
				{
					if( confbuddiesmenu->onlineBuddiesGroups().contains( group.name() ) )
					{
						include = true;
						break;
					}
				}
				if( ! include )
					continue;
			}
			if( onlinestatusgroups.contains( contact.currentStatus().group() ) )
			{
				menu->add( contact );
			}
			else if( confbuddiesmenu->onlineBuddiesIncludeBlocking() )
			{
				foreach( Contact contact, buddy.contacts() )
				if( contact.isBlocking() )
				{
					menu->add( contact );
					break;
				}
			}
		}
	}
	// add given buddies to the menu
	foreach( QString buddydisplay, confbuddiesmenu->buddies() )
	{
		Buddy buddy = BuddyManager::instance()->byDisplay( buddydisplay, ActionReturnNull );
		if( ! buddy.isNull() )
			menu->add( BuddyPreferredManager::instance()->preferredContact( buddy ) );
	}
	// add buddies from given groups to the menu
	if( ! confbuddiesmenu->groups().isEmpty() )
	{
		foreach( Buddy buddy, BuddyManager::instance()->items() )
		{
			foreach( Group group, buddy.groups() )
			{
				if( confbuddiesmenu->groups().contains( group.name() ) )
				{
					menu->add( BuddyPreferredManager::instance()->preferredContact( buddy ) );
					break;
				}
			}
		}
	}
	// popup in the center of the screen
	menu->popup();
	// global data
	SHOWNGLOBALWIDGET = menu;
	SHOWNGLOBALWIDGETHOTKEY = confbuddiesmenu->hotKey();
}
