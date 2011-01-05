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


#ifndef CONF_H
	#define CONF_H


#include <QList>
#include <QObject>
#include <QPointer>

#include "gui/widgets/configuration/config-action-button.h"
#include "gui/widgets/configuration/config-check-box.h"
#include "gui/widgets/configuration/config-line-edit.h"
#include "gui/widgets/configuration/config-line-separator.h"
#include "gui/windows/main-configuration-window.h"

#include "hotkey.h"
#include "hotkeyedit.h"

#include "property.h"

#include "defines.h"


class ConfHelper
{
	public:
		static QStringList buddies( QString string );
		static QStringList groups( QString string );
};


class ConfGroups
{
	friend class ConfHotKey;
	friend class ConfManager;
	friend class ConfBuddiesShortcut;
	friend class ConfBuddiesMenu;
	public:
		static void createGroups();
		static void deleteGroups();
	private:
		static QStringList GROUPS;
};


class ConfHotKey : public QObject
{
	Q_OBJECT
	friend class GlobalHotkeys;
	public:
		ConfHotKey( QObject *parent, QString group, QString caption, QString name, QString function, QString defaulthotkey = "", bool forcecreate = false );
		~ConfHotKey();
		static QList<ConfHotKey*> instances();
		static ConfHotKey *findByName( QString name );
		static ConfHotKey *findByHotkey( HotKey hotkey );
		void focus();
		PROPERTY_RO( QString, GROUP   , group    );
		PROPERTY_RO( QString, CAPTION , caption  );
		PROPERTY_RO( QString, NAME    , name     );
		PROPERTY_RO( HotKey , HOTKEY  , hotKey   );
		PROPERTY_RO( QString, FUNCTION, function );
	public slots:
		void configurationSaved();
		void mainConfigurationWindowCreated( MainConfigurationWindow *mainConfigurationWindow );
	private:
		static QList<ConfHotKey*> INSTANCES;
		QPointer<HotkeyEdit> HOTKEYEDIT;
};


class ConfManager : public QObject
{
	Q_OBJECT
	public:
		ConfManager( QObject *parent = 0 );
		~ConfManager();
	public slots:
		void mainConfigurationWindowCreated( MainConfigurationWindow *mainConfigurationWindow );
		void configurationWindowApplied();
};


class ConfBuddiesShortcut : public QObject
{
	Q_OBJECT
	friend class GlobalHotkeys;
	public:
		static QList<ConfBuddiesShortcut*> instances();
		static ConfBuddiesShortcut *findByHotkey( HotKey hotkey );
		ConfBuddiesShortcut( QObject *parent, QString group, bool forcecreate = false );
		~ConfBuddiesShortcut();
		void commitUIData();
		void fillUIData();
		QString serialized();
		void deserialize( QString serializedstring );
		void focus();
		PROPERTY_RO( QString    , GROUP   , group     );
		PROPERTY_RO( bool       , DELETED , isDeleted );
		PROPERTY_RO( HotKey     , HOTKEY  , hotKey    );
		PROPERTY_RO( QStringList, BUDDIES , buddies   );
		PROPERTY_RO( bool       , SHOWMENU, showMenu  );
	public slots:
		void mainConfigurationWindowCreated( MainConfigurationWindow *mainConfigurationWindow );
		void deletebuttonClicked();
	private:
		static QList<ConfBuddiesShortcut*> INSTANCES;
		QPointer<ConfigLineSeparator> LINESEPARATOR;
		QPointer<HotkeyEdit>          HOTKEYEDIT;
		QPointer<ConfigLineEdit>      BUDDIESEDIT;
		QPointer<ConfigCheckBox>      SHOWMENUCHECKBOX;
		QPointer<ConfigActionButton>  DELETEBUTTON;
};


class ConfBuddiesMenu : public QObject
{
	Q_OBJECT
	friend class GlobalHotkeys;
	public:
		static QList<ConfBuddiesMenu*> instances();
		static ConfBuddiesMenu *findByHotkey( HotKey hotkey );
		ConfBuddiesMenu( QObject *parent, QString group, bool forcecreate = false );
		~ConfBuddiesMenu();
		void commitUIData();
		void fillUIData();
		QString serialized();
		void deserialize( QString serializedstring );
		void focus();
		PROPERTY_RO( QString    , GROUP                       , group                        );
		PROPERTY_RO( bool       , DELETED                     , isDeleted                    );
		PROPERTY_RO( HotKey     , HOTKEY                      , hotKey                       );
		PROPERTY_RO( bool       , CURRENTCHATS                , currentChats                 );
		PROPERTY_RO( bool       , PENDINGCHATS                , pendingChats                 );
		PROPERTY_RO( bool       , RECENTCHATS                 , recentChats                  );
		PROPERTY_RO( bool       , ONLINEBUDDIES               , onlineBuddies                );
		PROPERTY_RO( QStringList, ONLINEBUDDIESGROUPS         , onlineBuddiesGroups          );
		PROPERTY_RO( bool       , ONLINEBUDDIESINCLUDEBLOCKING, onlineBuddiesIncludeBlocking );
		PROPERTY_RO( QStringList, BUDDIES                     , buddies                      );
		PROPERTY_RO( QStringList, GROUPS                      , groups                       );
		PROPERTY_RO( QStringList, EXCLUDEBUDDIES              , excludeBuddies               );
	public slots:
		void mainConfigurationWindowCreated( MainConfigurationWindow *mainConfigurationWindow );
		void deletebuttonClicked();
	private:
		static QList<ConfBuddiesMenu*> INSTANCES;
		QPointer<ConfigLineSeparator> LINESEPARATOR;
		QPointer<HotkeyEdit>          HOTKEYEDIT;
		QPointer<ConfigCheckBox>      CURRENTCHATSCHECKBOX;
		QPointer<ConfigCheckBox>      PENDINGCHATSCHECKBOX;
		QPointer<ConfigCheckBox>      RECENTCHATSCHECKBOX;
		QPointer<ConfigCheckBox>      ONLINEBUDDIESCHECKBOX;
		QPointer<ConfigLineEdit>      ONLINEBUDDIESGROUPSEDIT;
		QPointer<ConfigCheckBox>      ONLINEBUDDIESINCLUDEBLOCKINGCHECKBOX;
		QPointer<ConfigLineEdit>      BUDDIESEDIT;
		QPointer<ConfigLineEdit>      GROUPSEDIT;
		QPointer<ConfigLineEdit>      EXCLUDEBUDDIESEDIT;
		QPointer<ConfigActionButton>  DELETEBUTTON;
};


#endif
