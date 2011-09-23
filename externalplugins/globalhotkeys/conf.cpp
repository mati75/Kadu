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




#include <QScrollArea>

#include "buddies/buddy-manager.h"
#include "buddies/group-manager.h"
#include "configuration/configuration-file.h"
#include "gui/widgets/configuration/configuration-widget.h"

#include "conf.h"

#include "globalhotkeys.h"
#include "serializableqstringlist.h"




QStringList ConfHelper::buddies( QString string )
{
	QStringList result;
	QStringList buddies = string.split( QRegExp( GLOBALHOTKEYS_COMMAREGEXP ), QString::SkipEmptyParts );
	foreach( QString buddy, buddies )
	{
		if( ! BuddyManager::instance()->byDisplay( buddy, ActionReturnNull ).isNull() )
			result.append( buddy );
	}
	return result;
}


QStringList ConfHelper::groups( QString string )
{
	QStringList result;
	QStringList groups = string.split( QRegExp( GLOBALHOTKEYS_COMMAREGEXP ), QString::SkipEmptyParts );
	foreach( QString group, groups )
	{
		if( ! GroupManager::instance()->byName( group, ActionReturnNull ).isNull() )
			result.append( group );
	}
	return result;
}




void ConfGroups::createGroups()
{
	foreach( QString group, GROUPS )
	{
		MainConfigurationWindow::instance()->widget()->configGroupBox(
			QT_TRANSLATE_NOOP( "@default", "Shortcuts" ),
			QT_TRANSLATE_NOOP( "@default", "Global hotkeys" ),
			group,
			true
		);
	}
}


void ConfGroups::deleteGroups()
{
	if( MainConfigurationWindow::hasInstance() )
	{
		foreach( QString group, GROUPS )
		{
			ConfigGroupBox *configgroupbox = MainConfigurationWindow::instance()->widget()->configGroupBox(
				QT_TRANSLATE_NOOP( "@default", "Shortcuts" ),
				QT_TRANSLATE_NOOP( "@default", "Global hotkeys" ),
				group,
				false
			);
			if( configgroupbox != NULL )
				delete configgroupbox;
		}
	}
}


QStringList ConfGroups::GROUPS;




ConfHotKey::ConfHotKey( QObject *parent, QString group, QString caption, QString name, QString function, QString defaulthotkey, bool forcecreate ) : QObject( parent )
{
	INSTANCES.append( this );
	GROUP    = group;
	CAPTION  = caption;
	NAME     = name;
	FUNCTION = function;
	if( ! ConfGroups::GROUPS.contains( GROUP ) )
		ConfGroups::GROUPS.append( GROUP );
	if( config_file.readEntry( "GlobalHotkeys", name, " " ) == " " )
		config_file.addVariable( "GlobalHotkeys", name, defaulthotkey );
	configurationSaved();
	connect( GlobalHotkeys::instance(), SIGNAL(mainConfigurationWindowCreatedSignal(MainConfigurationWindow*)), this, SLOT(mainConfigurationWindowCreated(MainConfigurationWindow*)) );
	if( forcecreate && MainConfigurationWindow::hasInstance() )
		mainConfigurationWindowCreated( MainConfigurationWindow::instance() );
}


ConfHotKey::~ConfHotKey()
{
	INSTANCES.removeOne( this );
	if( ! HOTKEYEDIT.isNull() ) delete HOTKEYEDIT;
}


QList<ConfHotKey*> ConfHotKey::INSTANCES;


QList<ConfHotKey*> ConfHotKey::instances()
{
	return INSTANCES;
}


ConfHotKey *ConfHotKey::findByName( QString name )
{
	foreach( ConfHotKey* confhotkey, INSTANCES )
	{
		if( confhotkey->name() == name )
			return confhotkey;
	}
	return NULL;
}


ConfHotKey *ConfHotKey::findByHotkey( HotKey hotkey )
{
	foreach( ConfHotKey* confhotkey, INSTANCES )
	{
		if( confhotkey->hotKey() == hotkey )
			return confhotkey;
	}
	return NULL;
}


void ConfHotKey::focus()
{
	QScrollArea *scrollarea = NULL;
	QObject *object = HOTKEYEDIT;
	while( object != NULL )
	{
		scrollarea = dynamic_cast<QScrollArea*>( object );
		if( scrollarea != NULL )
			break;
		object = object->parent();
	}
	if( scrollarea != NULL )
	{
		scrollarea->widget()->adjustSize();
		scrollarea->ensureWidgetVisible( HOTKEYEDIT, 50, 50 );
	}
	HOTKEYEDIT->setFocus( Qt::OtherFocusReason );
}


void ConfHotKey::configurationSaved()
{
	HOTKEY = HotKey( config_file.readEntry( "GlobalHotkeys", NAME ) );
}


void ConfHotKey::mainConfigurationWindowCreated( MainConfigurationWindow *mainConfigurationWindow )
{
	if( ! HOTKEYEDIT.isNull() )
		return;
	ConfigGroupBox *configgroupbox = mainConfigurationWindow->widget()->configGroupBox(
		QT_TRANSLATE_NOOP( "@default", "Shortcuts" ),
		QT_TRANSLATE_NOOP( "@default", "Global hotkeys" ),
		GROUP,
		true
	);
	HOTKEYEDIT = new HotkeyEdit( "GlobalHotkeys", NAME, CAPTION, "", configgroupbox, (ConfigurationWindowDataManager*)MainConfigurationWindow::instanceDataManager() );
	HOTKEYEDIT->setText( HOTKEY.string() );
	connect( mainConfigurationWindow, SIGNAL(configurationSaved()), this, SLOT(configurationSaved()) );
}




ConfManager::ConfManager( QObject *parent ) : QObject( parent )
{
	// load BuddiesShortcuts
	if( ! ConfGroups::GROUPS.contains( QT_TRANSLATE_NOOP( "@default", "Buddies shortcuts" ) ) )
		ConfGroups::GROUPS.append( QT_TRANSLATE_NOOP( "@default", "Buddies shortcuts" ) );
	{
		SerializableQStringList list;
		list.deserialize( config_file.readEntry( "GlobalHotkeys", "BuddiesShortcuts" ) );
		foreach( QString string, list )
		{
			if( ! string.isEmpty() )
			{
				ConfBuddiesShortcut *confbuddiesshortcut = new ConfBuddiesShortcut( this, QT_TRANSLATE_NOOP( "@default", "Buddies shortcuts" ) );
				confbuddiesshortcut->deserialize( string );
			}
		}
	}
	// load BuddiesMenus
	if( ! ConfGroups::GROUPS.contains( QT_TRANSLATE_NOOP( "@default", "Buddies menus" ) ) )
		ConfGroups::GROUPS.append( QT_TRANSLATE_NOOP( "@default", "Buddies menus" ) );
	{
		SerializableQStringList list;
		list.deserialize( config_file.readEntry( "GlobalHotkeys", "BuddiesMenus" ) );
		foreach( QString string, list )
		{
			if( ! string.isEmpty() )
			{
				ConfBuddiesMenu *confbuddiesmenu = new ConfBuddiesMenu( this, QT_TRANSLATE_NOOP( "@default", "Buddies menus" ) );
				confbuddiesmenu->deserialize( string );
			}
		}
	}
	// connect GlobalHotkeys::mainConfigurationWindowCreated signal
	connect( GlobalHotkeys::instance(), SIGNAL(mainConfigurationWindowCreatedSignal(MainConfigurationWindow*)), this, SLOT(mainConfigurationWindowCreated(MainConfigurationWindow*)) );
}


ConfManager::~ConfManager()
{
}


void ConfManager::mainConfigurationWindowCreated( MainConfigurationWindow *mainConfigurationWindow )
{
	connect( mainConfigurationWindow, SIGNAL(configurationWindowApplied()), this, SLOT(configurationWindowApplied()) );
}


void ConfManager::configurationWindowApplied()
{
	// process BuddiesShortcuts
	{
		foreach( ConfBuddiesShortcut *confbuddiesshortcut, ConfBuddiesShortcut::instances() )
		{
			confbuddiesshortcut->commitUIData();
			if( confbuddiesshortcut->isDeleted() )
				delete confbuddiesshortcut;
		}
		SerializableQStringList list;
		foreach( ConfBuddiesShortcut *confbuddiesshortcut, ConfBuddiesShortcut::instances() )
			list.append( confbuddiesshortcut->serialized() );
		config_file.writeEntry( "GlobalHotkeys", "BuddiesShortcuts", list.serialized() );
	}
	// process BuddiesMenus
	{
		foreach( ConfBuddiesMenu *confbuddiesmenu, ConfBuddiesMenu::instances() )
		{
			confbuddiesmenu->commitUIData();
			if( confbuddiesmenu->isDeleted() )
				delete confbuddiesmenu;
		}
		SerializableQStringList list;
		foreach( ConfBuddiesMenu *confbuddiesmenu, ConfBuddiesMenu::instances() )
			list.append( confbuddiesmenu->serialized() );
		config_file.writeEntry( "GlobalHotkeys", "BuddiesMenus", list.serialized() );
	}
}




ConfBuddiesShortcut::ConfBuddiesShortcut( QObject *parent, QString group, bool forcecreate ) : QObject( parent )
{
	INSTANCES.append( this );
	GROUP = group;
	DELETED = false;
	if( ! ConfGroups::GROUPS.contains( GROUP ) )
		ConfGroups::GROUPS.append( GROUP );
	connect( GlobalHotkeys::instance(), SIGNAL(mainConfigurationWindowCreatedSignal(MainConfigurationWindow*)), this, SLOT(mainConfigurationWindowCreated(MainConfigurationWindow*)) );
	if( forcecreate && MainConfigurationWindow::hasInstance() )
		mainConfigurationWindowCreated( MainConfigurationWindow::instance() );
}


ConfBuddiesShortcut::~ConfBuddiesShortcut()
{
	INSTANCES.removeOne( this );
	if( ! LINESEPARATOR.isNull()    ) delete LINESEPARATOR;
	if( ! HOTKEYEDIT.isNull()       ) delete HOTKEYEDIT;
	if( ! BUDDIESEDIT.isNull()      ) delete BUDDIESEDIT;
	if( ! SHOWMENUCHECKBOX.isNull() ) delete SHOWMENUCHECKBOX;
	if( ! DELETEBUTTON.isNull()     ) delete DELETEBUTTON;
}


void ConfBuddiesShortcut::commitUIData()
{
	HOTKEYEDIT->setText(  HOTKEYEDIT->text().trimmed()  );
	BUDDIESEDIT->setText( BUDDIESEDIT->text().trimmed() );
	HOTKEY  = HotKey( HOTKEYEDIT->text() );
	BUDDIES = BUDDIESEDIT->text().split( QRegExp( GLOBALHOTKEYS_COMMAREGEXP ), QString::SkipEmptyParts );
	SHOWMENU = SHOWMENUCHECKBOX->isChecked();
}


void ConfBuddiesShortcut::fillUIData()
{
	HOTKEYEDIT->setText(  HOTKEY.string()      );
	BUDDIESEDIT->setText( BUDDIES.join( ", " ) );
	SHOWMENUCHECKBOX->setChecked( SHOWMENU );
}


QString ConfBuddiesShortcut::serialized()
{
	SerializableQStringList list;
	list.append( HOTKEY.string()      );
	list.append( BUDDIES.join( ", " ) );
	list.append( SHOWMENU ? "1" : "0" );
	return list.serialized();
}


void ConfBuddiesShortcut::deserialize( QString serializedstring )
{
	SerializableQStringList list;
	list.deserialize( serializedstring );
	while( list.count() < 3 )
		list.append( "" );
	HOTKEY   = HotKey( list[0] );
	BUDDIES  =       ( list[1].split( QRegExp( GLOBALHOTKEYS_COMMAREGEXP ), QString::SkipEmptyParts ) );
	SHOWMENU =       ( list[2] == "1" );
	// fill UI data if widgets exist
	if( ! HOTKEYEDIT.isNull() )
		fillUIData();
}


void ConfBuddiesShortcut::focus()
{
	QScrollArea *scrollarea = NULL;
	QObject *object = HOTKEYEDIT;
	while( object != NULL )
	{
		scrollarea = dynamic_cast<QScrollArea*>( object );
		if( scrollarea != NULL )
			break;
		object = object->parent();
	}
	if( scrollarea != NULL )
	{
		scrollarea->widget()->adjustSize();
		scrollarea->ensureWidgetVisible( HOTKEYEDIT, 50, DELETEBUTTON->y() - HOTKEYEDIT->y() + 50 );
	}
	HOTKEYEDIT->setFocus( Qt::OtherFocusReason );
}


QList<ConfBuddiesShortcut*> ConfBuddiesShortcut::INSTANCES;


QList<ConfBuddiesShortcut*> ConfBuddiesShortcut::instances()
{
	return INSTANCES;
}


ConfBuddiesShortcut *ConfBuddiesShortcut::findByHotkey( HotKey hotkey )
{
	foreach( ConfBuddiesShortcut* confbuddiesshortcut, INSTANCES )
	{
		if( confbuddiesshortcut->hotKey() == hotkey )
			return confbuddiesshortcut;
	}
	return NULL;
}


void ConfBuddiesShortcut::mainConfigurationWindowCreated( MainConfigurationWindow *mainConfigurationWindow )
{
	DELETED = false;
	if( HOTKEYEDIT.isNull() )
	{
		ConfigGroupBox *configgroupbox = mainConfigurationWindow->widget()->configGroupBox(
			QT_TRANSLATE_NOOP( "@default", "Shortcuts"),
			QT_TRANSLATE_NOOP( "@default", "Global hotkeys" ),
			GROUP,
			true
		);
		LINESEPARATOR    = new ConfigLineSeparator(                                                                                               configgroupbox, NULL );
		HOTKEYEDIT       = new HotkeyEdit(          "", "", QT_TRANSLATE_NOOP( "@default", "Shortcut"                                      ), "", configgroupbox, NULL );
		BUDDIESEDIT      = new ConfigLineEdit(      "", "", QT_TRANSLATE_NOOP( "@default", "Buddies (comma separated)"                     ), "", configgroupbox, NULL );
		SHOWMENUCHECKBOX = new ConfigCheckBox(      "", "", QT_TRANSLATE_NOOP( "@default", "If possible, show a menu with available chats" ), "", configgroupbox, NULL );
		DELETEBUTTON     = new ConfigActionButton(          QT_TRANSLATE_NOOP( "@default", "Delete this shortcut"                          ), "", configgroupbox, NULL );
		connect( DELETEBUTTON, SIGNAL(clicked()), this, SLOT(deletebuttonClicked()) );
		fillUIData();
	}
	LINESEPARATOR->show();
	HOTKEYEDIT->show();
	BUDDIESEDIT->show();
	SHOWMENUCHECKBOX->show();
	DELETEBUTTON->show();
}


void ConfBuddiesShortcut::deletebuttonClicked()
{
	DELETED = true;
	LINESEPARATOR->hide();
	HOTKEYEDIT->hide();
	BUDDIESEDIT->hide();
	SHOWMENUCHECKBOX->hide();
	DELETEBUTTON->hide();
}




ConfBuddiesMenu::ConfBuddiesMenu( QObject *parent, QString group, bool forcecreate ) : QObject( parent )
{
	INSTANCES.append( this );
	GROUP = group;
	DELETED = false;
	CURRENTCHATS                 = false;
	PENDINGCHATS                 = false;
	RECENTCHATS                  = false;
	ONLINEBUDDIES                = false;
	ONLINEBUDDIESINCLUDEBLOCKING = false;
	ONEITEMPERBUDDY              = true;
	SORTSTATELESSBUDDIES         = true;
	SORTSTATELESSBUDDIESBYSTATUS = true;
	if( ! ConfGroups::GROUPS.contains( GROUP ) )
		ConfGroups::GROUPS.append( GROUP );
	connect( GlobalHotkeys::instance(), SIGNAL(mainConfigurationWindowCreatedSignal(MainConfigurationWindow*)), this, SLOT(mainConfigurationWindowCreated(MainConfigurationWindow*)) );
	if( forcecreate && MainConfigurationWindow::hasInstance() )
		mainConfigurationWindowCreated( MainConfigurationWindow::instance() );
}


ConfBuddiesMenu::~ConfBuddiesMenu()
{
	INSTANCES.removeOne( this );
	if( ! LINESEPARATOR.isNull()                        ) delete LINESEPARATOR;
	if( ! HOTKEYEDIT.isNull()                           ) delete HOTKEYEDIT;
	if( ! CURRENTCHATSCHECKBOX.isNull()                 ) delete CURRENTCHATSCHECKBOX;
	if( ! PENDINGCHATSCHECKBOX.isNull()                 ) delete PENDINGCHATSCHECKBOX;
	if( ! RECENTCHATSCHECKBOX.isNull()                  ) delete RECENTCHATSCHECKBOX;
	if( ! ONLINEBUDDIESCHECKBOX.isNull()                ) delete ONLINEBUDDIESCHECKBOX;
	if( ! ONLINEBUDDIESGROUPSEDIT.isNull()              ) delete ONLINEBUDDIESGROUPSEDIT;
	if( ! ONLINEBUDDIESINCLUDEBLOCKINGCHECKBOX.isNull() ) delete ONLINEBUDDIESINCLUDEBLOCKINGCHECKBOX;
	if( ! BUDDIESEDIT.isNull()                          ) delete BUDDIESEDIT;
	if( ! GROUPSEDIT.isNull()                           ) delete GROUPSEDIT;
	if( ! EXCLUDEBUDDIESEDIT.isNull()                   ) delete EXCLUDEBUDDIESEDIT;
	if( ! ONEITEMPERBUDDYCHECKBOX.isNull()              ) delete ONEITEMPERBUDDYCHECKBOX;
	if( ! SORTSTATELESSBUDDIESCHECKBOX.isNull()         ) delete SORTSTATELESSBUDDIESCHECKBOX;
	if( ! SORTSTATELESSBUDDIESBYSTATUSCHECKBOX.isNull() ) delete SORTSTATELESSBUDDIESBYSTATUSCHECKBOX;
	if( ! DELETEBUTTON.isNull()                         ) delete DELETEBUTTON;
}


QList<ConfBuddiesMenu*> ConfBuddiesMenu::INSTANCES;


QList<ConfBuddiesMenu*> ConfBuddiesMenu::instances()
{
	return INSTANCES;
}


void ConfBuddiesMenu::commitUIData()
{
	HOTKEYEDIT->setText(              HOTKEYEDIT->text().trimmed()              );
	ONLINEBUDDIESGROUPSEDIT->setText( ONLINEBUDDIESGROUPSEDIT->text().trimmed() );
	BUDDIESEDIT->setText(             BUDDIESEDIT->text().trimmed()             );
	GROUPSEDIT->setText(              GROUPSEDIT->text().trimmed()              );
	EXCLUDEBUDDIESEDIT->setText(      EXCLUDEBUDDIESEDIT->text().trimmed()      );
	HOTKEY                       = HotKey( HOTKEYEDIT->text() );
	CURRENTCHATS                 = CURRENTCHATSCHECKBOX->isChecked();
	PENDINGCHATS                 = PENDINGCHATSCHECKBOX->isChecked();
	RECENTCHATS                  = RECENTCHATSCHECKBOX->isChecked();
	ONLINEBUDDIES                = ONLINEBUDDIESCHECKBOX->isChecked();
	ONLINEBUDDIESGROUPS          = ONLINEBUDDIESGROUPSEDIT->text().split( QRegExp( GLOBALHOTKEYS_COMMAREGEXP ), QString::SkipEmptyParts );
	ONLINEBUDDIESINCLUDEBLOCKING = ONLINEBUDDIESINCLUDEBLOCKINGCHECKBOX->isChecked();
	BUDDIES                      = BUDDIESEDIT->text().split( QRegExp( GLOBALHOTKEYS_COMMAREGEXP ), QString::SkipEmptyParts );
	GROUPS                       = GROUPSEDIT->text().split( QRegExp( GLOBALHOTKEYS_COMMAREGEXP ), QString::SkipEmptyParts );
	EXCLUDEBUDDIES               = EXCLUDEBUDDIESEDIT->text().split( QRegExp( GLOBALHOTKEYS_COMMAREGEXP ), QString::SkipEmptyParts );
	ONEITEMPERBUDDY              = ONEITEMPERBUDDYCHECKBOX->isChecked();
	SORTSTATELESSBUDDIES         = SORTSTATELESSBUDDIESCHECKBOX->isChecked();
	SORTSTATELESSBUDDIESBYSTATUS = SORTSTATELESSBUDDIESBYSTATUSCHECKBOX->isChecked();
}


void ConfBuddiesMenu::fillUIData()
{
	HOTKEYEDIT->setText(                              HOTKEY.string()                  );
	CURRENTCHATSCHECKBOX->setChecked(                 CURRENTCHATS                     );
	PENDINGCHATSCHECKBOX->setChecked(                 PENDINGCHATS                     );
	RECENTCHATSCHECKBOX->setChecked(                  RECENTCHATS                      );
	ONLINEBUDDIESCHECKBOX->setChecked(                ONLINEBUDDIES                    );
	ONLINEBUDDIESGROUPSEDIT->setText(                 ONLINEBUDDIESGROUPS.join( ", " ) );
	ONLINEBUDDIESINCLUDEBLOCKINGCHECKBOX->setChecked( ONLINEBUDDIESINCLUDEBLOCKING     );
	BUDDIESEDIT->setText(                             BUDDIES.join( ", " )             );
	GROUPSEDIT->setText(                              GROUPS.join( ", " )              );
	EXCLUDEBUDDIESEDIT->setText(                      EXCLUDEBUDDIES.join( ", " )      );
	ONEITEMPERBUDDYCHECKBOX->setChecked(              ONEITEMPERBUDDY                  );
	SORTSTATELESSBUDDIESCHECKBOX->setChecked(         SORTSTATELESSBUDDIES             );
	SORTSTATELESSBUDDIESBYSTATUSCHECKBOX->setChecked( SORTSTATELESSBUDDIESBYSTATUS     );
	ONLINEBUDDIESGROUPSEDIT->setEnabled( ONLINEBUDDIESCHECKBOX->isChecked() );
	ONLINEBUDDIESINCLUDEBLOCKINGCHECKBOX->setEnabled( ONLINEBUDDIESCHECKBOX->isChecked() );
	SORTSTATELESSBUDDIESBYSTATUSCHECKBOX->setEnabled( SORTSTATELESSBUDDIESCHECKBOX->isChecked() );
}


QString ConfBuddiesMenu::serialized()
{
	SerializableQStringList list;
	list.append( HOTKEY.string()                          );
	list.append( CURRENTCHATS                 ? "1" : "0" );
	list.append( PENDINGCHATS                 ? "1" : "0" );
	list.append( RECENTCHATS                  ? "1" : "0" );
	list.append( ONLINEBUDDIES                ? "1" : "0" );
	list.append( ONLINEBUDDIESGROUPS.join( ", " )         );
	list.append( ONLINEBUDDIESINCLUDEBLOCKING ? "1" : "0" );
	list.append( BUDDIES.join( ", " )                     );
	list.append( GROUPS.join( ", " )                      );
	list.append( EXCLUDEBUDDIES.join( ", " )              );
	list.append( ONEITEMPERBUDDY              ? "1" : "0" );
	list.append( SORTSTATELESSBUDDIES         ? "1" : "0" );
	list.append( SORTSTATELESSBUDDIESBYSTATUS ? "1" : "0" );
	return list.serialized();
}


void ConfBuddiesMenu::deserialize( QString serializedstring )
{
	SerializableQStringList list;
	list.deserialize( serializedstring );
	while( list.count() < 13 )
		list.append( "" );
	HOTKEY                       = HotKey( list[ 0] );
	CURRENTCHATS                 =       ( list[ 1] == "1" );
	PENDINGCHATS                 =       ( list[ 2] == "1" );
	RECENTCHATS                  =       ( list[ 3] == "1" );
	ONLINEBUDDIES                =       ( list[ 4] == "1" );
	ONLINEBUDDIESGROUPS          =       ( list[ 5].split( QRegExp( GLOBALHOTKEYS_COMMAREGEXP ), QString::SkipEmptyParts ) );
	ONLINEBUDDIESINCLUDEBLOCKING =       ( list[ 6] == "1" );
	BUDDIES                      =       ( list[ 7].split( QRegExp( GLOBALHOTKEYS_COMMAREGEXP ), QString::SkipEmptyParts ) );
	GROUPS                       =       ( list[ 8].split( QRegExp( GLOBALHOTKEYS_COMMAREGEXP ), QString::SkipEmptyParts ) );
	EXCLUDEBUDDIES               =       ( list[ 9].split( QRegExp( GLOBALHOTKEYS_COMMAREGEXP ), QString::SkipEmptyParts ) );
	ONEITEMPERBUDDY              =       ( list[10] == "1" );
	SORTSTATELESSBUDDIES         =       ( list[11] == "1" );
	SORTSTATELESSBUDDIESBYSTATUS =       ( list[12] == "1" );
	// fill UI data if widgets exist
	if( ! HOTKEYEDIT.isNull() )
		fillUIData();
}


void ConfBuddiesMenu::focus()
{
	QScrollArea *scrollarea = NULL;
	QObject *object = HOTKEYEDIT;
	while( object != NULL )
	{
		scrollarea = dynamic_cast<QScrollArea*>( object );
		if( scrollarea != NULL )
			break;
		object = object->parent();
	}
	if( scrollarea != NULL )
	{
		scrollarea->widget()->adjustSize();
		scrollarea->ensureWidgetVisible( HOTKEYEDIT, 50, DELETEBUTTON->y() - HOTKEYEDIT->y() + 50 );
	}
	HOTKEYEDIT->setFocus( Qt::OtherFocusReason );
}


ConfBuddiesMenu *ConfBuddiesMenu::findByHotkey( HotKey hotkey )
{
	foreach( ConfBuddiesMenu* confbuddiesmenu, INSTANCES )
	{
		if( confbuddiesmenu->hotKey() == hotkey )
			return confbuddiesmenu;
	}
	return NULL;
}


void ConfBuddiesMenu::mainConfigurationWindowCreated( MainConfigurationWindow *mainConfigurationWindow )
{
	DELETED = false;
	if( HOTKEYEDIT.isNull() )
	{
		ConfigGroupBox *configgroupbox = mainConfigurationWindow->widget()->configGroupBox(
			QT_TRANSLATE_NOOP( "@default", "Shortcuts" ),
			QT_TRANSLATE_NOOP( "@default", "Global hotkeys" ),
			GROUP,
			true
		);
		LINESEPARATOR                        = new ConfigLineSeparator(                                                                                                                                       configgroupbox, NULL );
		HOTKEYEDIT                           = new HotkeyEdit(          "", "", QT_TRANSLATE_NOOP( "@default", "Shortcut"                                            ), ""                                  , configgroupbox, NULL );
		CURRENTCHATSCHECKBOX                 = new ConfigCheckBox(      "", "", QT_TRANSLATE_NOOP( "@default", "Include current chats"                               ), ""                                  , configgroupbox, NULL );
		PENDINGCHATSCHECKBOX                 = new ConfigCheckBox(      "", "", QT_TRANSLATE_NOOP( "@default", "Include chats with pending messages"                 ), ""                                  , configgroupbox, NULL );
		RECENTCHATSCHECKBOX                  = new ConfigCheckBox(      "", "", QT_TRANSLATE_NOOP( "@default", "Include recent chats"                                ), ""                                  , configgroupbox, NULL );
		ONLINEBUDDIESCHECKBOX                = new ConfigCheckBox(      "", "", QT_TRANSLATE_NOOP( "@default", "Include online buddies"                              ), ""                                  , configgroupbox, NULL );
		ONLINEBUDDIESGROUPSEDIT              = new ConfigLineEdit(      "", "", QT_TRANSLATE_NOOP( "@default", "only from these groups (comma separated)"            ), "leave empty to disable this filter", configgroupbox, NULL );
		ONLINEBUDDIESINCLUDEBLOCKINGCHECKBOX = new ConfigCheckBox(      "", "", QT_TRANSLATE_NOOP( "@default", "Treat buddies blocking me as online"                 ), ""                                  , configgroupbox, NULL );
		BUDDIESEDIT                          = new ConfigLineEdit(      "", "", QT_TRANSLATE_NOOP( "@default", "Include these buddies (comma separated)"             ), ""                                  , configgroupbox, NULL );
		GROUPSEDIT                           = new ConfigLineEdit(      "", "", QT_TRANSLATE_NOOP( "@default", "Include buddies from these groups (comma separated)" ), ""                                  , configgroupbox, NULL );
		EXCLUDEBUDDIESEDIT                   = new ConfigLineEdit(      "", "", QT_TRANSLATE_NOOP( "@default", "Exclude these buddies (comma separated)"             ), ""                                  , configgroupbox, NULL );
		ONEITEMPERBUDDYCHECKBOX              = new ConfigCheckBox(      "", "", QT_TRANSLATE_NOOP( "@default", "Show at most one item per buddy"                     ), ""                                  , configgroupbox, NULL );
		SORTSTATELESSBUDDIESCHECKBOX         = new ConfigCheckBox(      "", "", QT_TRANSLATE_NOOP( "@default", "Sort stateless buddies"                              ), ""                                  , configgroupbox, NULL );
		SORTSTATELESSBUDDIESBYSTATUSCHECKBOX = new ConfigCheckBox(      "", "", QT_TRANSLATE_NOOP( "@default", "Sort by status"                                      ), ""                                  , configgroupbox, NULL );
		DELETEBUTTON                         = new ConfigActionButton(          QT_TRANSLATE_NOOP( "@default", "Delete this menu"                                    ), ""                                  , configgroupbox, NULL );
		connect( DELETEBUTTON, SIGNAL(clicked()), this, SLOT(deletebuttonClicked()) );
		connect( ONLINEBUDDIESCHECKBOX       , SIGNAL(toggled(bool)), ONLINEBUDDIESGROUPSEDIT             , SLOT(setEnabled(bool)) );
		connect( ONLINEBUDDIESCHECKBOX       , SIGNAL(toggled(bool)), ONLINEBUDDIESINCLUDEBLOCKINGCHECKBOX, SLOT(setEnabled(bool)) );
		connect( SORTSTATELESSBUDDIESCHECKBOX, SIGNAL(toggled(bool)), SORTSTATELESSBUDDIESBYSTATUSCHECKBOX, SLOT(setEnabled(bool)) );
		fillUIData();
	}
	LINESEPARATOR->show();
	HOTKEYEDIT->show();
	CURRENTCHATSCHECKBOX->show();
	PENDINGCHATSCHECKBOX->show();
	RECENTCHATSCHECKBOX->show();
	ONLINEBUDDIESCHECKBOX->show();
	ONLINEBUDDIESGROUPSEDIT->show();
	ONLINEBUDDIESINCLUDEBLOCKINGCHECKBOX->show();
	BUDDIESEDIT->show();
	GROUPSEDIT->show();
	EXCLUDEBUDDIESEDIT->show();
	ONEITEMPERBUDDYCHECKBOX->show();
	SORTSTATELESSBUDDIESCHECKBOX->show();
	SORTSTATELESSBUDDIESBYSTATUSCHECKBOX->show();
	DELETEBUTTON->show();
}


void ConfBuddiesMenu::deletebuttonClicked()
{
	DELETED = true;
	LINESEPARATOR->hide();
	HOTKEYEDIT->hide();
	CURRENTCHATSCHECKBOX->hide();
	PENDINGCHATSCHECKBOX->hide();
	RECENTCHATSCHECKBOX->hide();
	ONLINEBUDDIESCHECKBOX->hide();
	ONLINEBUDDIESGROUPSEDIT->hide();
	ONLINEBUDDIESINCLUDEBLOCKINGCHECKBOX->hide();
	BUDDIESEDIT->hide();
	GROUPSEDIT->hide();
	EXCLUDEBUDDIESEDIT->hide();
	ONEITEMPERBUDDYCHECKBOX->hide();
	SORTSTATELESSBUDDIESCHECKBOX->hide();
	SORTSTATELESSBUDDIESBYSTATUSCHECKBOX->hide();
	DELETEBUTTON->hide();
}
