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
#include "configuration/main-configuration-holder.h"
#include "contacts/contact.h"
#include "contacts/contact-shared.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/status-menu.h"
#include "gui/windows/add-buddy-window.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/your-accounts.h"
#include "file-transfer/file-transfer-manager.h"
#include "icons/icons-manager.h"
#include "misc/path-conversion.h"
#include "notify/notification-manager.h"
#include "status/status-container-manager.h"
#include "status/status-type.h"
#include "status/status-type-manager.h"
#include "activate.h"
#include "debug.h"

#include "globalhotkeys.h"

#include "api.h"
#include "conf.h"
#include "functions.h"
#include "statusesmenu.h"

#include <X11/Xlib.h>
#include <X11/Xatom.h>




int GlobalHotkeys::init( bool firstload )
{
	Q_UNUSED( firstload );
	kdebugf();
	MainConfigurationWindow::registerUiFile( dataPath("kadu/plugins/configuration/globalhotkeys.ui") );
	MainConfigurationWindow::registerUiHandler( this );
	kdebugf2();
	return 0;
}


void GlobalHotkeys::done()
{
	kdebugf();
	MainConfigurationWindow::unregisterUiHandler( this );
	MainConfigurationWindow::unregisterUiFile( dataPath("kadu/plugins/configuration/globalhotkeys.ui") );
	kdebugf2();
}


GlobalHotkeys::GlobalHotkeys()
{
	INSTANCE = this;
	// create Functions
	new Functions();
	// create simple hotkeys
	new ConfHotKey( this, QT_TRANSLATE_NOOP( "@default", "Kadu"              ), QT_TRANSLATE_NOOP( "@default", "Show Kadu's main window"            ), "ShowKadusMainWindow"        , "functionShowKadusMainWindow"                  );
	new ConfHotKey( this, QT_TRANSLATE_NOOP( "@default", "Kadu"              ), QT_TRANSLATE_NOOP( "@default", "Hide Kadu's main window"            ), "HideKadusMainWindow"        , "functionHideKadusMainWindow"                  );
	new ConfHotKey( this, QT_TRANSLATE_NOOP( "@default", "Kadu"              ), QT_TRANSLATE_NOOP( "@default", "Show/hide Kadu's main window"       ), "ShowHideKadusMainWindow"    , "functionShowHideKadusMainWindow"    , "Alt+S" );
	new ConfHotKey( this, QT_TRANSLATE_NOOP( "@default", "Kadu"              ), QT_TRANSLATE_NOOP( "@default", "Turn silent mode on"                ), "TurnSilentModeOn"           , "functionTurnSilentModeOn"                     );
	new ConfHotKey( this, QT_TRANSLATE_NOOP( "@default", "Kadu"              ), QT_TRANSLATE_NOOP( "@default", "Turn silent mode off"               ), "TurnSilentModeOff"          , "functionTurnSilentModeOff"                    );
	new ConfHotKey( this, QT_TRANSLATE_NOOP( "@default", "Kadu"              ), QT_TRANSLATE_NOOP( "@default", "Toggle silent mode"                 ), "ToggleSilentMode"           , "functionToggleSilentMode"                     );
	new ConfHotKey( this, QT_TRANSLATE_NOOP( "@default", "Kadu"              ), QT_TRANSLATE_NOOP( "@default", "Quit Kadu"                          ), "QuitKadu"                   , "functionQuitKadu"                             );
	new ConfHotKey( this, QT_TRANSLATE_NOOP( "@default", "Chats"             ), QT_TRANSLATE_NOOP( "@default", "Open incoming chat's window"        ), "OpenIncomingChatWindow"     , "functionOpenIncomingChatWindow"     , "Alt+E" );
	new ConfHotKey( this, QT_TRANSLATE_NOOP( "@default", "Chats"             ), QT_TRANSLATE_NOOP( "@default", "Open all incoming chats' windows"   ), "OpenAllIncomingChatWindows" , "functionOpenAllIncomingChatWindows"           );
	new ConfHotKey( this, QT_TRANSLATE_NOOP( "@default", "Chats"             ), QT_TRANSLATE_NOOP( "@default", "Minimize all opened chat windows"   ), "MinimizeOpenedChatWindows"  , "functionMinimizeOpenedChatWindows"            );
	new ConfHotKey( this, QT_TRANSLATE_NOOP( "@default", "Chats"             ), QT_TRANSLATE_NOOP( "@default", "Restore all minimized chat windows" ), "RestoreMinimizedChatWindows", "functionRestoreMinimizedChatWindows"          );
	new ConfHotKey( this, QT_TRANSLATE_NOOP( "@default", "Chats"             ), QT_TRANSLATE_NOOP( "@default", "Minimize/restore all chat windows"  ), "MinimizeRestoreChatWindows" , "functionMinimizeRestoreChatWindows"           );
	new ConfHotKey( this, QT_TRANSLATE_NOOP( "@default", "Chats"             ), QT_TRANSLATE_NOOP( "@default", "Close all chat windows"             ), "CloseAllChatWindows"        , "functionCloseAllChatWindows"                  );
	new ConfHotKey( this, QT_TRANSLATE_NOOP( "@default", "Chats"             ), QT_TRANSLATE_NOOP( "@default", "Open chat with ..."                 ), "OpenChatWith"               , "functionOpenChatWith"               , "Alt+W" );
	new ConfHotKey( this, QT_TRANSLATE_NOOP( "@default", "Status"            ), QT_TRANSLATE_NOOP( "@default", "Change status"                      ), "ChangeStatus"               , "functionChangeStatus"               , "Alt+F" );
	new ConfHotKey( this, QT_TRANSLATE_NOOP( "@default", "Status"            ), QT_TRANSLATE_NOOP( "@default", "Change description"                 ), "ChangeDescription"          , "functionChangeDescription"          , "Alt+D" );
	new ConfHotKey( this, QT_TRANSLATE_NOOP( "@default", "Buddies"           ), QT_TRANSLATE_NOOP( "@default", "Add a new buddy"                    ), "AddANewBuddy"               , "functionAddANewBuddy"                         );
	new ConfHotKey( this, QT_TRANSLATE_NOOP( "@default", "Buddies"           ), QT_TRANSLATE_NOOP( "@default", "Search for buddy"                   ), "SearchForBuddy"             , "functionSearchForBuddy"                       );
	new ConfHotKey( this, QT_TRANSLATE_NOOP( "@default", "Windows shortcuts" ), QT_TRANSLATE_NOOP( "@default", "File transfers window"              ), "FileTransfersWindow"        , "functionFileTransfersWindow"                  );
	new ConfHotKey( this, QT_TRANSLATE_NOOP( "@default", "Windows shortcuts" ), QT_TRANSLATE_NOOP( "@default", "Configuration window"               ), "ConfigurationWindow"        , "functionConfigurationWindow"                  );
	new ConfHotKey( this, QT_TRANSLATE_NOOP( "@default", "Windows shortcuts" ), QT_TRANSLATE_NOOP( "@default", "Account manager window"             ), "AccountManagerWindow"       , "functionAccountManagerWindow"                 );
	new ConfHotKey( this, QT_TRANSLATE_NOOP( "@default", "Windows shortcuts" ), QT_TRANSLATE_NOOP( "@default", "Plugins window"                     ), "PluginsWindow"              , "functionPluginsWindow"                        );
	// create config manager for BuddiesShotrcuts and BuddiesMenus
	new ConfManager( this );
	// create default configuration for BuddiesMenus
	if( config_file.readEntry( "GlobalHotkeys", "BuddiesMenus", QString::null ).isNull() )
	{
		ConfBuddiesMenu *confbuddiesmenu;
		confbuddiesmenu = new ConfBuddiesMenu( this, QT_TRANSLATE_NOOP( "@default", "Buddies menus" ), false );
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
		confbuddiesmenu = new ConfBuddiesMenu( this, QT_TRANSLATE_NOOP( "@default", "Buddies menus" ), false );
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
		ConfigGroupBox *groupBuddiesShortcuts = mainConfigurationWindow->widget()->configGroupBox(
			QT_TRANSLATE_NOOP( "@default", "Shortcuts" ),
			QT_TRANSLATE_NOOP( "@default", "Global hotkeys" ),
			QT_TRANSLATE_NOOP( "@default", "Buddies shortcuts" )
		);
		if( groupBuddiesShortcuts )
		{
			BUDDIESSHORTCUTSADDNEWBUTTON = new ConfigActionButton( QT_TRANSLATE_NOOP( "@default", "Add new shortcut ..." ), "", groupBuddiesShortcuts, NULL );
			groupBuddiesShortcuts->widget()->layout()->removeWidget( BUDDIESSHORTCUTSADDNEWBUTTON );
			groupBuddiesShortcuts->addWidget( BUDDIESSHORTCUTSADDNEWBUTTON, true );  // re-insert the button so that it takes full available horizontal space
			connect( BUDDIESSHORTCUTSADDNEWBUTTON, SIGNAL(clicked()), this, SLOT(buddiesShortcutsAddNewButtonPressed()));
		}
	}
	// add BUDDIESMENUSADDNEWBUTTON button
	if( BUDDIESMENUSADDNEWBUTTON == NULL )
	{
		ConfigGroupBox *groupBuddiesMenus = mainConfigurationWindow->widget()->configGroupBox(
			QT_TRANSLATE_NOOP( "@default", "Shortcuts" ),
			QT_TRANSLATE_NOOP( "@default", "Global hotkeys" ),
			QT_TRANSLATE_NOOP( "@default", "Buddies menus" )
		);
		if( groupBuddiesMenus )
		{
			BUDDIESMENUSADDNEWBUTTON = new ConfigActionButton( QT_TRANSLATE_NOOP( "@default", "Add new menu ..." ), "", groupBuddiesMenus, NULL );
			groupBuddiesMenus->widget()->layout()->removeWidget( BUDDIESMENUSADDNEWBUTTON );
			groupBuddiesMenus->addWidget( BUDDIESMENUSADDNEWBUTTON, true );  // re-insert the button so that it takes full available horizontal space
			connect( BUDDIESMENUSADDNEWBUTTON, SIGNAL(clicked()), this, SLOT(buddiesMenusAddNewButtonPressed()) );
		}
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
	QStringList notgrabbedhotkeys;
	foreach( ConfHotKey* confhotkey, ConfHotKey::instances() )
	{
		if( grabHotKey( confhotkey->hotKey() ) == 1 )
			notgrabbedhotkeys.append( confhotkey->hotKey().string() );
	}
	foreach( ConfBuddiesShortcut* confbuddiesshortcut, ConfBuddiesShortcut::instances() )
	{
		if( grabHotKey( confbuddiesshortcut->hotKey() ) == 1 )
			notgrabbedhotkeys.append( confbuddiesshortcut->hotKey().string() );
	}
	foreach( ConfBuddiesMenu* confbuddiesmenu, ConfBuddiesMenu::instances() )
	{
		if( grabHotKey( confbuddiesmenu->hotKey() ) == 1 )
			notgrabbedhotkeys.append( confbuddiesmenu->hotKey().string() );
	}
	if( notgrabbedhotkeys.count() > 0 )
	{
		QMessageBox *messagebox = new QMessageBox(
			qApp->translate( "@default", "Kadu" ) + " - " + qApp->translate( "@default", "Global hotkeys" ),
			qApp->translate( "@default", "%n hotkey(s):\n- %1\nis/are in use by another application.", 0, QCoreApplication::CodecForTr, notgrabbedhotkeys.count() ).arg( notgrabbedhotkeys.join( "\n- " ) ),
			QMessageBox::Warning,
			QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton,
			qApp->activeWindow()
		);
		messagebox->show();
	}
	// single shot the hotkeys timer
	HOTKEYSTIMER->start( GLOBALHOTKEYS_HOTKEYSTIMERINTERVAL );
}


unsigned char xerrorcode;


int (*oldXErrorHandler)(Display*, XErrorEvent*);


int xErrorHandler( Display *display, XErrorEvent *ee ) 
{
	Q_UNUSED(display);
	xerrorcode = ee->error_code;
	return 0;
}


int GlobalHotkeys::grabHotKey( HotKey hotkey )
{
	uint modifiers;
	int result = -1;
	if( ! hotkey.isNull() )
	{
		result = 0;
		// modifiers
		modifiers = 0;
		modifiers |= ( hotkey.shift()   ? GLOBALHOTKEYS_X11SHIFTMASK   : 0 );
		modifiers |= ( hotkey.control() ? GLOBALHOTKEYS_X11CONTROLMASK : 0 );
		modifiers |= ( hotkey.alt()     ? GLOBALHOTKEYS_X11ALTMASK     : 0 );
		modifiers |= ( hotkey.altGr()   ? GLOBALHOTKEYS_X11ALTGRMASK   : 0 );
		modifiers |= ( hotkey.super()   ? GLOBALHOTKEYS_X11SUPERMASK   : 0 );
		// install own X11 errors handler
		oldXErrorHandler = XSetErrorHandler( xErrorHandler );
		// pure hotkey
		xerrorcode = Success;
		XGrabKey(
			DISPLAY, hotkey.keyCode(), modifiers,
			DefaultRootWindow( DISPLAY ), False, GrabModeAsync, GrabModeAsync
			);
		XSync( DISPLAY, False );
		if( xerrorcode == Success )
		{
			// hotkey with NumLock
			XGrabKey(
				DISPLAY, hotkey.keyCode(), modifiers | GLOBALHOTKEYS_X11NUMLOCKMASK,
				DefaultRootWindow( DISPLAY ), True, GrabModeAsync, GrabModeAsync
				);
			// hotkey with CapsLock
			XGrabKey(
				DISPLAY, hotkey.keyCode(), modifiers | GLOBALHOTKEYS_X11CAPSLOCKMASK,
				DefaultRootWindow( DISPLAY ), True, GrabModeAsync, GrabModeAsync
				);
			// hotkey with CapsLock and NumLock
			XGrabKey(
				DISPLAY, hotkey.keyCode(), modifiers | GLOBALHOTKEYS_X11CAPSLOCKMASK | GLOBALHOTKEYS_X11NUMLOCKMASK,
				DefaultRootWindow( DISPLAY ), True, GrabModeAsync, GrabModeAsync
				);
			// catch errors
			XSync( DISPLAY, False );
		}
		else if( xerrorcode == BadAccess )
		{
			result = 1;
		}
		// install previous X11 errors handler
		XSetErrorHandler( oldXErrorHandler );
	}
	return result;
}


void GlobalHotkeys::buddiesShortcutsAddNewButtonPressed()
{
	ConfBuddiesShortcut *confbuddiesshortcut = new ConfBuddiesShortcut( this, QT_TRANSLATE_NOOP( "@default", "Buddies shortcuts" ), true );
	confbuddiesshortcut->focus();
}


void GlobalHotkeys::buddiesMenusAddNewButtonPressed()
{
	ConfBuddiesMenu *confbuddiesmenu = new ConfBuddiesMenu( this, QT_TRANSLATE_NOOP( "@default", "Buddies menus" ), true );
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
		ChatWidgetManager::instance()->openPendingMessages( chat, true );
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
	menu->setContactsSubmenu( true );
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
	// remove given buddies from the menu
	foreach( QString buddydisplay, confbuddiesmenu->excludeBuddies() )
		menu->remove( buddydisplay );
	// set one item per buddy
	menu->setOneItemPerBuddy( confbuddiesmenu->oneItemPerBuddy() );
	// set stateless sorting
	menu->setSortStatelessBuddies(         confbuddiesmenu->sortStatelessBuddies()         );
	menu->setSortStatelessBuddiesByStatus( confbuddiesmenu->sortStatelessBuddiesByStatus() );
	// popup in the center of the screen
	menu->popup();
	// global data
	SHOWNGLOBALWIDGET = menu;
	SHOWNGLOBALWIDGETHOTKEY = confbuddiesmenu->hotKey();
}




Q_EXPORT_PLUGIN2( globalhotkeys, GlobalHotkeys )
