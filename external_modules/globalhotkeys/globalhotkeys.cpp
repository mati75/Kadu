/***************************************************************************
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 3 of the License, or      *
*   (at your option) any later version.                                    *
*                                                                          *
***************************************************************************/




#include <QApplication>
#include <QDesktopWidget>
#include <QMap>
#include <QMessageBox>
#include <QMenu>
#include <QLayout>
#include <QList>
#include <QPainter>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QX11Info>

#include "config_file.h"
#include "debug.h"
#include "gadu.h"
#define private public
#include "kadu.h"
#undef private

#include "activate.h"
#include "chat_manager.h"
#include "groups_manager.h"
#include "icons_manager.h"
#include "ignore.h"
#include "misc.h"
#include "pending_msgs.h"
#include "status_changer.h"

#include "globalhotkeys.h"

#include "wideiconsmenu.h"

#include <X11/Xlib.h>
#include <X11/Xatom.h>




GlobalHotkeys *globalHotkeys;




extern "C" int globalhotkeys_init()
{
	kdebugf();
	// create new globalHotkeys object
	globalHotkeys = new GlobalHotkeys();
	// unregister UI file
	MainConfigurationWindow::registerUiFile( dataPath("kadu/modules/configuration/globalhotkeys.ui"), globalHotkeys );
	kdebugf2();
	return 0;
}


extern "C" void globalhotkeys_close()
{
	kdebugf();
	// unregister UI file
	MainConfigurationWindow::unregisterUiFile( dataPath("kadu/modules/configuration/globalhotkeys.ui"), globalHotkeys );
	// delete globalHotkeys object
	delete globalHotkeys;
	globalHotkeys = NULL;
	kdebugf2();
}




GlobalHotkeys::GlobalHotkeys() : QObject( NULL, "globalhotkeys" )
{
	// create default configuration if needed
	createDefaultConfiguration();
	// set display to NULL
	display = NULL;
	// statusmenu popup menu
	statusmenu = kadu->statusMenu;
	// create the contactsmenu popup menu and reparent it bypass the WM
	contactsmenu = new QMenu();
	int wideiconwidth =
		GLOBALHOTKEYS_CONTACTSMENUICONMARGINLEFT +
		GLOBALHOTKEYS_CONTACTSMENUSMALLICONSIZE +
		GLOBALHOTKEYS_CONTACTSMENUICONSPACING +
		GLOBALHOTKEYS_CONTACTSMENUSMALLICONSIZE +
		GLOBALHOTKEYS_CONTACTSMENUICONMARGINRIGHT;
	contactsmenu->setStyle( new WideIconsMenu( wideiconwidth, contactsmenu->style()->objectName() ) );
	contactsmenu->reparent( (QWidget*)0, Qt::WType_TopLevel | Qt::WX11BypassWM, QPoint( 0, 0 ), false );
	// create and connect() statusmenu popup menu inactivity timer
	statusmenuinactivitytimer = new QTimer( statusmenu );
	connect( statusmenuinactivitytimer, SIGNAL(timeout()), this, SLOT(statusmenuinactivitytimerTimeout()) );
	// statusmenu not shown yet
	statusmenushown = false;
	// create and connect() contactsmenu popup menu inactivity timer
	contactsmenuinactivitytimer = new QTimer( contactsmenu );
	connect( contactsmenuinactivitytimer, SIGNAL(timeout()), this, SLOT(contactsmenuinactivitytimerTimeout()) );
	// contactsmenu not shown yet
	contactsmenushown = false;
	lastcontactsmenushortcut = "";
	// create and connect() the hotkeys timer
	hotkeysTimer = new QTimer( this );
	connect( hotkeysTimer, SIGNAL(timeout()), this, SLOT(checkPendingHotkeys()) );
	// read the configuration and force its usage
	configurationUpdated();
}


GlobalHotkeys::~GlobalHotkeys()
{
	// stop the hotkeys timer
	if( hotkeysTimer->isActive() )
	{
		hotkeysTimer->stop();
	}
	// close X11 display
	if( display != NULL )
	{
		XCloseDisplay( display );
	}
	// clear hotkeys list
	for( QMap<QString,Hotkey*>::Iterator I = hotkeys.begin(); I != hotkeys.end(); I++ )
	{
		delete (*I);
	}
	hotkeys.clear();
	// remove configuration widgets if needed
	if( ! HEshowKadusMainWindow.isNull() )
	{
		if( ! HEshowKadusMainWindow.isNull()         ) delete (HotkeyEdit*) HEshowKadusMainWindow;
		if( ! HEhideKadusMainWindow.isNull()         ) delete (HotkeyEdit*) HEhideKadusMainWindow;
		if( ! HEshowHideKadusMainWindow.isNull()     ) delete (HotkeyEdit*) HEshowHideKadusMainWindow;
		if( ! HEchangeStatus.isNull()                ) delete (HotkeyEdit*) HEchangeStatus;
		if( ! HEchangeDescription.isNull()           ) delete (HotkeyEdit*) HEchangeDescription;
		if( ! HEturnSilentModeOn.isNull()            ) delete (HotkeyEdit*) HEturnSilentModeOn;
		if( ! HEturnSilentModeOff.isNull()           ) delete (HotkeyEdit*) HEturnSilentModeOff;
		if( ! HEtoggleSilentMode.isNull()            ) delete (HotkeyEdit*) HEtoggleSilentMode;
		if( ! HEopenIncomingChatWindow.isNull()      ) delete (HotkeyEdit*) HEopenIncomingChatWindow;
		if( ! HEopenAllIncomingChatWindows.isNull()  ) delete (HotkeyEdit*) HEopenAllIncomingChatWindows;
		if( ! HEminimizeOpenedChatWindows.isNull()   ) delete (HotkeyEdit*) HEminimizeOpenedChatWindows;
		if( ! HErestoreMinimizedChatWindows.isNull() ) delete (HotkeyEdit*) HErestoreMinimizedChatWindows;
		if( ! HEminimizeRestoreChatWindows.isNull()  ) delete (HotkeyEdit*) HEminimizeRestoreChatWindows;
		if( ! HEcloseAllChatWindows.isNull()         ) delete (HotkeyEdit*) HEcloseAllChatWindows;
		if( ! HEopenChatWith.isNull()                ) delete (HotkeyEdit*) HEopenChatWith;
		// <---
		// remove configuration widgets for contacts shortcuts if needed
		if( ! contactsAddNewButton.isNull() ) delete (ConfigActionButton*) contactsAddNewButton;
		for( QList< QPointer<ConfigLineEdit> >::Iterator I = contactsNamesEditList.begin(); I != contactsNamesEditList.end(); I++ )
		{
			if( ! (*I).isNull() ) delete (ConfigLineEdit*) (*I);
		}
		for( QList< QPointer<HotkeyEdit> >::Iterator I = contactsHotkeyEditList.begin(); I != contactsHotkeyEditList.end(); I++ )
		{
			if( ! (*I).isNull() ) delete (HotkeyEdit*) (*I);
		}
		// remove configuration widgets for contacts menus shortcuts if needed
		if( ! contactsmenusAddNewButton.isNull() ) delete (ConfigActionButton*) contactsmenusAddNewButton;
		for( QList< QPointer<ConfigCheckBox> >::Iterator I = contactsmenusCurrentChatsCheckboxList.begin(); I != contactsmenusCurrentChatsCheckboxList.end(); I++ )
		{
			if( ! (*I).isNull() ) delete (ConfigCheckBox*) (*I);
		}
		for( QList< QPointer<ConfigCheckBox> >::Iterator I = contactsmenusRecentChatsCheckboxList.begin(); I != contactsmenusRecentChatsCheckboxList.end(); I++ )
		{
			if( ! (*I).isNull() ) delete (ConfigCheckBox*) (*I);
		}
		for( QList< QPointer<ConfigCheckBox> >::Iterator I = contactsmenusOnlineContactsCheckboxList.begin(); I != contactsmenusOnlineContactsCheckboxList.end(); I++ )
		{
			if( ! (*I).isNull() ) delete (ConfigCheckBox*) (*I);
		}
		for( QList< QPointer<ConfigLineEdit> >::Iterator I = contactsmenusOnlineContactsGroupsEditList.begin(); I != contactsmenusOnlineContactsGroupsEditList.end(); I++ )
		{
			if( ! (*I).isNull() ) delete (ConfigLineEdit*) (*I);
		}
		for( QList< QPointer<ConfigCheckBox> >::Iterator I = contactsmenusOnlineContactsIncludeBlockingCheckboxList.begin(); I != contactsmenusOnlineContactsIncludeBlockingCheckboxList.end(); I++ )
		{
			if( ! (*I).isNull() ) delete (ConfigCheckBox*) (*I);
		}
		for( QList< QPointer<ConfigLineEdit> >::Iterator I = contactsmenusContactsEditList.begin(); I != contactsmenusContactsEditList.end(); I++ )
		{
			if( ! (*I).isNull() ) delete (ConfigLineEdit*) (*I);
		}
		for( QList< QPointer<ConfigLineEdit> >::Iterator I = contactsmenusGroupsEditList.begin(); I != contactsmenusGroupsEditList.end(); I++ )
		{
			if( ! (*I).isNull() ) delete (ConfigLineEdit*) (*I);
		}
		for( QList< QPointer<ConfigLineEdit> >::Iterator I = contactsmenusExcludeContactsEditList.begin(); I != contactsmenusExcludeContactsEditList.end(); I++ )
		{
			if( ! (*I).isNull() ) delete (ConfigLineEdit*) (*I);
		}
		for( QList< QPointer<HotkeyEdit> >::Iterator I = contactsmenusHotkeyEditList.begin(); I != contactsmenusHotkeyEditList.end(); I++ )
		{
			if( ! (*I).isNull() ) delete (HotkeyEdit*) (*I);
		}
		// remove configuration groups widgets
		if( groupKadu          ) delete groupKadu;
		if( groupChats         ) delete groupChats;
		if( groupContacts      ) delete groupContacts;
		if( groupContactsMenus ) delete groupContactsMenus;
		// <---
	}
}


QIcon GlobalHotkeys::contactsMenuIcon( QIcon chaticon, QIcon statusicon )
{
	int wideiconwidth =
		GLOBALHOTKEYS_CONTACTSMENUICONMARGINLEFT +
		GLOBALHOTKEYS_CONTACTSMENUSMALLICONSIZE +
		GLOBALHOTKEYS_CONTACTSMENUICONSPACING +
		GLOBALHOTKEYS_CONTACTSMENUSMALLICONSIZE +
		GLOBALHOTKEYS_CONTACTSMENUICONMARGINRIGHT;
	int smalliconsize = GLOBALHOTKEYS_CONTACTSMENUSMALLICONSIZE;
	int left          = GLOBALHOTKEYS_CONTACTSMENUICONMARGINLEFT;
	int spacing       = GLOBALHOTKEYS_CONTACTSMENUICONSPACING;
	QPixmap pixmap( wideiconwidth, smalliconsize );
	pixmap.fill( Qt::transparent );
	QPainter painter( &pixmap );
	painter.drawPixmap(
			QRect( left, 0, smalliconsize, smalliconsize ),
			chaticon.pixmap( smalliconsize, smalliconsize ),
			QRect( 0, 0, smalliconsize, smalliconsize )
		);
	painter.drawPixmap(
			QRect( left+smalliconsize+spacing, 0, smalliconsize, smalliconsize ),
			statusicon.pixmap( smalliconsize, smalliconsize ),
			QRect( 0, 0, smalliconsize, smalliconsize )
		);
	return QIcon( pixmap );
}


void GlobalHotkeys::createContactsMenuIcons()
{
	// source icons
	QPixmap emptypixmap( GLOBALHOTKEYS_CONTACTSMENUSMALLICONSIZE, GLOBALHOTKEYS_CONTACTSMENUSMALLICONSIZE );
	emptypixmap.fill( Qt::transparent );
	QIcon icon_empty           = QIcon( emptypixmap );
	QIcon icon_chatcurrent     = icons_manager->loadIcon( "OpenChat" );
	QIcon icon_chatpending     = icons_manager->loadIcon( "Message"  );
	QIcon icon_chatrecent      = icons_manager->loadIcon( "History"  );
	QIcon icon_chatnone        = icon_empty;
	QIcon icon_statusonline    = icons_manager->loadIcon( "Online"    );
	QIcon icon_statusbusy      = icons_manager->loadIcon( "Busy"      );
	QIcon icon_statusinvisible = icons_manager->loadIcon( "Invisible" );
	QIcon icon_statusoffline   = icons_manager->loadIcon( "Offline"   );
	QIcon icon_statusblocking  = icons_manager->loadIcon( "Blocking"  );
	QIcon icon_conference      = icons_manager->loadIcon( "OpenChat"  );
	// contactsmenuicons
	contactsmenuicons[ "empty"             ] = contactsMenuIcon( icon_empty      , icon_empty           );
	contactsmenuicons[ "CurrentOnline"     ] = contactsMenuIcon( icon_chatcurrent, icon_statusonline    );
	contactsmenuicons[ "CurrentBusy"       ] = contactsMenuIcon( icon_chatcurrent, icon_statusbusy      );
	contactsmenuicons[ "CurrentInvisible"  ] = contactsMenuIcon( icon_chatcurrent, icon_statusinvisible );
	contactsmenuicons[ "CurrentOffline"    ] = contactsMenuIcon( icon_chatcurrent, icon_statusoffline   );
	contactsmenuicons[ "CurrentBlocking"   ] = contactsMenuIcon( icon_chatcurrent, icon_statusblocking  );
	contactsmenuicons[ "CurrentConference" ] = contactsMenuIcon( icon_chatcurrent, icon_conference      );
	contactsmenuicons[ "PendingOnline"     ] = contactsMenuIcon( icon_chatpending, icon_statusonline    );
	contactsmenuicons[ "PendingBusy"       ] = contactsMenuIcon( icon_chatpending, icon_statusbusy      );
	contactsmenuicons[ "PendingInvisible"  ] = contactsMenuIcon( icon_chatpending, icon_statusinvisible );
	contactsmenuicons[ "PendingOffline"    ] = contactsMenuIcon( icon_chatpending, icon_statusoffline   );
	contactsmenuicons[ "PendingBlocking"   ] = contactsMenuIcon( icon_chatpending, icon_statusblocking  );
	contactsmenuicons[ "PendingConference" ] = contactsMenuIcon( icon_chatpending, icon_conference      );
	contactsmenuicons[ "RecentOnline"      ] = contactsMenuIcon( icon_chatrecent , icon_statusonline    );
	contactsmenuicons[ "RecentBusy"        ] = contactsMenuIcon( icon_chatrecent , icon_statusbusy      );
	contactsmenuicons[ "RecentInvisible"   ] = contactsMenuIcon( icon_chatrecent , icon_statusinvisible );
	contactsmenuicons[ "RecentOffline"     ] = contactsMenuIcon( icon_chatrecent , icon_statusoffline   );
	contactsmenuicons[ "RecentBlocking"    ] = contactsMenuIcon( icon_chatrecent , icon_statusblocking  );
	contactsmenuicons[ "RecentConference"  ] = contactsMenuIcon( icon_chatrecent , icon_conference      );
	contactsmenuicons[ "NoneOnline"        ] = contactsMenuIcon( icon_chatnone   , icon_statusonline    );
	contactsmenuicons[ "NoneBusy"          ] = contactsMenuIcon( icon_chatnone   , icon_statusbusy      );
	contactsmenuicons[ "NoneInvisible"     ] = contactsMenuIcon( icon_chatnone   , icon_statusinvisible );
	contactsmenuicons[ "NoneOffline"       ] = contactsMenuIcon( icon_chatnone   , icon_statusoffline   );
	contactsmenuicons[ "NoneBlocking"      ] = contactsMenuIcon( icon_chatnone   , icon_statusblocking  );
	contactsmenuicons[ "NoneConference"    ] = contactsMenuIcon( icon_chatnone   , icon_conference      );
}


void GlobalHotkeys::mainConfigurationWindowCreated( MainConfigurationWindow *mainConfigurationWindow )
{
	// create groups boxes
	groupKadu          = mainConfigurationWindow->configGroupBox( "Shortcuts", "Global hotkeys", "Kadu"           );
	groupChats         = mainConfigurationWindow->configGroupBox( "Shortcuts", "Global hotkeys", "Chats"          );
	groupContacts      = mainConfigurationWindow->configGroupBox( "Shortcuts", "Global hotkeys", "Contacts"       );
	groupContactsMenus = mainConfigurationWindow->configGroupBox( "Shortcuts", "Global hotkeys", "Contacts menus" );
	// create hotkey edit widgets
	HEshowKadusMainWindow         = new HotkeyEdit( "GlobalHotkeys", "ShowKadusMainWindow"        , "Show Kadu's main window"           , "", groupKadu  );
	HEhideKadusMainWindow         = new HotkeyEdit( "GlobalHotkeys", "HideKadusMainWindow"        , "Hide Kadu's main window"           , "", groupKadu  );
	HEshowHideKadusMainWindow     = new HotkeyEdit( "GlobalHotkeys", "ShowHideKadusMainWindow"    , "Show/hide Kadu's main window"      , "", groupKadu  );
	HEchangeStatus                = new HotkeyEdit( "GlobalHotkeys", "ChangeStatus"               , "Change status"                     , "", groupKadu  );
	HEchangeDescription           = new HotkeyEdit( "GlobalHotkeys", "ChangeDescription"          , "Change description"                , "", groupKadu  );
	HEturnSilentModeOn            = new HotkeyEdit( "GlobalHotkeys", "TurnSilentModeOn"           , "Turn silent mode on"               , "", groupKadu  );
	HEturnSilentModeOff           = new HotkeyEdit( "GlobalHotkeys", "TurnSilentModeOff"          , "Turn silent mode off"              , "", groupKadu  );
	HEtoggleSilentMode            = new HotkeyEdit( "GlobalHotkeys", "ToggleSilentMode"           , "Toggle silent mode"                , "", groupKadu  );
	HEopenIncomingChatWindow      = new HotkeyEdit( "GlobalHotkeys", "OpenIncomingChatWindow"     , "Open incoming chat window"         , "", groupChats );
	HEopenAllIncomingChatWindows  = new HotkeyEdit( "GlobalHotkeys", "OpenAllIncomingChatWindows" , "Open all incoming chat windows"    , "", groupChats );
	HEminimizeOpenedChatWindows   = new HotkeyEdit( "GlobalHotkeys", "MinimizeOpenedChatWindows"  , "Minimize all opened chat windows"  , "", groupChats );
	HErestoreMinimizedChatWindows = new HotkeyEdit( "GlobalHotkeys", "RestoreMinimizedChatWindows", "Restore all minimized chat windows", "", groupChats );
	HEminimizeRestoreChatWindows  = new HotkeyEdit( "GlobalHotkeys", "MinimizeRestoreChatWindows" , "Minimize/restore all chat windows" , "", groupChats );
	HEcloseAllChatWindows         = new HotkeyEdit( "GlobalHotkeys", "CloseAllChatWindows"        , "Close all chat windows"            , "", groupChats );
	HEopenChatWith                = new HotkeyEdit( "GlobalHotkeys", "OpenChatWith"               , "Open chat with ..."                , "", groupChats );
	// <---
	// set hotkey edit widgets' values
	HEshowKadusMainWindow->setText(         config_file.readEntry( "GlobalHotkeys", "ShowKadusMainWindow"         ).stripWhiteSpace() );
	HEhideKadusMainWindow->setText(         config_file.readEntry( "GlobalHotkeys", "HideKadusMainWindow"         ).stripWhiteSpace() );
	HEshowHideKadusMainWindow->setText(     config_file.readEntry( "GlobalHotkeys", "ShowHideKadusMainWindow"     ).stripWhiteSpace() );
	HEchangeStatus->setText(                config_file.readEntry( "GlobalHotkeys", "ChangeStatus"                ).stripWhiteSpace() );
	HEchangeDescription->setText(           config_file.readEntry( "GlobalHotkeys", "ChangeDescription"           ).stripWhiteSpace() );
	HEturnSilentModeOn->setText(            config_file.readEntry( "GlobalHotkeys", "TurnSilentModeOn"            ).stripWhiteSpace() );
	HEturnSilentModeOff->setText(           config_file.readEntry( "GlobalHotkeys", "TurnSilentModeOff"           ).stripWhiteSpace() );
	HEtoggleSilentMode->setText(            config_file.readEntry( "GlobalHotkeys", "ToggleSilentMode"            ).stripWhiteSpace() );
	HEopenIncomingChatWindow->setText(      config_file.readEntry( "GlobalHotkeys", "OpenIncomingChatWindow"      ).stripWhiteSpace() );
	HEopenAllIncomingChatWindows->setText(  config_file.readEntry( "GlobalHotkeys", "OpenAllIncomingChatWindows"  ).stripWhiteSpace() );
	HEminimizeOpenedChatWindows->setText(   config_file.readEntry( "GlobalHotkeys", "MinimizeOpenedChatWindows"   ).stripWhiteSpace() );
	HErestoreMinimizedChatWindows->setText( config_file.readEntry( "GlobalHotkeys", "RestoreMinimizedChatWindows" ).stripWhiteSpace() );
	HEminimizeRestoreChatWindows->setText(  config_file.readEntry( "GlobalHotkeys", "MinimizeRestoreChatWindows"  ).stripWhiteSpace() );
	HEcloseAllChatWindows->setText(         config_file.readEntry( "GlobalHotkeys", "CloseAllChatWindows"         ).stripWhiteSpace() );
	HEopenChatWith->setText(                config_file.readEntry( "GlobalHotkeys", "OpenChatWith"                ).stripWhiteSpace() );
	// <---
	int c;
	// add contacts group fields
	contactsAddNewButton = new ConfigActionButton( "Add new shortcut ...", "", groupContacts );
	groupContacts->widget()->layout()->removeWidget( contactsAddNewButton );
	groupContacts->addWidget( contactsAddNewButton, true );  // re-insert the button so that it takes full available horizontal space
	connect( contactsAddNewButton, SIGNAL( clicked() ), this, SLOT( contactsAddNewButtonPressed() ) );
	contactsshortcuts.clear();
	c = 0;
	while( true )
	{
		QString _contacts = config_file.readEntry( "GlobalHotkeys", QString( "ContactsShortcuts_Contacts_%" ).replace( "%", QString::number( c ) ) ).stripWhiteSpace();
		QString _shortcut = config_file.readEntry( "GlobalHotkeys", QString( "ContactsShortcuts_Shortcut_%" ).replace( "%", QString::number( c ) ) ).stripWhiteSpace();
		if( _contacts.isEmpty() && _shortcut.isEmpty() )
		{
			break;
		}
		contactsshortcuts.push_back( QPair<QStringList,QString>( QStringList::split( QRegExp(" *, *"), _contacts.trimmed(), false ), _shortcut ) );
		c++;
	}
	c = 0;
	for( QList< QPair<QStringList,QString> >::Iterator I = contactsshortcuts.begin(); I != contactsshortcuts.end(); I++ )
	{
		ConfigLineEdit *contactnameedit   = new ConfigLineEdit( "GlobalHotkeys", QString( "ContactsShortcuts_Contacts_%" ).replace( "%", QString::number( c ) ), "Contacts (comma separated)", "", groupContacts );
		HotkeyEdit     *contacthotkeyedit = new HotkeyEdit(     "GlobalHotkeys", QString( "ContactsShortcuts_Shortcut_%" ).replace( "%", QString::number( c ) ), "Shortcut"                  , "", groupContacts );
		contactsNamesEditList.push_back(  contactnameedit   );
		contactsHotkeyEditList.push_back( contacthotkeyedit );
		contactnameedit->setText(   (*I).first.join( ", " ) );
		contacthotkeyedit->setText( (*I).second             );
		contactnameedit->show();
		contacthotkeyedit->show();
		c++;
	}
	// add contactsmenus group fields
	contactsmenusAddNewButton = new ConfigActionButton( "Add new contacts menu ...", "", groupContactsMenus );
	groupContactsMenus->widget()->layout()->removeWidget( contactsmenusAddNewButton );
	groupContactsMenus->addWidget( contactsmenusAddNewButton, true );  // re-insert the button so that it takes full available horizontal space
	connect( contactsmenusAddNewButton, SIGNAL( clicked() ), this, SLOT( contactsmenusAddNewButtonPressed() ) );
	contactsmenusshortcuts.clear();
	c = 0;
	while( true )
	{
		bool    _currentchats                  = config_file.readBoolEntry( "GlobalHotkeys", QString( "ContactsMenuShortcuts_CurrentChats_%"                  ).replace( "%", QString::number( c ) ) );
		bool    _pendingchats                  = config_file.readBoolEntry( "GlobalHotkeys", QString( "ContactsMenuShortcuts_PendingChats_%"                  ).replace( "%", QString::number( c ) ) );
		bool    _recentchats                   = config_file.readBoolEntry( "GlobalHotkeys", QString( "ContactsMenuShortcuts_RecentChats_%"                   ).replace( "%", QString::number( c ) ) );
		bool    _onlinecontacts                = config_file.readBoolEntry( "GlobalHotkeys", QString( "ContactsMenuShortcuts_OnlineContacts_%"                ).replace( "%", QString::number( c ) ) );
		QString _onlinecontactsgroups          = config_file.readEntry(     "GlobalHotkeys", QString( "ContactsMenuShortcuts_OnlineContactsGroups_%"          ).replace( "%", QString::number( c ) ) ).stripWhiteSpace();
		bool    _onlinecontactsincludeblocking = config_file.readBoolEntry( "GlobalHotkeys", QString( "ContactsMenuShortcuts_OnlineContactsIncludeBlocking_%" ).replace( "%", QString::number( c ) ) );
		QString _contacts                      = config_file.readEntry(     "GlobalHotkeys", QString( "ContactsMenuShortcuts_Contacts_%"                      ).replace( "%", QString::number( c ) ) ).stripWhiteSpace();
		QString _groups                        = config_file.readEntry(     "GlobalHotkeys", QString( "ContactsMenuShortcuts_Groups_%"                        ).replace( "%", QString::number( c ) ) ).stripWhiteSpace();
		QString _excludecontacts               = config_file.readEntry(     "GlobalHotkeys", QString( "ContactsMenuShortcuts_ExcludeContacts_%"               ).replace( "%", QString::number( c ) ) ).stripWhiteSpace();
		QString _shortcut                      = config_file.readEntry(     "GlobalHotkeys", QString( "ContactsMenuShortcuts_Shortcut_%"                      ).replace( "%", QString::number( c ) ) ).stripWhiteSpace();
		if( ( ! _currentchats ) && ( ! _pendingchats ) && ( ! _recentchats ) && ( ! _onlinecontacts ) && ( _contacts.isEmpty() ) && ( _groups.isEmpty() ) && ( _excludecontacts.isEmpty() ) )
		{
			break;
		}
		ContactsMenuItemData _contactsmenuitemdata;
		_contactsmenuitemdata.currentchats                  = _currentchats;
		_contactsmenuitemdata.pendingchats                  = _pendingchats;
		_contactsmenuitemdata.recentchats                   = _recentchats;
		_contactsmenuitemdata.onlinecontacts                = _onlinecontacts;
		_contactsmenuitemdata.onlinecontactsgroups          = QStringList::split( QRegExp(" *, *"), _onlinecontactsgroups.trimmed(), false );
		_contactsmenuitemdata.onlinecontactsincludeblocking = _onlinecontactsincludeblocking;
		_contactsmenuitemdata.contacts                      = QStringList::split( QRegExp(" *, *"), _contacts.trimmed()            , false );
		_contactsmenuitemdata.groups                        = QStringList::split( QRegExp(" *, *"), _groups.trimmed()              , false );
		_contactsmenuitemdata.excludecontacts               = QStringList::split( QRegExp(" *, *"), _excludecontacts.trimmed()     , false );
		contactsmenusshortcuts.push_back( QPair<ContactsMenuItemData,QString>( _contactsmenuitemdata, _shortcut ) );
		c++;
	}
	c = 0;
	for( QList< QPair<ContactsMenuItemData,QString> >::Iterator I = contactsmenusshortcuts.begin(); I != contactsmenusshortcuts.end(); I++ )
	{
		ConfigCheckBox *contactsmenucurrentchatsbox                  = new ConfigCheckBox( "GlobalHotkeys", QString( "ContactsMenuShortcuts_CurrentChats_%"                  ).replace( "%", QString::number( c ) ), "Include current chats"                               , ""                                  , groupContactsMenus );
		ConfigCheckBox *contactsmenupendingchatsbox                  = new ConfigCheckBox( "GlobalHotkeys", QString( "ContactsMenuShortcuts_PendingChats_%"                  ).replace( "%", QString::number( c ) ), "Include chats with pending messages"                 , ""                                  , groupContactsMenus );
		ConfigCheckBox *contactsmenurecentchatsbox                   = new ConfigCheckBox( "GlobalHotkeys", QString( "ContactsMenuShortcuts_RecentChats_%"                   ).replace( "%", QString::number( c ) ), "Include recent chats"                                , ""                                  , groupContactsMenus );
		ConfigCheckBox *contactsmenuonlinecontactsbox                = new ConfigCheckBox( "GlobalHotkeys", QString( "ContactsMenuShortcuts_OnlineContacts_%"                ).replace( "%", QString::number( c ) ), "Include online contacts"                             , ""                                  , groupContactsMenus );
		ConfigLineEdit *contactsmenuonlinecontactsgroupsedit         = new ConfigLineEdit( "GlobalHotkeys", QString( "ContactsMenuShortcuts_OnlineContactsGroups_%"          ).replace( "%", QString::number( c ) ), "only from these groups (comma separated)"            , "leave empty to disable this filter", groupContactsMenus );
		ConfigCheckBox *contactsmenuonlinecontactsincludeblockingbox = new ConfigCheckBox( "GlobalHotkeys", QString( "ContactsMenuShortcuts_OnlineContactsIncludeBlocking_%" ).replace( "%", QString::number( c ) ), "Treat contacts blocking me as online"                , ""                                  , groupContactsMenus );
		ConfigLineEdit *contactsmenucontactsedit                     = new ConfigLineEdit( "GlobalHotkeys", QString( "ContactsMenuShortcuts_Contacts_%"                      ).replace( "%", QString::number( c ) ), "Include these contacts (comma separated)"            , ""                                  , groupContactsMenus );
		ConfigLineEdit *contactsmenugroupsedit                       = new ConfigLineEdit( "GlobalHotkeys", QString( "ContactsMenuShortcuts_Groups_%"                        ).replace( "%", QString::number( c ) ), "Include contacts from these gropus (comma separated)", ""                                  , groupContactsMenus );
		ConfigLineEdit *contactsmenuexcludecontactsedit              = new ConfigLineEdit( "GlobalHotkeys", QString( "ContactsMenuShortcuts_ExcludeContacts_%"               ).replace( "%", QString::number( c ) ), "Exclude these contacts (comma separated)"            , ""                                  , groupContactsMenus );
		HotkeyEdit     *contactsmenuhotkeyedit                       = new HotkeyEdit(     "GlobalHotkeys", QString( "ContactsMenuShortcuts_Shortcut_%"                      ).replace( "%", QString::number( c ) ), "Shortcut"                                            , ""                                  , groupContactsMenus );
		contactsmenusCurrentChatsCheckboxList.push_back(                   contactsmenucurrentchatsbox                  );
		contactsmenusPendingChatsCheckboxList.push_back(                   contactsmenupendingchatsbox                  );
		contactsmenusRecentChatsCheckboxList.push_back(                    contactsmenurecentchatsbox                   );
		contactsmenusOnlineContactsCheckboxList.push_back(                 contactsmenuonlinecontactsbox                );
		contactsmenusOnlineContactsGroupsEditList.push_back(               contactsmenuonlinecontactsgroupsedit         );
		contactsmenusOnlineContactsIncludeBlockingCheckboxList.push_back(  contactsmenuonlinecontactsincludeblockingbox );
		contactsmenusContactsEditList.push_back(                           contactsmenucontactsedit                     );
		contactsmenusGroupsEditList.push_back(                             contactsmenugroupsedit                       );
		contactsmenusExcludeContactsEditList.push_back(                    contactsmenuexcludecontactsedit              );
		contactsmenusHotkeyEditList.push_back(                             contactsmenuhotkeyedit                       );
		contactsmenucurrentchatsbox->setChecked(                  (*I).first.currentchats                      );
		contactsmenupendingchatsbox->setChecked(                  (*I).first.pendingchats                      );
		contactsmenurecentchatsbox->setChecked(                   (*I).first.recentchats                       );
		contactsmenuonlinecontactsbox->setChecked(                (*I).first.onlinecontacts                    );
		contactsmenuonlinecontactsgroupsedit->setText(            (*I).first.onlinecontactsgroups.join( ", " ) );
		contactsmenuonlinecontactsincludeblockingbox->setChecked( (*I).first.onlinecontactsincludeblocking     );
		contactsmenucontactsedit->setText(                        (*I).first.contacts.join( ", " )             );
		contactsmenugroupsedit->setText(                          (*I).first.groups.join( ", " )               );
		contactsmenuexcludecontactsedit->setText(                 (*I).first.excludecontacts.join( ", " )      );
		contactsmenuhotkeyedit->setText(                          (*I).second                                  );
		contactsmenuonlinecontactsgroupsedit->setEnabled(         (*I).first.onlinecontacts                    );
		contactsmenuonlinecontactsincludeblockingbox->setEnabled( (*I).first.onlinecontacts                    );
		connect( contactsmenuonlinecontactsbox, SIGNAL(toggled(bool)), contactsmenuonlinecontactsgroupsedit, SLOT(setEnabled(bool)) );
		connect( contactsmenuonlinecontactsbox, SIGNAL(toggled(bool)), contactsmenuonlinecontactsincludeblockingbox, SLOT(setEnabled(bool)) );
		contactsmenucurrentchatsbox->show();
		contactsmenupendingchatsbox->show();
		contactsmenurecentchatsbox->show();
		contactsmenuonlinecontactsbox->show();
		contactsmenuonlinecontactsgroupsedit->show();
		contactsmenuonlinecontactsincludeblockingbox->show();
		contactsmenucontactsedit->show();
		contactsmenugroupsedit->show();
		contactsmenuexcludecontactsedit->show();
		contactsmenuhotkeyedit->show();
		c++;
	}
}


void GlobalHotkeys::createDefaultConfiguration()
{
	config_file.addVariable( "GlobalHotkeys", "ShowKadusMainWindow"        , "" );
	config_file.addVariable( "GlobalHotkeys", "HideKadusMainWindow"        , "" );
	config_file.addVariable( "GlobalHotkeys", "ShowHideKadusMainWindow"    , "" );
	config_file.addVariable( "GlobalHotkeys", "ChangeStatus"               , "" );
	config_file.addVariable( "GlobalHotkeys", "ChangeDescription"          , "" );
	config_file.addVariable( "GlobalHotkeys", "TurnSilentModeOn"           , "" );
	config_file.addVariable( "GlobalHotkeys", "TurnSilentModeOff"          , "" );
	config_file.addVariable( "GlobalHotkeys", "ToggleSilentMode"           , "" );
	config_file.addVariable( "GlobalHotkeys", "OpenIncomingChatWindow"     , "" );
	config_file.addVariable( "GlobalHotkeys", "OpenAllIncomingChatWindows" , "" );
	config_file.addVariable( "GlobalHotkeys", "MinimizeOpenedChatWindows"  , "" );
	config_file.addVariable( "GlobalHotkeys", "RestoreMinimizedChatWindows", "" );
	config_file.addVariable( "GlobalHotkeys", "MinimizeRestoreChatWindows" , "" );
	config_file.addVariable( "GlobalHotkeys", "CloseAllChatWindows"        , "" );
	config_file.addVariable( "GlobalHotkeys", "OpenChatWith"               , "" );
	// <---
}


void GlobalHotkeys::configurationUpdated()
{
	// create contactsmenu icons using current icons set
	createContactsMenuIcons();
	// update configuration data
	showKadusMainWindow         = config_file.readEntry( "GlobalHotkeys", "ShowKadusMainWindow"         ).stripWhiteSpace();
	hideKadusMainWindow         = config_file.readEntry( "GlobalHotkeys", "HideKadusMainWindow"         ).stripWhiteSpace();
	showHideKadusMainWindow     = config_file.readEntry( "GlobalHotkeys", "ShowHideKadusMainWindow"     ).stripWhiteSpace();
	changeStatus                = config_file.readEntry( "GlobalHotkeys", "ChangeStatus"                ).stripWhiteSpace();
	changeDescription           = config_file.readEntry( "GlobalHotkeys", "ChangeDescription"           ).stripWhiteSpace();
	turnSilentModeOn            = config_file.readEntry( "GlobalHotkeys", "TurnSilentModeOn"            ).stripWhiteSpace();
	turnSilentModeOff           = config_file.readEntry( "GlobalHotkeys", "TurnSilentModeOff"           ).stripWhiteSpace();
	toggleSilentMode            = config_file.readEntry( "GlobalHotkeys", "ToggleSilentMode"            ).stripWhiteSpace();
	openIncomingChatWindow      = config_file.readEntry( "GlobalHotkeys", "OpenIncomingChatWindow"      ).stripWhiteSpace();
	openAllIncomingChatWindows  = config_file.readEntry( "GlobalHotkeys", "OpenAllIncomingChatWindows"  ).stripWhiteSpace();
	minimizeOpenedChatWindows   = config_file.readEntry( "GlobalHotkeys", "MinimizeOpenedChatWindows"   ).stripWhiteSpace();
	restoreMinimizedChatWindows = config_file.readEntry( "GlobalHotkeys", "RestoreMinimizedChatWindows" ).stripWhiteSpace();
	minimizeRestoreChatWindows  = config_file.readEntry( "GlobalHotkeys", "MinimizeRestoreChatWindows"  ).stripWhiteSpace();
	closeAllChatWindows         = config_file.readEntry( "GlobalHotkeys", "CloseAllChatWindows"         ).stripWhiteSpace();
	openChatWith                = config_file.readEntry( "GlobalHotkeys", "OpenChatWith"                ).stripWhiteSpace();
	// <---
	int c;
	// update contacts shortcuts configuration data and recreate configuration widgets
	contactsshortcuts.clear();
	c = 0;
	while( true )
	{
		QString _contacts = config_file.readEntry( "GlobalHotkeys", QString( "ContactsShortcuts_Contacts_%" ).replace( "%", QString::number( c ) ) ).stripWhiteSpace();
		QString _shortcut = config_file.readEntry( "GlobalHotkeys", QString( "ContactsShortcuts_Shortcut_%" ).replace( "%", QString::number( c ) ) ).stripWhiteSpace();
		if( ( ! _contacts.isEmpty() ) || ( ! _shortcut.isEmpty() ) )
		{
			QStringList _contactslist = QStringList::split( QRegExp(" *, *"), _contacts.trimmed(), false );
			// remove duplicated contacts from the shortcut
			for( int k = _contactslist.size()-1; k > 0; k-- )
			{
				if( _contactslist.indexOf( _contactslist[k] ) != k )
				{
					_contactslist.removeAt( k );
				}
			}
			// remove contacts that do not exist from the shortcut
			for( QStringList::Iterator I = _contactslist.begin(); I != _contactslist.end(); )
			{
				if( ! userlist->containsAltNick( (*I), FalseForAnonymous ) )
				{
					I = _contactslist.erase( I );
				}
				else
				{
					I++;
				}
			}
			contactsshortcuts.push_back( QPair<QStringList,QString>( _contactslist, _shortcut ) );
		}
		else if( contactsHotkeyEditList.count() == 0 )
		{
			break;
		}
		if( ( contactsHotkeyEditList.count() > 0 ) && ( c >= contactsHotkeyEditList.count()-1 ) )
		{
			break;
		}
		c++;
	}
	c = 0;
	for( QList< QPair<QStringList,QString> >::Iterator I = contactsshortcuts.begin(); I != contactsshortcuts.end(); I++ )
	{
		config_file.writeEntry( "GlobalHotkeys", QString( "ContactsShortcuts_Contacts_%" ).replace( "%", QString::number( c ) ), (*I).first.join( ", " ) );
		config_file.writeEntry( "GlobalHotkeys", QString( "ContactsShortcuts_Shortcut_%" ).replace( "%", QString::number( c ) ), (*I).second             );
		c++;
	}
	config_file.removeVariable( "GlobalHotkeys", QString( "ContactsShortcuts_Contacts_%" ).replace( "%", QString::number( c ) ) );
	config_file.removeVariable( "GlobalHotkeys", QString( "ContactsShortcuts_Shortcut_%" ).replace( "%", QString::number( c ) ) );
	for( int cc = c+1; cc < contactsHotkeyEditList.count(); cc++ )
	{
		config_file.removeVariable( "GlobalHotkeys", QString( "ContactsShortcuts_Contacts_%" ).replace( "%", QString::number( cc ) ) );
		config_file.removeVariable( "GlobalHotkeys", QString( "ContactsShortcuts_Shortcut_%" ).replace( "%", QString::number( cc ) ) );
	}
	if( contactsHotkeyEditList.count() > 0 )
	{
		for( QList< QPointer<ConfigLineEdit> >::Iterator I = contactsNamesEditList.begin(); I != contactsNamesEditList.end(); I++ )
		{
			if( ! (*I).isNull() ) delete (ConfigLineEdit*) (*I);
		}
		contactsNamesEditList.clear();
		for( QList< QPointer<HotkeyEdit> >::Iterator I = contactsHotkeyEditList.begin(); I != contactsHotkeyEditList.end(); I++ )
		{
			if( ! (*I).isNull() ) delete (HotkeyEdit*) (*I);
		}
		contactsHotkeyEditList.clear();
		c = 0;
		for( QList< QPair<QStringList,QString> >::Iterator I = contactsshortcuts.begin(); I != contactsshortcuts.end(); I++ )
		{
			ConfigLineEdit *contactnameedit   = new ConfigLineEdit( "GlobalHotkeys", QString( "ContactsShortcuts_Contacts_%" ).replace( "%", QString::number( c ) ), "Contacts (comma separated)", "", groupContacts );
			HotkeyEdit     *contacthotkeyedit = new HotkeyEdit(     "GlobalHotkeys", QString( "ContactsShortcuts_Shortcut_%" ).replace( "%", QString::number( c ) ), "Shortcut"                  , "", groupContacts );
			contactsNamesEditList.push_back(  contactnameedit   );
			contactsHotkeyEditList.push_back( contacthotkeyedit );
			contactnameedit->setText( (*I).first.join( ", " ) );
			contacthotkeyedit->setText( (*I).second );
			contactnameedit->show();
			contacthotkeyedit->show();
			c++;
		}
	}
	// update contacts menus shortcuts configuration data and recreate configuration widgets
	contactsmenusshortcuts.clear();
	c = 0;
	while( true )
	{
		bool    _currentchats                  = config_file.readBoolEntry( "GlobalHotkeys", QString( "ContactsMenuShortcuts_CurrentChats_%"                  ).replace( "%", QString::number( c ) ) );
		bool    _pendingchats                  = config_file.readBoolEntry( "GlobalHotkeys", QString( "ContactsMenuShortcuts_PendingChats_%"                  ).replace( "%", QString::number( c ) ) );
		bool    _recentchats                   = config_file.readBoolEntry( "GlobalHotkeys", QString( "ContactsMenuShortcuts_RecentChats_%"                   ).replace( "%", QString::number( c ) ) );
		bool    _onlinecontacts                = config_file.readBoolEntry( "GlobalHotkeys", QString( "ContactsMenuShortcuts_OnlineContacts_%"                ).replace( "%", QString::number( c ) ) );
		QString _onlinecontactsgroups          = config_file.readEntry(     "GlobalHotkeys", QString( "ContactsMenuShortcuts_OnlineContactsGroups_%"          ).replace( "%", QString::number( c ) ) ).stripWhiteSpace();
		bool    _onlinecontactsincludeblocking = config_file.readBoolEntry( "GlobalHotkeys", QString( "ContactsMenuShortcuts_OnlineContactsIncludeBlocking_%" ).replace( "%", QString::number( c ) ) );
		QString _contacts                      = config_file.readEntry(     "GlobalHotkeys", QString( "ContactsMenuShortcuts_Contacts_%"                      ).replace( "%", QString::number( c ) ) ).stripWhiteSpace();
		QString _groups                        = config_file.readEntry(     "GlobalHotkeys", QString( "ContactsMenuShortcuts_Groups_%"                        ).replace( "%", QString::number( c ) ) ).stripWhiteSpace();
		QString _excludecontacts               = config_file.readEntry(     "GlobalHotkeys", QString( "ContactsMenuShortcuts_ExcludeContacts_%"               ).replace( "%", QString::number( c ) ) ).stripWhiteSpace();
		QString _shortcut                      = config_file.readEntry(     "GlobalHotkeys", QString( "ContactsMenuShortcuts_Shortcut_%"                      ).replace( "%", QString::number( c ) ) ).stripWhiteSpace();
		if( ( _currentchats ) || ( _pendingchats ) || ( _recentchats ) || ( _onlinecontacts ) || ( ! _contacts.isEmpty() ) || ( ! _groups.isEmpty() ) || ( ! _excludecontacts.isEmpty() ) || ( ! _shortcut.isEmpty() ) )
		{
			if( ! _onlinecontacts )
			{
				_onlinecontactsgroups = "";
				_onlinecontactsincludeblocking = false;
			}
			// _onlinecontactsgroups
			QStringList _onlinecontactsgroupslist = QStringList::split( QRegExp(" *, *"), _onlinecontactsgroups.trimmed(), false );
			// remove duplicated groups from the shortcut
			for( int k = _onlinecontactsgroupslist.size()-1; k > 0; k-- )
			{
				if( _onlinecontactsgroupslist.indexOf( _onlinecontactsgroupslist[k] ) != k )
				{
					_onlinecontactsgroupslist.removeAt( k );
				}
			}
			// remove groups that do not exist from the shortcut
			for( QStringList::Iterator I = _onlinecontactsgroupslist.begin(); I != _onlinecontactsgroupslist.end(); )
			{
				if( ! groups_manager->groups().contains( (*I) ) )
				{
					I = _onlinecontactsgroupslist.erase( I );
				}
				else
				{
					I++;
				}
			}
			// _contacts
			QStringList _contactslist = QStringList::split( QRegExp(" *, *"), _contacts.trimmed(), false );
			// remove duplicated contacts from the shortcut
			for( int k = _contactslist.size()-1; k > 0; k-- )
			{
				if( _contactslist.indexOf( _contactslist[k] ) != k )
				{
					_contactslist.removeAt( k );
				}
			}
			// remove contacts that do not exist from the shortcut
			for( QStringList::Iterator I = _contactslist.begin(); I != _contactslist.end(); )
			{
				if( ! userlist->containsAltNick( (*I), FalseForAnonymous ) )
				{
					I = _contactslist.erase( I );
				}
				else
				{
					I++;
				}
			}
			// _groups
			QStringList _groupslist = QStringList::split( QRegExp(" *, *"), _groups.trimmed(), false );
			// remove duplicated groups from the shortcut
			for( int k = _groupslist.size()-1; k > 0; k-- )
			{
				if( _groupslist.indexOf( _groupslist[k] ) != k )
				{
					_groupslist.removeAt( k );
				}
			}
			// remove groups that do not exist from the shortcut
			for( QStringList::Iterator I = _groupslist.begin(); I != _groupslist.end(); )
			{
				if( ! groups_manager->groupExists( (*I) ) )
				{
					I = _groupslist.erase( I );
				}
				else
				{
					I++;
				}
			}
			// _excludecontacts
			QStringList _excludecontactslist = QStringList::split( QRegExp(" *, *"), _excludecontacts.trimmed(), false );
			// remove duplicated contacts from the shortcut
			for( int k = _excludecontactslist.size()-1; k > 0; k-- )
			{
				if( _excludecontactslist.indexOf( _excludecontactslist[k] ) != k )
				{
					_excludecontactslist.removeAt( k );
				}
			}
			// remove contacts that do not exist from the shortcut
			for( QStringList::Iterator I = _excludecontactslist.begin(); I != _excludecontactslist.end(); )
			{
				if( ! userlist->containsAltNick( (*I), FalseForAnonymous ) )
				{
					I = _excludecontactslist.erase( I );
				}
				else
				{
					I++;
				}
			}
			// set ContactsMenuItemData
			ContactsMenuItemData _contactsmenuitemdata;
			_contactsmenuitemdata.currentchats                  = _currentchats;
			_contactsmenuitemdata.pendingchats                  = _pendingchats;
			_contactsmenuitemdata.recentchats                   = _recentchats;
			_contactsmenuitemdata.onlinecontacts                = _onlinecontacts;
			_contactsmenuitemdata.onlinecontactsgroups          = _onlinecontactsgroupslist;
			_contactsmenuitemdata.onlinecontactsincludeblocking = _onlinecontactsincludeblocking;
			_contactsmenuitemdata.contacts                      = _contactslist;
			_contactsmenuitemdata.groups                        = _groupslist;
			_contactsmenuitemdata.excludecontacts               = _excludecontactslist;
			contactsmenusshortcuts.push_back( QPair<ContactsMenuItemData,QString>( _contactsmenuitemdata, _shortcut ) );
		}
		else if( contactsmenusHotkeyEditList.count() == 0 )
		{
			break;
		}
		if( ( contactsmenusHotkeyEditList.count() > 0 ) && ( c >= contactsmenusHotkeyEditList.count()-1 ) )
		{
			break;
		}
		c++;
	}
	c = 0;
	for( QList< QPair<ContactsMenuItemData,QString> >::Iterator I = contactsmenusshortcuts.begin(); I != contactsmenusshortcuts.end(); I++ )
	{
		config_file.writeEntry( "GlobalHotkeys", QString( "ContactsMenuShortcuts_CurrentChats_%"                  ).replace( "%", QString::number( c ) ), (*I).first.currentchats                      );
		config_file.writeEntry( "GlobalHotkeys", QString( "ContactsMenuShortcuts_PendingChats_%"                  ).replace( "%", QString::number( c ) ), (*I).first.pendingchats                      );
		config_file.writeEntry( "GlobalHotkeys", QString( "ContactsMenuShortcuts_RecentChats_%"                   ).replace( "%", QString::number( c ) ), (*I).first.recentchats                       );
		config_file.writeEntry( "GlobalHotkeys", QString( "ContactsMenuShortcuts_OnlineContacts_%"                ).replace( "%", QString::number( c ) ), (*I).first.onlinecontacts                    );
		config_file.writeEntry( "GlobalHotkeys", QString( "ContactsMenuShortcuts_OnlineContactsGroups_%"          ).replace( "%", QString::number( c ) ), (*I).first.onlinecontactsgroups.join( ", " ) );
		config_file.writeEntry( "GlobalHotkeys", QString( "ContactsMenuShortcuts_OnlineContactsIncludeBlocking_%" ).replace( "%", QString::number( c ) ), (*I).first.onlinecontactsincludeblocking     );
		config_file.writeEntry( "GlobalHotkeys", QString( "ContactsMenuShortcuts_Contacts_%"                      ).replace( "%", QString::number( c ) ), (*I).first.contacts.join( ", " )             );
		config_file.writeEntry( "GlobalHotkeys", QString( "ContactsMenuShortcuts_Groups_%"                        ).replace( "%", QString::number( c ) ), (*I).first.groups.join( ", " )               );
		config_file.writeEntry( "GlobalHotkeys", QString( "ContactsMenuShortcuts_ExcludeContacts_%"               ).replace( "%", QString::number( c ) ), (*I).first.excludecontacts.join( ", " )      );
		config_file.writeEntry( "GlobalHotkeys", QString( "ContactsMenuShortcuts_Shortcut_%"                      ).replace( "%", QString::number( c ) ), (*I).second                                  );
		c++;
	}
	config_file.removeVariable( "GlobalHotkeys", QString( "ContactsMenuShortcuts_CurrentChats_%"                  ).replace( "%", QString::number( c ) ) );
	config_file.removeVariable( "GlobalHotkeys", QString( "ContactsMenuShortcuts_PendingChats_%"                  ).replace( "%", QString::number( c ) ) );
	config_file.removeVariable( "GlobalHotkeys", QString( "ContactsMenuShortcuts_RecentChats_%"                   ).replace( "%", QString::number( c ) ) );
	config_file.removeVariable( "GlobalHotkeys", QString( "ContactsMenuShortcuts_OnlineContacts_%"                ).replace( "%", QString::number( c ) ) );
	config_file.removeVariable( "GlobalHotkeys", QString( "ContactsMenuShortcuts_OnlineContactsGroups_%"          ).replace( "%", QString::number( c ) ) );
	config_file.removeVariable( "GlobalHotkeys", QString( "ContactsMenuShortcuts_OnlineContactsIncludeBlocking_%" ).replace( "%", QString::number( c ) ) );
	config_file.removeVariable( "GlobalHotkeys", QString( "ContactsMenuShortcuts_Contacts_%"                      ).replace( "%", QString::number( c ) ) );
	config_file.removeVariable( "GlobalHotkeys", QString( "ContactsMenuShortcuts_Groups_%"                        ).replace( "%", QString::number( c ) ) );
	config_file.removeVariable( "GlobalHotkeys", QString( "ContactsMenuShortcuts_ExcludeContacts_%"               ).replace( "%", QString::number( c ) ) );
	config_file.removeVariable( "GlobalHotkeys", QString( "ContactsMenuShortcuts_Shortcut_%"                      ).replace( "%", QString::number( c ) ) );
	for( int cc = c+1; cc < contactsmenusHotkeyEditList.count(); cc++ )
	{
		config_file.removeVariable( "GlobalHotkeys", QString( "ContactsMenuShortcuts_CurrentChats_%"                  ).replace( "%", QString::number( c ) ) );
		config_file.removeVariable( "GlobalHotkeys", QString( "ContactsMenuShortcuts_PendingChats_%"                  ).replace( "%", QString::number( c ) ) );
		config_file.removeVariable( "GlobalHotkeys", QString( "ContactsMenuShortcuts_RecentChats_%"                   ).replace( "%", QString::number( c ) ) );
		config_file.removeVariable( "GlobalHotkeys", QString( "ContactsMenuShortcuts_OnlineContacts_%"                ).replace( "%", QString::number( c ) ) );
		config_file.removeVariable( "GlobalHotkeys", QString( "ContactsMenuShortcuts_OnlineContactsGroups_%"          ).replace( "%", QString::number( c ) ) );
		config_file.removeVariable( "GlobalHotkeys", QString( "ContactsMenuShortcuts_OnlineContactsIncludeBlocking_%" ).replace( "%", QString::number( c ) ) );
		config_file.removeVariable( "GlobalHotkeys", QString( "ContactsMenuShortcuts_Contacts_%"                      ).replace( "%", QString::number( c ) ) );
		config_file.removeVariable( "GlobalHotkeys", QString( "ContactsMenuShortcuts_Groups_%"                        ).replace( "%", QString::number( c ) ) );
		config_file.removeVariable( "GlobalHotkeys", QString( "ContactsMenuShortcuts_ExcludeContacts_%"               ).replace( "%", QString::number( c ) ) );
		config_file.removeVariable( "GlobalHotkeys", QString( "ContactsMenuShortcuts_Shortcut_%"                      ).replace( "%", QString::number( c ) ) );
	}
	if( contactsmenusHotkeyEditList.count() > 0 )
	{
		for( QList< QPointer<ConfigCheckBox> >::Iterator I = contactsmenusCurrentChatsCheckboxList.begin(); I != contactsmenusCurrentChatsCheckboxList.end(); I++ )
		{
			if( ! (*I).isNull() ) delete (ConfigCheckBox*) (*I);
		}
		contactsmenusCurrentChatsCheckboxList.clear();
		for( QList< QPointer<ConfigCheckBox> >::Iterator I = contactsmenusPendingChatsCheckboxList.begin(); I != contactsmenusPendingChatsCheckboxList.end(); I++ )
		{
			if( ! (*I).isNull() ) delete (ConfigCheckBox*) (*I);
		}
		contactsmenusPendingChatsCheckboxList.clear();
		for( QList< QPointer<ConfigCheckBox> >::Iterator I = contactsmenusRecentChatsCheckboxList.begin(); I != contactsmenusRecentChatsCheckboxList.end(); I++ )
		{
			if( ! (*I).isNull() ) delete (ConfigCheckBox*) (*I);
		}
		contactsmenusRecentChatsCheckboxList.clear();
		for( QList< QPointer<ConfigCheckBox> >::Iterator I = contactsmenusOnlineContactsCheckboxList.begin(); I != contactsmenusOnlineContactsCheckboxList.end(); I++ )
		{
			if( ! (*I).isNull() ) delete (ConfigCheckBox*) (*I);
		}
		contactsmenusOnlineContactsCheckboxList.clear();
		for( QList< QPointer<ConfigLineEdit> >::Iterator I = contactsmenusOnlineContactsGroupsEditList.begin(); I != contactsmenusOnlineContactsGroupsEditList.end(); I++ )
		{
			if( ! (*I).isNull() ) delete (ConfigLineEdit*) (*I);
		}
		contactsmenusOnlineContactsGroupsEditList.clear();
		for( QList< QPointer<ConfigCheckBox> >::Iterator I = contactsmenusOnlineContactsIncludeBlockingCheckboxList.begin(); I != contactsmenusOnlineContactsIncludeBlockingCheckboxList.end(); I++ )
		{
			if( ! (*I).isNull() ) delete (ConfigCheckBox*) (*I);
		}
		contactsmenusOnlineContactsIncludeBlockingCheckboxList.clear();
		for( QList< QPointer<ConfigLineEdit> >::Iterator I = contactsmenusContactsEditList.begin(); I != contactsmenusContactsEditList.end(); I++ )
		{
			if( ! (*I).isNull() ) delete (ConfigLineEdit*) (*I);
		}
		contactsmenusContactsEditList.clear();
		for( QList< QPointer<ConfigLineEdit> >::Iterator I = contactsmenusGroupsEditList.begin(); I != contactsmenusGroupsEditList.end(); I++ )
		{
			if( ! (*I).isNull() ) delete (ConfigLineEdit*) (*I);
		}
		contactsmenusGroupsEditList.clear();
		for( QList< QPointer<ConfigLineEdit> >::Iterator I = contactsmenusExcludeContactsEditList.begin(); I != contactsmenusExcludeContactsEditList.end(); I++ )
		{
			if( ! (*I).isNull() ) delete (ConfigLineEdit*) (*I);
		}
		contactsmenusExcludeContactsEditList.clear();
		for( QList< QPointer<HotkeyEdit> >::Iterator I = contactsmenusHotkeyEditList.begin(); I != contactsmenusHotkeyEditList.end(); I++ )
		{
			if( ! (*I).isNull() ) delete (HotkeyEdit*) (*I);
		}
		contactsmenusHotkeyEditList.clear();
		c = 0;
		for( QList< QPair<ContactsMenuItemData,QString> >::Iterator I = contactsmenusshortcuts.begin(); I != contactsmenusshortcuts.end(); I++ )
		{
			ConfigCheckBox *contactsmenucurrentchatsbox                  = new ConfigCheckBox( "GlobalHotkeys", QString( "ContactsMenuShortcuts_CurrentChats_%"                  ).replace( "%", QString::number( c ) ), "Include current chats"                               , ""                                  , groupContactsMenus );
			ConfigCheckBox *contactsmenupendingchatsbox                  = new ConfigCheckBox( "GlobalHotkeys", QString( "ContactsMenuShortcuts_PendingChats_%"                  ).replace( "%", QString::number( c ) ), "Include chats with pending messages"                 , ""                                  , groupContactsMenus );
			ConfigCheckBox *contactsmenurecentchatsbox                   = new ConfigCheckBox( "GlobalHotkeys", QString( "ContactsMenuShortcuts_RecentChats_%"                   ).replace( "%", QString::number( c ) ), "Include recent chats"                                , ""                                  , groupContactsMenus );
			ConfigCheckBox *contactsmenuonlinecontactsbox                = new ConfigCheckBox( "GlobalHotkeys", QString( "ContactsMenuShortcuts_OnlineContacts_%"                ).replace( "%", QString::number( c ) ), "Include online contacts"                             , ""                                  , groupContactsMenus );
			ConfigLineEdit *contactsmenuonlinecontactsgroupsedit         = new ConfigLineEdit( "GlobalHotkeys", QString( "ContactsMenuShortcuts_OnlineContactsGroups_%"          ).replace( "%", QString::number( c ) ), "only from these groups (comma separated)"            , "leave empty to disable this filter", groupContactsMenus );
			ConfigCheckBox *contactsmenuonlinecontactsincludeblockingbox = new ConfigCheckBox( "GlobalHotkeys", QString( "ContactsMenuShortcuts_OnlineContactsIncludeBlocking_%" ).replace( "%", QString::number( c ) ), "Treat contacts blocking me as online"                , ""                                  , groupContactsMenus );
			ConfigLineEdit *contactsmenucontactsedit                     = new ConfigLineEdit( "GlobalHotkeys", QString( "ContactsMenuShortcuts_Contacts_%"                      ).replace( "%", QString::number( c ) ), "Include these contacts (comma separated)"            , ""                                  , groupContactsMenus );
			ConfigLineEdit *contactsmenugroupsedit                       = new ConfigLineEdit( "GlobalHotkeys", QString( "ContactsMenuShortcuts_Groups_%"                        ).replace( "%", QString::number( c ) ), "Include contacts from these gropus (comma separated)", ""                                  , groupContactsMenus );
			ConfigLineEdit *contactsmenuexcludecontactsedit              = new ConfigLineEdit( "GlobalHotkeys", QString( "ContactsMenuShortcuts_ExcludeContacts_%"               ).replace( "%", QString::number( c ) ), "Exclude these contacts (comma separated)"            , ""                                  , groupContactsMenus );
			HotkeyEdit     *contactsmenuhotkeyedit                       = new HotkeyEdit(     "GlobalHotkeys", QString( "ContactsMenuShortcuts_Shortcut_%"                      ).replace( "%", QString::number( c ) ), "Shortcut"                                            , ""                                  , groupContactsMenus );
			contactsmenusCurrentChatsCheckboxList.push_back(                   contactsmenucurrentchatsbox                  );
			contactsmenusPendingChatsCheckboxList.push_back(                   contactsmenupendingchatsbox                  );
			contactsmenusRecentChatsCheckboxList.push_back(                    contactsmenurecentchatsbox                   );
			contactsmenusOnlineContactsCheckboxList.push_back(                 contactsmenuonlinecontactsbox                );
			contactsmenusOnlineContactsGroupsEditList.push_back(               contactsmenuonlinecontactsgroupsedit         );
			contactsmenusOnlineContactsIncludeBlockingCheckboxList.push_back(  contactsmenuonlinecontactsincludeblockingbox );
			contactsmenusContactsEditList.push_back(                           contactsmenucontactsedit                     );
			contactsmenusGroupsEditList.push_back(                             contactsmenugroupsedit                       );
			contactsmenusExcludeContactsEditList.push_back(                    contactsmenuexcludecontactsedit              );
			contactsmenusHotkeyEditList.push_back(                             contactsmenuhotkeyedit                       );
			contactsmenucurrentchatsbox->setChecked(                  (*I).first.currentchats                      );
			contactsmenupendingchatsbox->setChecked(                  (*I).first.pendingchats                      );
			contactsmenurecentchatsbox->setChecked(                   (*I).first.recentchats                       );
			contactsmenuonlinecontactsbox->setChecked(                (*I).first.onlinecontacts                    );
			contactsmenuonlinecontactsgroupsedit->setText(            (*I).first.onlinecontactsgroups.join( ", " ) );
			contactsmenuonlinecontactsincludeblockingbox->setChecked( (*I).first.onlinecontactsincludeblocking     );
			contactsmenucontactsedit->setText(                        (*I).first.contacts.join( ", " )             );
			contactsmenugroupsedit->setText(                          (*I).first.groups.join( ", " )               );
			contactsmenuexcludecontactsedit->setText(                 (*I).first.excludecontacts.join( ", " )      );
			contactsmenuhotkeyedit->setText(                          (*I).second                                  );
			contactsmenuonlinecontactsgroupsedit->setEnabled(         (*I).first.onlinecontacts                    );
			contactsmenuonlinecontactsincludeblockingbox->setEnabled( (*I).first.onlinecontacts                    );
			connect( contactsmenuonlinecontactsbox, SIGNAL(toggled(bool)), contactsmenuonlinecontactsgroupsedit, SLOT(setEnabled(bool)) );
			connect( contactsmenuonlinecontactsbox, SIGNAL(toggled(bool)), contactsmenuonlinecontactsincludeblockingbox, SLOT(setEnabled(bool)) );
			contactsmenucurrentchatsbox->show();
			contactsmenupendingchatsbox->show();
			contactsmenurecentchatsbox->show();
			contactsmenuonlinecontactsbox->show();
			contactsmenuonlinecontactsgroupsedit->show();
			contactsmenuonlinecontactsincludeblockingbox->show();
			contactsmenucontactsedit->show();
			contactsmenugroupsedit->show();
			contactsmenuexcludecontactsedit->show();
			contactsmenuhotkeyedit->show();
			c++;
		}
	}
	// stop the hotkeys timer
	if( hotkeysTimer->isActive() )
	{
		hotkeysTimer->stop();
	}
	// new X11 display
	if( display != NULL )
	{
		XCloseDisplay( display );
	}
	display = XOpenDisplay( 0 );
	// clear hotkeys list
	for( QMap<QString,Hotkey*>::Iterator I = hotkeys.begin(); I != hotkeys.end(); I++ )
	{
		delete (*I);
	}
	hotkeys.clear();
	// update hotkeys list
	hotkeys.insert( "ShowKadusMainWindow"        , new Hotkey( showKadusMainWindow         ) );
	hotkeys.insert( "HideKadusMainWindow"        , new Hotkey( hideKadusMainWindow         ) );
	hotkeys.insert( "ShowHideKadusMainWindow"    , new Hotkey( showHideKadusMainWindow     ) );
	hotkeys.insert( "ChangeStatus"               , new Hotkey( changeStatus                ) );
	hotkeys.insert( "ChangeDescription"          , new Hotkey( changeDescription           ) );
	hotkeys.insert( "TurnSilentModeOn"           , new Hotkey( turnSilentModeOn            ) );
	hotkeys.insert( "TurnSilentModeOff"          , new Hotkey( turnSilentModeOff           ) );
	hotkeys.insert( "ToggleSilentMode"           , new Hotkey( toggleSilentMode            ) );
	hotkeys.insert( "OpenIncomingChatWindow"     , new Hotkey( openIncomingChatWindow      ) );
	hotkeys.insert( "OpenAllIncomingChatWindows" , new Hotkey( openAllIncomingChatWindows  ) );
	hotkeys.insert( "MinimizeOpenedChatWindows"  , new Hotkey( minimizeOpenedChatWindows   ) );
	hotkeys.insert( "RestoreMinimizedChatWindows", new Hotkey( restoreMinimizedChatWindows ) );
	hotkeys.insert( "MinimizeRestoreChatWindows" , new Hotkey( minimizeRestoreChatWindows  ) );
	hotkeys.insert( "CloseAllChatWindows"        , new Hotkey( closeAllChatWindows         ) );
	hotkeys.insert( "OpenChatWith"               , new Hotkey( openChatWith                ) );
	// <---
	c = 0;
	for( QList< QPair<QStringList,QString> >::Iterator I = contactsshortcuts.begin(); I != contactsshortcuts.end(); I++ )
	{
		hotkeys.insert( QString( "ContactsShortcut_%" ).replace( "%", QString::number( c ) ), new Hotkey( (*I).second ) );
		c++;
	}
	for( QList< QPair<ContactsMenuItemData,QString> >::Iterator I = contactsmenusshortcuts.begin(); I != contactsmenusshortcuts.end(); I++ )
	{
		hotkeys.insert( QString( "ContactsMenusShortcut_%" ).replace( "%", QString::number( c ) ), new Hotkey( (*I).second ) );
		c++;
	}
	// grab hotkeys
	grabHotkeys( hotkeys );
	// start the hotkeys timer
	hotkeysTimer->start( GLOBALHOTKEYS_HOTKEYSTIMERINTERVAL, false );
}


void GlobalHotkeys::grabHotkeys( QMap<QString,Hotkey*> hotkeys )
{
	uint modifiers;
	for( QMap<QString,Hotkey*>::Iterator I = hotkeys.begin(); I != hotkeys.end(); I++ )
	{
		if( (*I)->keycode != 0 )
		{
			modifiers = 0;
			modifiers |= ( (*I)->shift   ? GLOBALHOTKEYS_X11SHIFTMASK   : 0 );
			modifiers |= ( (*I)->control ? GLOBALHOTKEYS_X11CONTROLMASK : 0 );
			modifiers |= ( (*I)->alt     ? GLOBALHOTKEYS_X11ALTMASK     : 0 );
			modifiers |= ( (*I)->altgr   ? GLOBALHOTKEYS_X11ALTGRMASK   : 0 );
			modifiers |= ( (*I)->super   ? GLOBALHOTKEYS_X11SUPERMASK   : 0 );
			// install empty messages handler to avoid warning being printed to the output
			messageshandled = 0;
			QtMsgHandler previousmsghandler = qInstallMsgHandler( EmptyMsgHandler );
			// pure hotkey
			XGrabKey(
				display, (*I)->keycode, modifiers,
				DefaultRootWindow( display ), False, GrabModeAsync, GrabModeAsync
				);
			// hotkey with CapsLock
			XGrabKey(
				display, (*I)->keycode, modifiers | GLOBALHOTKEYS_X11CAPSLOCKMASK,
				DefaultRootWindow( display ), False, GrabModeAsync, GrabModeAsync
				);
			// hotkey with NumLock
			XGrabKey(
				display, (*I)->keycode, modifiers | GLOBALHOTKEYS_X11NUMLOCKMASK,
				DefaultRootWindow( display ), False, GrabModeAsync, GrabModeAsync
				);
			// hotkey with CapsLock and NumLock
			XGrabKey(
				display, (*I)->keycode, modifiers | GLOBALHOTKEYS_X11CAPSLOCKMASK | GLOBALHOTKEYS_X11NUMLOCKMASK,
				DefaultRootWindow( display ), False, GrabModeAsync, GrabModeAsync
				);
			// sync the X11 connection, so that we are sure the X11 errors will be handled now
			XSync( display, false );
			// install the previous messages handler
			qInstallMsgHandler( previousmsghandler );
			// check for X11 errors
			if( messageshandled > 0 )
			{
				QMessageBox *messagebox = new QMessageBox(
						qApp->translate( "@default", "Kadu - Global hotkeys" ),
						qApp->translate( "@default", "Hotkey %% is used by another application." ).replace( "%%", (*I)->comment ),
						QMessageBox::Warning,
						QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton,
						qApp->activeWindow()
					);
				messagebox->show();
				messageshandled = 0;
			}
		}
	}
}


void GlobalHotkeys::contactsAddNewButtonPressed()
{
	int c = 0;
	QList< QPointer<HotkeyEdit> >::Iterator I_contactsHotkeyEditList;
	I_contactsHotkeyEditList  = contactsHotkeyEditList.begin();
	while( ( I_contactsHotkeyEditList != contactsHotkeyEditList.end() ) )
	{
		c++;
		I_contactsHotkeyEditList++;
	}
	ConfigLineEdit *contactnameedit   = new ConfigLineEdit( "GlobalHotkeys", QString( "ContactsShortcuts_Contacts_%" ).replace( "%", QString::number( c ) ), "Contacts (comma separated)", "", groupContacts );
	HotkeyEdit     *contacthotkeyedit = new HotkeyEdit(     "GlobalHotkeys", QString( "ContactsShortcuts_Shortcut_%" ).replace( "%", QString::number( c ) ), "Shortcut"                  , "", groupContacts );
	contactsNamesEditList.push_back(  contactnameedit   );
	contactsHotkeyEditList.push_back( contacthotkeyedit );
	contactnameedit->show();
	contacthotkeyedit->show();
}


void GlobalHotkeys::contactsmenusAddNewButtonPressed()
{
	int c = 0;
	QList< QPointer<HotkeyEdit> >::Iterator I_contactsmenusHotkeyEditList;
	I_contactsmenusHotkeyEditList  = contactsmenusHotkeyEditList.begin();
	while( ( I_contactsmenusHotkeyEditList != contactsmenusHotkeyEditList.end() ) )
	{
		c++;
		I_contactsmenusHotkeyEditList++;
	}
	ConfigCheckBox *contactsmenucurrentchatsbox                  = new ConfigCheckBox( "GlobalHotkeys", QString( "ContactsMenuShortcuts_CurrentChats_%"                  ).replace( "%", QString::number( c ) ), "Include current chats"                               , ""                                  , groupContactsMenus );
	ConfigCheckBox *contactsmenupendingchatsbox                  = new ConfigCheckBox( "GlobalHotkeys", QString( "ContactsMenuShortcuts_PendingChats_%"                  ).replace( "%", QString::number( c ) ), "Include chats with pending messages"                 , ""                                  , groupContactsMenus );
	ConfigCheckBox *contactsmenurecentchatsbox                   = new ConfigCheckBox( "GlobalHotkeys", QString( "ContactsMenuShortcuts_RecentChats_%"                   ).replace( "%", QString::number( c ) ), "Include recent chats"                                , ""                                  , groupContactsMenus );
	ConfigCheckBox *contactsmenuonlinecontactsbox                = new ConfigCheckBox( "GlobalHotkeys", QString( "ContactsMenuShortcuts_OnlineContacts_%"                ).replace( "%", QString::number( c ) ), "Include online contacts"                             , ""                                  , groupContactsMenus );
	ConfigLineEdit *contactsmenuonlinecontactsgroupsedit         = new ConfigLineEdit( "GlobalHotkeys", QString( "ContactsMenuShortcuts_OnlineContactsGroups_%"          ).replace( "%", QString::number( c ) ), "only from these groups (comma separated)"            , "leave empty to disable this filter", groupContactsMenus );
	ConfigCheckBox *contactsmenuonlinecontactsincludeblockingbox = new ConfigCheckBox( "GlobalHotkeys", QString( "ContactsMenuShortcuts_OnlineContactsIncludeBlocking_%" ).replace( "%", QString::number( c ) ), "Treat contacts blocking me as online"                , ""                                  , groupContactsMenus );
	ConfigLineEdit *contactsmenucontactsedit                     = new ConfigLineEdit( "GlobalHotkeys", QString( "ContactsMenuShortcuts_Contacts_%"                      ).replace( "%", QString::number( c ) ), "Include these contacts (comma separated)"            , ""                                  , groupContactsMenus );
	ConfigLineEdit *contactsmenugroupsedit                       = new ConfigLineEdit( "GlobalHotkeys", QString( "ContactsMenuShortcuts_Groups_%"                        ).replace( "%", QString::number( c ) ), "Include contacts from these gropus (comma separated)", ""                                  , groupContactsMenus );
	ConfigLineEdit *contactsmenuexcludecontactsedit              = new ConfigLineEdit( "GlobalHotkeys", QString( "ContactsMenuShortcuts_ExcludeContacts_%"               ).replace( "%", QString::number( c ) ), "Exclude these contacts (comma separated)"            , ""                                  , groupContactsMenus );
	HotkeyEdit     *contactsmenuhotkeyedit                       = new HotkeyEdit(     "GlobalHotkeys", QString( "ContactsMenuShortcuts_Shortcut_%"                      ).replace( "%", QString::number( c ) ), "Shortcut"                                            , ""                                  , groupContactsMenus );
	contactsmenusCurrentChatsCheckboxList.push_back(                   contactsmenucurrentchatsbox                  );
	contactsmenusPendingChatsCheckboxList.push_back(                   contactsmenupendingchatsbox                  );
	contactsmenusRecentChatsCheckboxList.push_back(                    contactsmenurecentchatsbox                   );
	contactsmenusOnlineContactsCheckboxList.push_back(                 contactsmenuonlinecontactsbox                );
	contactsmenusOnlineContactsGroupsEditList.push_back(               contactsmenuonlinecontactsgroupsedit         );
	contactsmenusOnlineContactsIncludeBlockingCheckboxList.push_back(  contactsmenuonlinecontactsincludeblockingbox );
	contactsmenusContactsEditList.push_back(                           contactsmenucontactsedit                     );
	contactsmenusGroupsEditList.push_back(                             contactsmenugroupsedit                       );
	contactsmenusExcludeContactsEditList.push_back(                    contactsmenuexcludecontactsedit              );
	contactsmenusHotkeyEditList.push_back(                             contactsmenuhotkeyedit                       );
	contactsmenuonlinecontactsgroupsedit->setEnabled( false );
	contactsmenuonlinecontactsincludeblockingbox->setEnabled( false );
	connect( contactsmenuonlinecontactsbox, SIGNAL(toggled(bool)), contactsmenuonlinecontactsgroupsedit, SLOT(setEnabled(bool)) );
	connect( contactsmenuonlinecontactsbox, SIGNAL(toggled(bool)), contactsmenuonlinecontactsincludeblockingbox, SLOT(setEnabled(bool)) );
	contactsmenucurrentchatsbox->show();
	contactsmenupendingchatsbox->show();
	contactsmenurecentchatsbox->show();
	contactsmenuonlinecontactsbox->show();
	contactsmenuonlinecontactsgroupsedit->show();
	contactsmenuonlinecontactsincludeblockingbox->show();
	contactsmenucontactsedit->show();
	contactsmenugroupsedit->show();
	contactsmenuexcludecontactsedit->show();
	contactsmenuhotkeyedit->show();
}


void GlobalHotkeys::checkPendingHotkeys()
{
	XEvent event;
	bool mod_shift;
	bool mod_control;
	bool mod_alt;
	bool mod_altgr;
	bool mod_super;
	Hotkey *hotkey;
	// (*) calling hide() and show() too fast may result in X11 error
	// (*) "X Error: BadWindow (invalid Window parameter) 3" - it seems to be a Qt bug
	// (*) to avoid warnings being reported we have to handle messages
	QtMsgHandler previousmsghandler = qInstallMsgHandler( EmptyMsgHandler );
	// process any pending hotkeys
	while( XPending( display ) > 0 )
	{
		// get the next event
		XNextEvent( display, &event );
		// is it KeyPress event?
		if( event.type == KeyPress )
		{
			// read modifiers state from the current event
			mod_shift   = ( ( event.xkey.state & GLOBALHOTKEYS_X11SHIFTMASK   ) != 0 );
			mod_control = ( ( event.xkey.state & GLOBALHOTKEYS_X11CONTROLMASK ) != 0 );
			mod_alt     = ( ( event.xkey.state & GLOBALHOTKEYS_X11ALTMASK     ) != 0 );
			mod_altgr   = ( ( event.xkey.state & GLOBALHOTKEYS_X11ALTGRMASK   ) != 0 );
			mod_super   = ( ( event.xkey.state & GLOBALHOTKEYS_X11SUPERMASK   ) != 0 );
			// create the hotkey
			hotkey = new Hotkey( mod_shift, mod_control, mod_alt, mod_altgr, mod_super, event.xkey.keycode );
			// check hotkeys and execute related commands
			if( hotkey->equals( hotkeys["ShowKadusMainWindow"] ) )
			{
				if( ! kadu->isVisible() )
				{
					// show and activate Kadu's main window
					kadu->show();
					kadu->raise();
					activateWindow( kadu->winId() );
				}
				else if( kadu->isVisible() && ( ! kadu->isActiveWindow() ) )
				{
					// hide Kadu's main window first to avoid virtual desktop switching
					kadu->hide();
					// show and activate Kadu's main window
					QTimer *showandactivatetimer = new QTimer( kadu );
					connect( showandactivatetimer, SIGNAL(timeout()), this, SLOT(showAndActivateToplevel()) );
					showandactivatetimer->start( GLOBALHOTKEYS_SHOWANDACTIVATETIMERINTERVAL, true );
				}
			}
			else if( hotkey->equals( hotkeys["HideKadusMainWindow"] ) )
			{
				if( kadu->isVisible() )
				{
					// hide Kadu's main window
					kadu->hide();
				}
			}
			else if( hotkey->equals( hotkeys["ShowHideKadusMainWindow"] ) )
			{
				if( ! kadu->isVisible() )
				{
					// show and activate Kadu's main window
					kadu->show();
					kadu->raise();
					activateWindow( kadu->winId() );
				}
				else if( kadu->isVisible() && ( ! kadu->isActiveWindow() ) )
				{
					// hide Kadu's main window first to avoid virtual desktop switching
					kadu->hide();
					// show and activate Kadu's main window
					QTimer *showandactivatetimer = new QTimer( kadu );
					connect( showandactivatetimer, SIGNAL(timeout()), this, SLOT(showAndActivateToplevel()) );
					showandactivatetimer->start( GLOBALHOTKEYS_SHOWANDACTIVATETIMERINTERVAL, true );
				}
				else if( kadu->isVisible() && ( kadu->isActiveWindow() ) )
				{
					// hide Kadu's main window
					kadu->hide();
				}
			}
			else if( hotkey->equals( hotkeys["ChangeStatus"] ) )
			{
				if( statusmenu->isVisible() )
				{
					statusmenu->close();
				}
				else
				{
					statusmenu->popup( QPoint(
							( qApp->desktop()->screenGeometry().width()  - statusmenu->sizeHint().width()  ) / 2,
							( qApp->desktop()->screenGeometry().height() - statusmenu->sizeHint().height() ) / 2
						) );
					foreach( QAction *action, statusmenu->actions() )
					{
						if( action->isChecked() )
						{
							statusmenu->setActiveAction( action );
							break;
						}
					}
					// make the statusmenu popup menu active window
					statusmenu->setActiveWindow();
					statusmenu->raise();
					activateWindow( statusmenu->winId() );
					XSetInputFocus( QX11Info::display(), statusmenu->winId(), RevertToNone, CurrentTime );
					// start inactivity checking
					statusmenuinactivitytimer->start( GLOBALHOTKEYS_STATUSMENUINACTIVITYTIMERINTERVAL );
				}
			}
			else if( hotkey->equals( hotkeys["ChangeDescription"] ) )
			{
				if( ( ! choosedescriptionwindow.isNull() ) && ( ! choosedescriptionwindow->isVisible() ) )
				{
					delete choosedescriptionwindow;
				}
				if( choosedescriptionwindow.isNull() )
				{
					choosedescriptionwindow = new ChooseDescription( kadu );
					choosedescriptionwindow->move(
							( qApp->desktop()->screenGeometry().width()  - choosedescriptionwindow->sizeHint().width()  ) / 2,
							( qApp->desktop()->screenGeometry().height() - choosedescriptionwindow->sizeHint().height() ) / 2
						);
					choosedescriptionwindow->setStatus( gadu->currentStatus() );
					((QDialog *)choosedescriptionwindow)->show();
					choosedescriptionwindow->raise();
					activateWindow( choosedescriptionwindow->winId() );
				}
				else
				{
					// close and delete the window
					choosedescriptionwindow->close( true );
				}
			}
			else if( hotkey->equals( hotkeys["OpenIncomingChatWindow"] ) )
			{
				if( pending.count() > 0 )
				{
					// open window for pending message(s)
					chat_manager->openPendingMsgs( true );
					// activate it
					QWidget *win = chat_manager->chats()[ chat_manager->chats().count() - 1 ];  // last created chat widget
					win = win->window();
					win->show();
					win->raise();
					activateWindow( win->winId() );
				}
				else
				{
					// show window with new unread message(s)
					foreach( ChatWidget *chat, chat_manager->chats() )
					{
						if( chat->newMessagesCount() > 0 )
						{
							// reopen the chat (needed when Tabs module is active)
							chat_manager->openChatWidget( gadu, chat->users()->toUserListElements() );
							// activate the window
							QWidget *win = chat;
							win = win->window();
							win->show();
							win->raise();
							activateWindow( win->winId() );
							// done - only one window
							break;
						}
					}
				}
			}
			else if( hotkey->equals( hotkeys["OpenAllIncomingChatWindows"] ) )
			{
				// open all windows for pending message(s)
				while( pending.count() > 0 )
				{
					// open the window
					chat_manager->openPendingMsgs( true );
					// activate it
					QWidget *win = chat_manager->chats()[ chat_manager->chats().count() - 1 ];  // last created chat widget
					win = win->window();
					win->show();
					win->raise();
					activateWindow( win->winId() );
				}
				// show all windows with new unread message(s)
				foreach( ChatWidget *chat, chat_manager->chats() )
				{
					if( chat->newMessagesCount() > 0 )
					{
						// reopen the chat (needed when Tabs module is active)
						chat_manager->openChatWidget( gadu, chat->users()->toUserListElements() );
						// activate the window
						QWidget *win = chat;
						win = win->window();
						win->show();
						win->raise();
						activateWindow( win->winId() );
					}
				}
			}
			else if( hotkey->equals( hotkeys["MinimizeOpenedChatWindows"] ) )
			{
				// minimize all windows (if needed)
				foreach( ChatWidget *chat, chat_manager->chats() )
				{
					QWidget *win = chat;
					win = win->window();
					if( ! win->isMinimized() )
						win->showMinimized();
				}
			}
			else if( hotkey->equals( hotkeys["RestoreMinimizedChatWindows"] ) )
			{
				// restore all windows (if needed) and activate them
				foreach( ChatWidget *chat, chat_manager->chats() )
				{
					QWidget *win = chat;
					win = win->window();
					if( win->isMinimized() )
						win->showNormal();
					// hide the window first to avoid virtual desktop switching
					win->hide();
					// show and activate the window
					QTimer *showandactivatetimer = new QTimer( win );
					connect( showandactivatetimer, SIGNAL(timeout()), this, SLOT(showAndActivateToplevel()) );
					showandactivatetimer->start( GLOBALHOTKEYS_SHOWANDACTIVATETIMERINTERVAL, true );
				}
			}
			else if( hotkey->equals( hotkeys["MinimizeRestoreChatWindows"] ) )
			{
				// check if all windows are minimized already
				bool allwindowsminimized = true;
				foreach( ChatWidget *chat, chat_manager->chats() )
				{
					QWidget *win = chat;
					win = win->window();
					if( ! win->isMinimized() )
						allwindowsminimized = false;
				}
				if( ! allwindowsminimized )
				{
					// minimize all windows (if needed)
					foreach( ChatWidget *chat, chat_manager->chats() )
					{
						QWidget *win = chat;
						win = win->window();
						if( ! win->isMinimized() )
							win->showMinimized();
					}
				}
				else
				{
					// restore all windows (if needed) and activate them
					foreach( ChatWidget *chat, chat_manager->chats() )
					{
						QWidget *win = chat;
						win = win->window();
						if( win->isMinimized() )
							win->showNormal();
						// hide the window first to avoid virtual desktop switching
						win->hide();
						// show and activate the window
						QTimer *showandactivatetimer = new QTimer( win );
						connect( showandactivatetimer, SIGNAL(timeout()), this, SLOT(showAndActivateToplevel()) );
						showandactivatetimer->start( GLOBALHOTKEYS_SHOWANDACTIVATETIMERINTERVAL, true );
					}
				}
			}
			else if( hotkey->equals( hotkeys["CloseAllChatWindows"] ) )
			{
				// list of windows to close (needed by Tabs module - we cannot close the same window multiple times!)
				QList<QWidget*> wins;
				foreach( ChatWidget *chat, chat_manager->chats() )
				{
					// add the window to the list
					QWidget *win = chat;
					win = win->window();
					if( wins.contains( win ) == 0 )  // if this window is not on the list yet
						wins.push_back( win );
				}
				// close the windows from the list
				for( QList<QWidget*>::Iterator I = wins.begin(); I != wins.end(); I++ )
				{
					(*I)->close();
				}
			}
			else if( hotkey->equals( hotkeys["OpenChatWith"] ) )
			{
				if( openchatwithwindow.isNull() )  // if the "open chat with ..." window is not opened
				{
					// open the window
					openchatwithwindow = new OpenChatWith( kadu );
					openchatwithwindow->move(
							( qApp->desktop()->screenGeometry().width()  - openchatwithwindow->width()  ) / 2,
							( qApp->desktop()->screenGeometry().height() - openchatwithwindow->height() ) / 2
						);
					openchatwithwindow->show();
					openchatwithwindow->raise();
					activateWindow( openchatwithwindow->winId() );
				}
				else
				{
					// close and delete the window
					openchatwithwindow->close( true );
				}
			}
			else if( hotkey->equals( hotkeys["TurnSilentModeOn"] ) )
			{
				kadu->silentActionActivated( NULL, true );
			}
			else if( hotkey->equals( hotkeys["TurnSilentModeOff"] ) )
			{
				kadu->silentActionActivated( NULL, false );
			}
			else if( hotkey->equals( hotkeys["ToggleSilentMode"] ) )
			{
				kadu->silentActionActivated( NULL, !kadu->silentMode() );
			}
			// <---
			else
			{
				for( QList< QPair<QStringList,QString> >::Iterator I = contactsshortcuts.begin(); I != contactsshortcuts.end(); I++ )
				{
					if( hotkey->equals( new Hotkey( (*I).second ) ) )
					{
						UserListElements users;
						for( QStringList::Iterator I2 = (*I).first.begin(); I2 != (*I).first.end(); I2++ )
						{
							users.push_back( userlist->byAltNick( (*I2) ) );
						}
						users.sort();
						if( users.count() > 0 )
						{
							chat_manager->openPendingMsgs( users );
							// for each currently open chat
							foreach( ChatWidget *chat, chat_manager->chats() )
							{
								if( chat->users()->toUserListElements() == users )
								{
									// find the window
									QWidget *win = chat;
									win = win->window();
									// hide chat window first to avoid virtual desktop switching
									win->hide();
									// show and activate chat window
									QTimer *showandactivatetimer = new QTimer( win );
									connect( showandactivatetimer, SIGNAL(timeout()), this, SLOT(showAndActivateToplevel()) );
									showandactivatetimer->start( GLOBALHOTKEYS_SHOWANDACTIVATETIMERINTERVAL, true );
									usleep( 1000*GLOBALHOTKEYS_SHOWANDACTIVATETIMERINTERVAL/10 );
								}
							}
						}
						break;
					}
				}
				for( QList< QPair<ContactsMenuItemData,QString> >::Iterator I = contactsmenusshortcuts.begin(); I != contactsmenusshortcuts.end(); I++ )
				{
					if( hotkey->equals( new Hotkey( (*I).second ) ) )
					{
						// hide previous contacts menu
						contactsmenuinactivitytimer->stop();
						if( contactsmenushown && ( (*I).second == lastcontactsmenushortcut ) )
						{
							// last menu was this one - don't show it again
							contactsmenu->hide();
							contactsmenushown = false;
							lastcontactsmenushortcut = "";
							break;
						}
						contactsmenushown = false;
						lastcontactsmenushortcut = "";
						contactsmenu->clear();
						contactsmenuusers.clear();
						contactsmenuwindows.clear();
						int n = 0;
						// add currently open chats to the popup menu
						if( (*I).first.currentchats && ( ! chat_manager->chats().isEmpty() ) )
						{
							// for each currently open chat
							foreach( ChatWidget *chat, chat_manager->chats() )
							{
								// add popup menu item
								UserListElements users = chat->users()->toUserListElements();
								QStringList altnicks = users.altNicks();
								QString chat_users;
								if( users.count() <= 5 )
								{
									chat_users = altnicks.join( ", " );
								}
								else
								{
									for( int i=0; i<4; i++ )
										chat_users.append( *altnicks.at(i) + ", " );
									chat_users.append( *altnicks.at(4) + ", ..." );
								}
								QString icon = "Current";
								if( chat->newMessagesCount() > 0 )
								{
									icon = "Pending";
								}
								if( users.count() == 1 )
								{
									eUserStatus status = users.first().status( "Gadu" ).status();
									if(      status == Online    ) { icon += "Online";    }
									else if( status == Busy      ) { icon += "Busy";      }
									else if( status == Invisible ) { icon += "Invisible"; }
									else if( status == Offline   ) { icon += "Offline";   }
									else if( status == Blocking  ) { icon += "Blocking";  }
								}
								else
								{
									icon += "Conference";
								}
								// action
								QAction *action = new QAction( contactsmenuicons[icon], chat_users, this );
								action->setData( QVariant( n ) );
								connect( action, SIGNAL(triggered()), this, SLOT(openContactsMenuChat()) );
								contactsmenu->addAction( action );
								// add users
								contactsmenuusers.push_back( users );
								// find and add the window
								QWidget *win = chat;
								win = win->window();
								contactsmenuwindows.push_back( win );
								// increase n
								n++;
							}
						}
						// add chats with pending messages to the popup menu
						if( (*I).first.pendingchats && ( pending.count() > 0 ) )
						{
							// for each pending message
							for( int k=0; k<pending.count(); k++ )
							{
								UserListElements users = (pending[k]).users;
								if( contactsmenuusers.contains( users ) )
								{
									continue;
								}
								// add popup menu item
								QStringList altnicks = users.altNicks();
								QString chat_users;
								if( users.count() <= 5 )
								{
									chat_users = altnicks.join( ", " );
								}
								else
								{
									for( int i=0; i<4; i++ )
										chat_users.append( *altnicks.at(i) + ", " );
									chat_users.append( *altnicks.at(4) + ", ..." );
								}
								QWidget *win = NULL;
								QString icon = "Pending";
								bool found = false;
								// is current ?
								foreach( ChatWidget *chat, chat_manager->chats() )
								{
									if( chat->users()->toUserListElements() == users )
									{
										// find the window
										win = chat;
										win = win->window();
										found = true;
										break;
									}
								}
								// status
								if( users.count() == 1 )
								{
									eUserStatus status = users.first().status( "Gadu" ).status();
									if(      status == Online    ) { icon += "Online";    }
									else if( status == Busy      ) { icon += "Busy";      }
									else if( status == Invisible ) { icon += "Invisible"; }
									else if( status == Offline   ) { icon += "Offline";   }
									else if( status == Blocking  ) { icon += "Blocking";  }
								}
								else
								{
									icon += "Conference";
								}
								// action
								QAction *action = new QAction( contactsmenuicons[icon], chat_users, this );
								action->setData( QVariant( n ) );
								connect( action, SIGNAL(triggered()), this, SLOT(openContactsMenuChat()) );
								contactsmenu->addAction( action );
								// add users
								contactsmenuusers.push_back( users );
								// add window, if any
								contactsmenuwindows.push_back( win );
								// increase n
								n++;
							}
						}
						// add recently closed chats to the popup menu
						if( (*I).first.recentchats && ( ! chat_manager->closedChatUsers().isEmpty() ) )
						{
							// for each recently closed chat
							foreach( UserListElements users, chat_manager->closedChatUsers() )
							{
								if( contactsmenuusers.contains( users )  )
								{
									continue;
								}
								// exclude contact ?
								if( users.count() == 1 )
								{
									bool _continue = false;
									foreach( QString contact, (*I).first.excludecontacts )
									{
										if( users.first().altNick() == contact )
										{
											_continue = true;
											break;
										}
									}
									if( _continue )
									{
										continue;
									}
								}
								// add popup menu item
								QStringList altnicks = users.altNicks();
								QString chat_users;
								if( users.count() <= 5 )
								{
									chat_users = altnicks.join( ", " );
								}
								else
								{
									for( int i=0; i<4; i++ )
										chat_users.append( *altnicks.at(i) + ", " );
									chat_users.append( *altnicks.at(4) + ", ..." );
								}
								QWidget *win = NULL;
								QString icon = "Recent";
								bool found = false;
								// is current ?
								foreach( ChatWidget *chat, chat_manager->chats() )
								{
									if( chat->users()->toUserListElements() == users )
									{
										icon = "Current";
										// find the window
										win = chat;
										win = win->window();
										found = true;
										break;
									}
								}
								// is pending ?
								for( int k=0; k<pending.count(); k++ )
								{
									if( users == (pending[k]).users )
									{
										icon = "Pending";
										found = true;
										break;
									}
								}
								// status
								if( users.count() == 1 )
								{
									eUserStatus status = users.first().status( "Gadu" ).status();
									if(      status == Online    ) { icon += "Online";    }
									else if( status == Busy      ) { icon += "Busy";      }
									else if( status == Invisible ) { icon += "Invisible"; }
									else if( status == Offline   ) { icon += "Offline";   }
									else if( status == Blocking  ) { icon += "Blocking";  }
								}
								else
								{
									icon += "Conference";
								}
								// action
								QAction *action = new QAction( contactsmenuicons[icon], chat_users, this );
								action->setData( QVariant( n ) );
								connect( action, SIGNAL(triggered()), this, SLOT(openContactsMenuChat()) );
								contactsmenu->addAction( action );
								// add users
								contactsmenuusers.push_back( users );
								// add window, if any
								contactsmenuwindows.push_back( win );
								// increase n
								n++;
							}
						}
						// add online contatcts to the popup menu
						if( (*I).first.onlinecontacts )
						{
							// online & busy
							foreach( UserListElement user, onlineUsers->toUserListElements() )
							{
								UserListElements users = UserListElements( user );
								if( contactsmenuusers.contains( users ) )
								{
									continue;
								}
								// exclude self
								if( user.ID( "Gadu" ) == QString::number( config_file.readNumEntry( "General", "UIN" ) ) )
								{
									continue;
								}
								// exclude contact ?
								bool _continue = false;
								foreach( QString contact, (*I).first.excludecontacts )
								{
									if( user.altNick() == contact )
									{
										_continue = true;
										break;
									}
								}
								if( _continue )
								{
									continue;
								}
								// go on
								if( (*I).first.onlinecontactsgroups.count() > 0 )
								{
									bool found = false;
									foreach( QString groupname, (*I).first.onlinecontactsgroups )
									{
										if( ( groups_manager->group( groupname ) != NULL ) && user.containsGroup( groups_manager->group( groupname ) ) )
										{
											found = true;
											break;
										}
									}
									if( ! found )
									{
										continue;
									}
								}
								eUserStatus status = users.first().status( "Gadu" ).status();
								if( ( status != Online ) && ( status != Busy ) )
								{
									continue;
								}
								QWidget *win = NULL;
								QString icon = "None";
								bool found = false;
								// is current ?
								foreach( ChatWidget *chat, chat_manager->chats() )
								{
									if( chat->users()->toUserListElements() == users )
									{
										icon = "Current";
										// find the window
										win = chat;
										win = win->window();
										found = true;
										break;
									}
								}
								// is pending ?
								for( int k=0; k<pending.count(); k++ )
								{
									if( users == (pending[k]).users )
									{
										icon = "Pending";
										found = true;
										break;
									}
								}
								// is recent ?
								if( ! found )
								{
									foreach( UserListElements users2, chat_manager->closedChatUsers() )
									{
										if( users == users2 )
										{
											icon = "Recent";
											found = true;
											break;
										}
									}
								}
								// status
								if(      status == Online    ) { icon += "Online";    }
								else if( status == Busy      ) { icon += "Busy";      }
								// action
								QAction *action = new QAction( contactsmenuicons[icon], user.altNick(), this );
								action->setData( QVariant( n ) );
								connect( action, SIGNAL(triggered()), this, SLOT(openContactsMenuChat()) );
								contactsmenu->addAction( action );
								// add users
								contactsmenuusers.push_back( users );
								// add window, if any
								contactsmenuwindows.push_back( win );
								// increase n
								n++;
							}
							// invisible
							foreach( UserListElement user, onlineUsers->toUserListElements() )
							{
								UserListElements users = UserListElements( user );
								if( contactsmenuusers.contains( users ) )
								{
									continue;
								}
								// exclude self
								if( user.ID( "Gadu" ) == QString::number( config_file.readNumEntry( "General", "UIN" ) ) )
								{
									continue;
								}
								// exclude contact ?
								bool _continue = false;
								foreach( QString contact, (*I).first.excludecontacts )
								{
									if( user.altNick() == contact )
									{
										_continue = true;
										break;
									}
								}
								if( _continue )
								{
									continue;
								}
								// go on
								if( (*I).first.onlinecontactsgroups.count() > 0 )
								{
									bool found = false;
									foreach( QString groupname, (*I).first.onlinecontactsgroups )
									{
										if( ( groups_manager->group( groupname ) != NULL ) && user.containsGroup( groups_manager->group( groupname ) ) )
										{
											found = true;
											break;
										}
									}
									if( ! found )
									{
										continue;
									}
								}
								eUserStatus status = users.first().status( "Gadu" ).status();
								if( status != Invisible )
								{
									continue;
								}
								QWidget *win = NULL;
								QString icon = "None";
								bool found = false;
								// is current ?
								foreach( ChatWidget *chat, chat_manager->chats() )
								{
									if( chat->users()->toUserListElements() == users )
									{
										icon = "Current";
										// find the window
										win = chat;
										win = win->window();
										found = true;
										break;
									}
								}
								// is pending ?
								for( int k=0; k<pending.count(); k++ )
								{
									if( users == (pending[k]).users )
									{
										icon = "Pending";
										found = true;
										break;
									}
								}
								// is recent ?
								if( ! found )
								{
									foreach( UserListElements users2, chat_manager->closedChatUsers() )
									{
										if( users == users2 )
										{
											icon = "Recent";
											found = true;
											break;
										}
									}
								}
								// status
								icon += "Invisible";
								// action
								QAction *action = new QAction( contactsmenuicons[icon], user.altNick(), this );
								action->setData( QVariant( n ) );
								connect( action, SIGNAL(triggered()), this, SLOT(openContactsMenuChat()) );
								contactsmenu->addAction( action );
								// add users
								contactsmenuusers.push_back( users );
								// add window, if any
								contactsmenuwindows.push_back( win );
								// increase n
								n++;
							}
							// blocking
							if( (*I).first.onlinecontactsincludeblocking )
							{
								foreach( UserListElement user, blockingUsers->toUserListElements() )
								{
									UserListElements users = UserListElements( user );
									if( contactsmenuusers.contains( users ) )
									{
										continue;
									}
									// exclude self
									if( user.ID( "Gadu" ) == QString::number( config_file.readNumEntry( "General", "UIN" ) ) )
									{
										continue;
									}
									// exclude contact ?
									bool _continue = false;
									foreach( QString contact, (*I).first.excludecontacts )
									{
										if( user.altNick() == contact )
										{
											_continue = true;
											break;
										}
									}
									if( _continue )
									{
										continue;
									}
									// go on
									if( (*I).first.onlinecontactsgroups.count() > 0 )
									{
										bool found = false;
										foreach( QString groupname, (*I).first.onlinecontactsgroups )
										{
											if( ( groups_manager->group( groupname ) != NULL ) && user.containsGroup( groups_manager->group( groupname ) ) )
											{
												found = true;
												break;
											}
										}
										if( ! found )
										{
											continue;
										}
									}
									QWidget *win = NULL;
									QString icon = "None";
									bool found = false;
									// is current ?
									foreach( ChatWidget *chat, chat_manager->chats() )
									{
										if( chat->users()->toUserListElements() == users )
										{
											icon = "Current";
											// find the window
											win = chat;
											win = win->window();
											found = true;
											break;
										}
									}
									// is pending ?
									for( int k=0; k<pending.count(); k++ )
									{
										if( users == (pending[k]).users )
										{
											icon = "Pending";
											found = true;
											break;
										}
									}
									// is recent ?
									if( ! found )
									{
										foreach( UserListElements users2, chat_manager->closedChatUsers() )
										{
											if( users == users2 )
											{
												icon = "Recent";
												found = true;
												break;
											}
										}
									}
									// status
									icon += "Blocking";
									// action
									QAction *action = new QAction( contactsmenuicons[icon], user.altNick(), this );
									action->setData( QVariant( n ) );
									connect( action, SIGNAL(triggered()), this, SLOT(openContactsMenuChat()) );
									contactsmenu->addAction( action );
									// add users
									contactsmenuusers.push_back( users );
									// add window, if any
									contactsmenuwindows.push_back( win );
									// increase n
									n++;
								}
							}
						}
						// add given contatcts to the popup menu
						if( ! (*I).first.contacts.isEmpty() )
						{
							foreach( QString contact, (*I).first.contacts )
							{
								// add popup menu item
								if( ! userlist->containsAltNick( contact, FalseForAnonymous ) )
								{
									continue;
								}
								UserListElements users = UserListElements( userlist->byAltNick( contact ) );
								if( contactsmenuusers.contains( users ) )
								{
									continue;
								}
								// exclude self
								if( users.first().ID( "Gadu" ) == QString::number( config_file.readNumEntry( "General", "UIN" ) ) )
								{
									continue;
								}
								// go on
								QWidget *win = NULL;
								QString icon = "None";
								bool found = false;
								// is current ?
								foreach( ChatWidget *chat, chat_manager->chats() )
								{
									if( chat->users()->toUserListElements() == users )
									{
										icon = "Current";
										// find the window
										win = chat;
										win = win->window();
										found = true;
										break;
									}
								}
							// is pending ?
								for( int k=0; k<pending.count(); k++ )
								{
									if( users == (pending[k]).users )
									{
										icon = "Pending";
										found = true;
										break;
									}
								}
								// is recent ?
								if( ! found )
								{
									foreach( UserListElements users2, chat_manager->closedChatUsers() )
									{
										if( users == users2 )
										{
											icon = "Recent";
											found = true;
											break;
										}
									}
								}
								// status
								eUserStatus status = users.first().status( "Gadu" ).status();
								if(      status == Online    ) { icon += "Online";    }
								else if( status == Busy      ) { icon += "Busy";      }
								else if( status == Invisible ) { icon += "Invisible"; }
								else if( status == Offline   ) { icon += "Offline";   }
								else if( status == Blocking  ) { icon += "Blocking";  }
								// action
								QAction *action = new QAction( contactsmenuicons[icon], contact, this );
								action->setData( QVariant( n ) );
								connect( action, SIGNAL(triggered()), this, SLOT(openContactsMenuChat()) );
								contactsmenu->addAction( action );
								// add users
								contactsmenuusers.push_back( users );
								// add window, if any
								contactsmenuwindows.push_back( win );
								// increase n
								n++;
							}
						}
						// add contacts from given groups to the popup menu
						if( ! (*I).first.groups.isEmpty() )
						{
							foreach( QString groupname, (*I).first.groups )
							{
								if( groups_manager->group( groupname ) != NULL )
								{
									UserListElements groupusers = groups_manager->group( groupname )->toUserListElements();
									foreach( UserListElement user, groupusers )
									{
										UserListElements users = UserListElements( user );
										if( contactsmenuusers.contains( users ) )
										{
											continue;
										}
										// exclude self
										if( user.ID( "Gadu" ) == QString::number( config_file.readNumEntry( "General", "UIN" ) ) )
										{
											continue;
										}
										// exclude contact ?
										bool _continue = false;
										foreach( QString contact, (*I).first.excludecontacts )
										{
											if( user.altNick() == contact )
											{
												_continue = true;
												break;
											}
										}
										if( _continue )
										{
											continue;
										}
										// go on
										QWidget *win = NULL;
										QString icon = "None";
										bool found = false;
										// is current ?
										foreach( ChatWidget *chat, chat_manager->chats() )
										{
											if( chat->users()->toUserListElements() == users )
											{
												icon = "Current";
												// find the window
												win = chat;
												win = win->window();
												found = true;
												break;
											}
										}
										// is pending ?
										for( int k=0; k<pending.count(); k++ )
										{
											if( users == (pending[k]).users )
											{
												icon = "Pending";
												found = true;
												break;
											}
										}
										// is recent ?
										if( ! found )
										{
											foreach( UserListElements users2, chat_manager->closedChatUsers() )
											{
												if( users == users2 )
												{
													icon = "Recent";
													found = true;
													break;
												}
											}
										}
										// status
										eUserStatus status = users.first().status( "Gadu" ).status();
										if(      status == Online    ) { icon += "Online";    }
										else if( status == Busy      ) { icon += "Busy";      }
										else if( status == Invisible ) { icon += "Invisible"; }
										else if( status == Offline   ) { icon += "Offline";   }
										else if( status == Blocking  ) { icon += "Blocking";  }
										// action
										QAction *action = new QAction( contactsmenuicons[icon], user.altNick(), this );
										action->setData( QVariant( n ) );
										connect( action, SIGNAL(triggered()), this, SLOT(openContactsMenuChat()) );
										contactsmenu->addAction( action );
										// add users
										contactsmenuusers.push_back( users );
										// add window, if any
										contactsmenuwindows.push_back( win );
										// increase n
										n++;
									}
								}
							}
						}
						// <---
						if( n == 0 )
						{
							// insert disabled item saying there are no contacts to display
							QAction *action = new QAction( contactsmenuicons["empty"], qApp->translate( "@default", "no contacts to display" ), this );
							action->setEnabled( false );
							contactsmenu->addAction( action );
						}
						lastcontactsmenushortcut = (*I).second;
						// popup in the center of the screen
						contactsmenu->popup( QPoint(
								( qApp->desktop()->screenGeometry().width()  - contactsmenu->sizeHint().width()  ) / 2,
								( qApp->desktop()->screenGeometry().height() - contactsmenu->sizeHint().height() ) / 2
							) );
						// activate first item
						if( n > 0 )
						{
							contactsmenu->setActiveItem( 0 );
						}
						// make the contactsmenu popup menu active window
						contactsmenu->setActiveWindow();
						contactsmenu->raise();
						activateWindow( contactsmenu->winId() );
						XSetInputFocus( QX11Info::display(), contactsmenu->winId(), RevertToNone, CurrentTime );
						// start inactivity checking
						contactsmenuinactivitytimer->start( GLOBALHOTKEYS_CONTACTSMENUINACTIVITYTIMERINTERVAL );
						break;
					}
				}
			}
			// destroy the hotkey
			delete hotkey;
		}
	}
	// (*) restore previous messages handler
	qInstallMsgHandler( previousmsghandler );
}


void GlobalHotkeys::showAndActivateToplevel()
{
	QObject *sender = (QObject*)(QObject::sender());  // QTimer
	QWidget *toplevel = (QWidget*)(sender->parent());  // QTimer's parent which should be set to a required Toplevel
	// show and activate the toplevel
	toplevel->show();
	qApp->processEvents();
	usleep( 1000*GLOBALHOTKEYS_SHOWANDACTIVATETIMERWAITTIME );
	qApp->processEvents();
	toplevel->raise();
	toplevel->activateWindow();
	activateWindow( toplevel->winId() );
}


void GlobalHotkeys::openContactsMenuChat()
{
	int n = ((QAction*)sender())->data().toInt();
	// hide the popup menu
	contactsmenuinactivitytimer->stop();
	contactsmenu->hide();
	contactsmenushown = false;
	// (re)open the chat with selected user(s)
	chat_manager->openPendingMsgs( contactsmenuusers[n] );
	if( n < contactsmenuwindows.size() )  // if it was currently open chat, activate the window
	{
		// check if the chat window is still opened
		if( ! chat_manager->chats().isEmpty() )
		{
			// for each currently open chat
			foreach( ChatWidget *chat, chat_manager->chats() )
			{
				// find the window
				QWidget *win = chat;
				win = win->window();
				if( win == contactsmenuwindows[n] )
				{
					// hide chat window first to avoid virtual desktop switching
					win->hide();
					// show and activate chat window
					QTimer *showandactivatetimer = new QTimer( win );
					connect( showandactivatetimer, SIGNAL(timeout()), this, SLOT(showAndActivateToplevel()) );
					showandactivatetimer->start( GLOBALHOTKEYS_SHOWANDACTIVATETIMERINTERVAL, true );
				}
			}
		}
	}
}


void GlobalHotkeys::statusmenuinactivitytimerTimeout()
{
	Window activewindow; int revertto;
	XGetInputFocus( QX11Info::display(), &activewindow, &revertto );
	if( ! statusmenushown )
	{
		if( activewindow == statusmenu->winId() )
		{
			statusmenushown = true;
		}
		else if( statusmenu->isVisible() )
		{
			// hide the statusmenu popup menu if it is inactive
			statusmenuinactivitytimer->stop();
			statusmenu->hide();
			statusmenushown = false;
		}
	}
	else
	{
		if( ( activewindow != statusmenu->winId() ) )
		{
			if( statusmenu->isVisible() )
			{
				// hide the statusmenu popup menu if it is inactive
				statusmenuinactivitytimer->stop();
				statusmenu->hide();
			}
			statusmenushown = false;
		}
	}
}


void GlobalHotkeys::contactsmenuinactivitytimerTimeout()
{
	Window activewindow; int revertto;
	XGetInputFocus( QX11Info::display(), &activewindow, &revertto );
	if( ! contactsmenushown )
	{
		if( activewindow == contactsmenu->winId() )
		{
			contactsmenushown = true;
		}
		else if( contactsmenu->isVisible() )
		{
			// hide the contactsmenu popup menu if it is inactive
			contactsmenuinactivitytimer->stop();
			contactsmenu->hide();
			contactsmenushown = false;
		}
	}
	else
	{
		if( ( activewindow != contactsmenu->winId() ) )
		{
			if( contactsmenu->isVisible() )
			{
				// hide the contactsmenu popup menu if it is inactive
				contactsmenuinactivitytimer->stop();
				contactsmenu->hide();
			}
			contactsmenushown = false;
		}
	}
}




void EmptyMsgHandler( QtMsgType type, const char *msg )
{
	messageshandled = 1;
}
