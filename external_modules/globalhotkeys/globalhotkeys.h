/***************************************************************************
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 3 of the License, or      *
*   (at your option) any later version.                                    *
*                                                                          *
***************************************************************************/


#ifndef GLOBALHOTKEYS_H
	#define GLOBALHOTKEYS_H


#include <QPointer>
#include <QMap>
#include <QList>
#include <QString>
#include <QTimer>

#include "configuration_aware_object.h"
#include "configuration_window_widgets.h"
#include "main_configuration_window.h"

#include "hotkey.h"
#include "hotkeyedit.h"

#include <X11/Xlib.h>
#include <X11/keysym.h>

#include "defines.h"


static int messageshandled;
void EmptyMsgHandler( QtMsgType type, const char *msg );


struct ContactsMenuItemData
{
	bool currentchats;
	bool pendingchats;
	bool recentchats;
	bool onlinecontacts;
	QStringList onlinecontactsgroups;
	bool onlinecontactsincludeblocking;
	QStringList contacts;
	QStringList groups;
	QStringList excludecontacts;
};


class GlobalHotkeys : public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT
	public:
		GlobalHotkeys();
		~GlobalHotkeys();
		void mainConfigurationWindowCreated( MainConfigurationWindow *mainConfigurationWindow );
	protected:
		void configurationUpdated();
	private slots:
		void contactsAddNewButtonPressed();
		void contactsmenusAddNewButtonPressed();
		void checkPendingHotkeys();
		void showAndActivateToplevel();
		void openContactsMenuChat();
		void statusmenuinactivitytimerTimeout();
		void contactsmenuinactivitytimerTimeout();
	private:
		void createDefaultConfiguration();
		void grabHotkeys( QMap<QString,Hotkey*> hotkeys );
		QIcon contactsMenuIcon( QIcon chaticon, QIcon statusicon );
		void createContactsMenuIcons();
		QTimer *hotkeysTimer;
		Display *display;
		QMap<QString,Hotkey*> hotkeys;
		QPointer<OpenChatWith> openchatwithwindow;
		QPointer<ChooseDescription> choosedescriptionwindow;
		QPointer<QMenu> statusmenu;
		QMenu *contactsmenu;
		QMap<QString,QIcon> contactsmenuicons;
		QList<UserListElements> contactsmenuusers;
		QList<QWidget*> contactsmenuwindows;
		QTimer *statusmenuinactivitytimer;
		QTimer *contactsmenuinactivitytimer;
		bool statusmenushown;
		bool contactsmenushown;
		QString lastcontactsmenushortcut;
		ConfigGroupBox *groupKadu, *groupChats, *groupContacts, *groupContactsMenus;
		QPointer<HotkeyEdit> HEshowKadusMainWindow, HEhideKadusMainWindow, HEshowHideKadusMainWindow,
			HEchangeStatus, HEchangeDescription, HEturnSilentModeOn, HEturnSilentModeOff, HEtoggleSilentMode,
			HEopenIncomingChatWindow, HEopenAllIncomingChatWindows, HEminimizeOpenedChatWindows,
			HErestoreMinimizedChatWindows, HEminimizeRestoreChatWindows, HEcloseAllChatWindows,
			HEopenChatWith;  // <---
		QString showKadusMainWindow;           // configuration value
		QString hideKadusMainWindow;           // configuration value
		QString showHideKadusMainWindow;       // configuration value
		QString changeStatus;                  // configuration value
		QString changeDescription;             // configuration value
		QString turnSilentModeOn;              // configuration value
		QString turnSilentModeOff;             // configuration value
		QString toggleSilentMode;              // configuration value
		QString openIncomingChatWindow;        // configuration value
		QString openAllIncomingChatWindows;    // configuration value
		QString minimizeOpenedChatWindows;     // configuration value
		QString restoreMinimizedChatWindows;   // configuration value
		QString minimizeRestoreChatWindows;    // configuration value
		QString closeAllChatWindows;           // configuration value
		QString openChatWith;                  // configuration value
		// <---
		QPointer<ConfigActionButton>        contactsAddNewButton;
		QList< QPointer<ConfigLineEdit> >   contactsNamesEditList;
		QList< QPointer<HotkeyEdit> >       contactsHotkeyEditList;
		QList< QPair<QStringList,QString> > contactsshortcuts;
		QPointer<ConfigActionButton>                 contactsmenusAddNewButton;
		QList< QPointer<HotkeyEdit> >                contactsmenusHotkeyEditList;
		QList< QPointer<ConfigCheckBox> >            contactsmenusCurrentChatsCheckboxList;
		QList< QPointer<ConfigCheckBox> >            contactsmenusPendingChatsCheckboxList;
		QList< QPointer<ConfigCheckBox> >            contactsmenusRecentChatsCheckboxList;
		QList< QPointer<ConfigCheckBox> >            contactsmenusOnlineContactsCheckboxList;
		QList< QPointer<ConfigLineEdit> >            contactsmenusOnlineContactsGroupsEditList;
		QList< QPointer<ConfigCheckBox> >            contactsmenusOnlineContactsIncludeBlockingCheckboxList;
		QList< QPointer<ConfigLineEdit> >            contactsmenusContactsEditList;
		QList< QPointer<ConfigLineEdit> >            contactsmenusGroupsEditList;
		QList< QPointer<ConfigLineEdit> >            contactsmenusExcludeContactsEditList;
		QList< QPair<ContactsMenuItemData,QString> > contactsmenusshortcuts;
		// <---
};


extern GlobalHotkeys *globalHotkeys;


#endif
