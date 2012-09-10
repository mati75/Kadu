/****************************************************************************
*                                                                           *
*   NExtInfo plugin for Kadu                                                *
*   Copyright (C) 2008-2012  Piotr Dąbrowski ultr@ultr.pl                   *
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
#include <QFileDialog>
#include <QList>
#include <QMenu>
#include <QMessageBox>
#include <QPair>
#include <QPointer>
#include <QRegExp>
#include <QScriptEngine>
#include <QTextCodec>
#include <QTextStream>


#include "accounts/account-manager.h"
#include "avatars/avatar-manager.h"
#include "buddies/buddy.h"
#include "buddies/buddy-gender.h"
#include "buddies/buddy-manager.h"
#include "configuration/configuration-file.h"
#include "contacts/contact-manager.h"
#include "core/core.h"
#include "gui/actions/action.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/windows/kadu-window.h"
#include "gui/widgets/talkable-menu-manager.h"
#include "misc/kadu-paths.h"
#include "notify/notification.h"
#include "notify/notification-manager.h"
#include "parser/parser.h"
#include "activate.h"
#include "debug.h"

#include "nextinfo.h"

#include "buddynextinfodata.h"
#include "extendedinformationwidgets.h"
#include "gettags.h"




int NExtInfo::init( bool firstLoad )
{
	Q_UNUSED( firstLoad );
	kdebugf();
	MainConfigurationWindow::registerUiFile( KaduPaths::instance()->dataPath() + "plugins/configuration/nextinfo.ui" );
	MainConfigurationWindow::registerUiHandler( this );
	kdebugf2();
	return 0;
}


void NExtInfo::done()
{
	kdebugf();
	MainConfigurationWindow::unregisterUiHandler( this );
	MainConfigurationWindow::unregisterUiFile( KaduPaths::instance()->dataPath() + "plugins/configuration/nextinfo.ui" );
	kdebugf2();
}


NExtInfo::NExtInfo()
{
	// data format and update
	int dataformatversion = config_file.readNumEntry( "NExtInfo", "DataFormatVersion", 0 );
	if( dataformatversion < NEXTINFO_DATAFORMATVERSION )
	{
		config_file.writeEntry( "NExtInfo", "DataFormatVersion", NEXTINFO_DATAFORMATVERSION );
		importOldData( dataformatversion );
	}
	// create default configuration if needed
	createDefaultConfiguration();
	// create birthdaynamedaytimer QTimer and connect it
	birthdaynamedaytimer = new QTimer();
	connect( birthdaynamedaytimer, SIGNAL(timeout()), this, SLOT(notifyBirthdayNameday()) );
	// read the configuration and force its usage
	configurationUpdated();
	// add NExtInfo actions to Talkable's context menu
	actionbirthday = new ActionDescription(
		this, ActionDescription::TypeUser, "nextinfo_birthdayinform", this, SLOT(actionBirthdayTriggered(QAction*,bool)),
		KaduIcon( "external_modules/nextinfo-birthday" ), qApp->translate( "@nextinfo", "Birthday notifications" ),
		true, NExtInfo::updateActionBirthday
	);
	TalkableMenuManager::instance()->addListActionDescription( actionbirthday, TalkableMenuItem::CategoryManagement, 200 );
	connect( actionbirthday, SIGNAL(actionCreated(Action*)), this, SLOT(actionBirthdayCreated(Action*)) );
	actionnameday = new ActionDescription(
		this, ActionDescription::TypeUser, "nextinfo_namedayinform", this, SLOT(actionNamedayTriggered(QAction*,bool)),
		KaduIcon( "external_modules/nextinfo-nameday" ), qApp->translate( "@nextinfo", "Name-day notifications" ),
		true, NExtInfo::updateActionNameday
	);
	TalkableMenuManager::instance()->addListActionDescription( actionnameday, TalkableMenuItem::CategoryManagement, 200 );
	connect( actionnameday, SIGNAL(actionCreated(Action*)), this, SLOT(actionNamedayCreated(Action*)) );
	// register parser tags
	Parser::registerTag( "nextinfo_middleName", getTag_middleName );
	Parser::registerTag( "nextinfo_address"   , getTag_address    );
	Parser::registerTag( "nextinfo_city"      , getTag_city       );
	Parser::registerTag( "nextinfo_email2"    , getTag_email2     );
	Parser::registerTag( "nextinfo_birthday"  , getTag_birthday   );
	Parser::registerTag( "nextinfo_nameday"   , getTag_nameday    );
	Parser::registerTag( "nextinfo_interests" , getTag_interests  );
	Parser::registerTag( "nextinfo_notes"     , getTag_notes      );
	// register the notification
	notifyevent = new NotifyEvent( "NExtInfo", NotifyEvent::CallbackNotRequired, QT_TRANSLATE_NOOP( "@default", "Birthday/name-day notification" ) );
	NotificationManager::instance()->registerNotifyEvent( notifyevent );
	// add widgets to already opened BuddyDataWindow-s
	triggerAllBuddyDataWindowsCreated();
	// check birthdays and name-days at startup
	QTimer::singleShot( NEXTINFO_INITIALNOTIFYBIRTHDAYNAMEDAYINTERVAL, this, SLOT(notifyBirthdayNameday()) );
	// start the birthdaynamedaytimer timer
	birthdaynamedaytimer->start( config_file.readNumEntry( "NExtInfo", "DelayBetweenNotifications" ) * 1000 );
}


NExtInfo::~NExtInfo()
{
	// stop the birthdaynamedaytimer timer
	birthdaynamedaytimer->stop();
	// delete all instances of ExtendedInformationWidgets
	foreach( ExtendedInformationWidgets* widgets, ExtendedInformationWidgets::instances() )
		widgets->deleteLater();
	// unregister the notification
	NotificationManager::instance()->unregisterNotifyEvent( notifyevent );
	delete notifyevent;
	// remove NExtInfo actions from Talkable's context menu
	disconnect( actionbirthday );
	disconnect( actionnameday );
	TalkableMenuManager::instance()->removeListActionDescription( actionbirthday );
	TalkableMenuManager::instance()->removeListActionDescription( actionnameday  );
	actionbirthday->deleteLater();
	actionnameday->deleteLater();
	// unregister parser tags
	Parser::unregisterTag( "nextinfo_middleName");
	Parser::unregisterTag( "nextinfo_address"   );
	Parser::unregisterTag( "nextinfo_city"      );
	Parser::unregisterTag( "nextinfo_email2"    );
	Parser::unregisterTag( "nextinfo_birthday"  );
	Parser::unregisterTag( "nextinfo_nameday"   );
	Parser::unregisterTag( "nextinfo_interests" );
	Parser::unregisterTag( "nextinfo_notes"     );
}


void NExtInfo::mainConfigurationWindowCreated( MainConfigurationWindow *mainConfigurationWindow )
{
	connect( mainConfigurationWindow->widget()->widgetById("nextinfo/enableNotifications"), SIGNAL(toggled(bool)), mainConfigurationWindow->widget()->widgetById("nextinfo/notifyAboutBirthdays")     , SLOT(setEnabled(bool)) );
	connect( mainConfigurationWindow->widget()->widgetById("nextinfo/enableNotifications"), SIGNAL(toggled(bool)), mainConfigurationWindow->widget()->widgetById("nextinfo/notifyAboutNamedays")      , SLOT(setEnabled(bool)) );
	connect( mainConfigurationWindow->widget()->widgetById("nextinfo/enableNotifications"), SIGNAL(toggled(bool)), mainConfigurationWindow->widget()->widgetById("nextinfo/notificationAdvance")      , SLOT(setEnabled(bool)) );
	connect( mainConfigurationWindow->widget()->widgetById("nextinfo/enableNotifications"), SIGNAL(toggled(bool)), mainConfigurationWindow->widget()->widgetById("nextinfo/delayBetweenNotifications"), SLOT(setEnabled(bool)) );
	connect( mainConfigurationWindow->widget()->widgetById("nextinfo/showHelp"), SIGNAL(clicked()), this, SLOT(showHelp()) );
	connect( mainConfigurationWindow->widget()->widgetById("nextinfo/importDataFromExtInfo"), SIGNAL(clicked()), this, SLOT(importDataFromExtInfo()) );
}


void NExtInfo::createDefaultConfiguration()
{
	config_file.addVariable( "NExtInfo", "EnableNotifications"      , true                                      );
	config_file.addVariable( "NExtInfo", "NotifyAboutBirthdays"     , true                                      );
	config_file.addVariable( "NExtInfo", "NotifyAboutNamedays"      , true                                      );
	config_file.addVariable( "NExtInfo", "NotificationAdvance"      , NEXTINFO_DEFAULTNOTIFICATIONADVANCE       );
	config_file.addVariable( "NExtInfo", "DelayBetweenNotifications", NEXTINFO_DEFAULTDELAYBETWEENNOTIFICATIONS );
}


void NExtInfo::configurationUpdated()
{
	// restart the birthdaynamedaytimer timer if it is running
	if( birthdaynamedaytimer->isActive() )
	{
		birthdaynamedaytimer->stop();
		birthdaynamedaytimer->start( config_file.readNumEntry(  "NExtInfo", "DelayBetweenNotifications" ) * 1000 );
	}
}


QPair< bool, QPair<int,int> > NExtInfo::checkBirthdayNotify( Buddy buddy )
{
	QPair< bool, QPair<int,int> > result( false, QPair<int,int>( 0, 0 ) );
	if( ( ! config_file.readBoolEntry( "NExtInfo", "EnableNotifications" ) ) || ( ! config_file.readBoolEntry( "NExtInfo", "NotifyAboutBirthdays" ) ) )
		return result;
	QDate nextbirthdaydate = BuddyNExtInfoData::nextBirthdayDate( buddy );
	if( ! nextbirthdaydate.isValid() )
		return result;
	int remainingdays = QDate::currentDate().daysTo( nextbirthdaydate );
	result.second.first = remainingdays;
	result.second.second = BuddyNExtInfoData::nextBirthdayAge( buddy );
	result.first = ( remainingdays <= config_file.readNumEntry( "NExtInfo", "NotificationAdvance" ) );
	return result;
}


QPair< bool, QPair<int,int> > NExtInfo::checkNamedayNotify( Buddy buddy )
{
	QPair< bool, QPair<int,int> > result( false, QPair<int,int>( 0, 0 ) );
	if( ( ! config_file.readBoolEntry( "NExtInfo", "EnableNotifications" ) ) || ( ! config_file.readBoolEntry( "NExtInfo", "NotifyAboutNamedays" ) ) )
		return result;
	QDate nextnamedaydate = BuddyNExtInfoData::nextNamedayDate( buddy );
	if( ! nextnamedaydate.isValid() )
		return result;
	int remainingdays = QDate::currentDate().daysTo( nextnamedaydate );
	result.second.first = remainingdays;
	result.second.second = BuddyNExtInfoData::age( buddy );
	result.first = ( remainingdays <= config_file.readNumEntry( "NExtInfo", "NotificationAdvance" ) );
	return result;
}


bool NExtInfo::checkBirthdayRemind( Buddy buddy )
{
	return ( BuddyNExtInfoData::birthdayRemindDate( buddy ) <= QDate::currentDate() );
}


bool NExtInfo::checkNamedayRemind( Buddy buddy )
{
	return ( BuddyNExtInfoData::namedayRemindDate( buddy ) <= QDate::currentDate() );
}


void NExtInfo::setBirthdayRemind( Buddy buddy, RemindTime time )
{
	QDate date;
	switch( time )
	{
		case RemindTimeNow:
			date = QDate::currentDate();
			break;
		case RemindTimeTomorrow:
			date = QDate::currentDate().addDays( 1 );
			break;
		case RemindTimeTheDay:
			date = BuddyNExtInfoData::nextBirthdayDate( buddy );
			break;
		case RemindTimeNextYear:
			date = BuddyNExtInfoData::nextBirthdayDate( buddy ).addDays( 2 );
			break;
	}
	BuddyNExtInfoData::setBirthdayRemindDate( buddy, date );
}


void NExtInfo::setNamedayRemind( Buddy buddy, RemindTime time )
{
	QDate date;
	switch( time )
	{
		case RemindTimeNow:
			date = QDate::currentDate();
			break;
		case RemindTimeTomorrow:
			date = QDate::currentDate().addDays( 1 );
			break;
		case RemindTimeTheDay:
			date = BuddyNExtInfoData::nextNamedayDate( buddy );
			break;
		case RemindTimeNextYear:
			date = BuddyNExtInfoData::nextNamedayDate( buddy ).addDays( 2 );
			break;
	}
	BuddyNExtInfoData::setNamedayRemindDate( buddy, date );
}


void NExtInfo::actionBirthdayCreated( Action *action )
{
	QMenu *actionbirthdaymenu = new QMenu( Core::instance()->kaduWindow() );
	QAction *actionbirthdaynow      = new QAction( KaduIcon( "external_modules/nextinfo-birthday" ).icon(), qApp->translate( "@nextinfo", "Keep reminding me now" ), actionbirthdaymenu );
	QAction *actionbirthdaytomorrow = new QAction( KaduIcon( "external_modules/nextinfo-birthday" ).icon(), qApp->translate( "@nextinfo", "Remind me tomorrow"    ), actionbirthdaymenu );
	QAction *actionbirthdaytheday   = new QAction( KaduIcon( "external_modules/nextinfo-birthday" ).icon(), qApp->translate( "@nextinfo", "Remind me on the day"  ), actionbirthdaymenu );
	QAction *actionbirthdaynextyear = new QAction( KaduIcon( "external_modules/nextinfo-birthday" ).icon(), qApp->translate( "@nextinfo", "Remind me next year"   ), actionbirthdaymenu );
	actionbirthdaynow->setCheckable(      true );
	actionbirthdaytomorrow->setCheckable( true );
	actionbirthdaytheday->setCheckable(   true );
	actionbirthdaynextyear->setCheckable( true );
	QActionGroup *actiongroup = new QActionGroup( actionbirthdaymenu );
	actiongroup->addAction( actionbirthdaynow      );
	actiongroup->addAction( actionbirthdaytomorrow );
	actiongroup->addAction( actionbirthdaytheday   );
	actiongroup->addAction( actionbirthdaynextyear );
	actionbirthdaynow->setData(      QVariant::fromValue( (QObject*)action ) );
	actionbirthdaytomorrow->setData( QVariant::fromValue( (QObject*)action ) );
	actionbirthdaytheday->setData(   QVariant::fromValue( (QObject*)action ) );
	actionbirthdaynextyear->setData( QVariant::fromValue( (QObject*)action ) );
	actionbirthdaymenu->addAction( actionbirthdaynow      );
	actionbirthdaymenu->addAction( actionbirthdaytomorrow );
	actionbirthdaymenu->addAction( actionbirthdaytheday   );
	actionbirthdaymenu->addAction( actionbirthdaynextyear );
	connect( actionbirthdaynow     , SIGNAL(triggered()), this, SLOT(actionBirthdayNowTriggered())      );
	connect( actionbirthdaytomorrow, SIGNAL(triggered()), this, SLOT(actionBirthdayTomorrowTriggered()) );
	connect( actionbirthdaytheday  , SIGNAL(triggered()), this, SLOT(actionBirthdayTheDayTriggered())   );
	connect( actionbirthdaynextyear, SIGNAL(triggered()), this, SLOT(actionBirthdayNextYearTriggered()) );
	action->setMenu( actionbirthdaymenu );
	updateActionBirthdayMenu( action );
}


void NExtInfo::actionNamedayCreated( Action *action )
{
	QMenu *actionnamedaymenu = new QMenu( Core::instance()->kaduWindow() );
	QAction *actionnamedaynow      = new QAction( KaduIcon( "external_modules/nextinfo-nameday" ).icon(), qApp->translate( "@nextinfo", "Keep reminding me now" ), actionnamedaymenu );
	QAction *actionnamedaytomorrow = new QAction( KaduIcon( "external_modules/nextinfo-nameday" ).icon(), qApp->translate( "@nextinfo", "Remind me tomorrow"    ), actionnamedaymenu );
	QAction *actionnamedaytheday   = new QAction( KaduIcon( "external_modules/nextinfo-nameday" ).icon(), qApp->translate( "@nextinfo", "Remind me on the day"  ), actionnamedaymenu );
	QAction *actionnamedaynextyear = new QAction( KaduIcon( "external_modules/nextinfo-nameday" ).icon(), qApp->translate( "@nextinfo", "Remind me next year"   ), actionnamedaymenu );
	actionnamedaynow->setCheckable(      true );
	actionnamedaytomorrow->setCheckable( true );
	actionnamedaytheday->setCheckable(   true );
	actionnamedaynextyear->setCheckable( true );
	QActionGroup *actiongroup = new QActionGroup( actionnamedaymenu );
	actiongroup->addAction( actionnamedaynow      );
	actiongroup->addAction( actionnamedaytomorrow );
	actiongroup->addAction( actionnamedaytheday   );
	actiongroup->addAction( actionnamedaynextyear );
	actionnamedaynow->setData(      QVariant::fromValue( (QObject*)action ) );
	actionnamedaytomorrow->setData( QVariant::fromValue( (QObject*)action ) );
	actionnamedaytheday->setData(   QVariant::fromValue( (QObject*)action ) );
	actionnamedaynextyear->setData( QVariant::fromValue( (QObject*)action ) );
	actionnamedaymenu->addAction( actionnamedaynow      );
	actionnamedaymenu->addAction( actionnamedaytomorrow );
	actionnamedaymenu->addAction( actionnamedaytheday   );
	actionnamedaymenu->addAction( actionnamedaynextyear );
	connect( actionnamedaynow     , SIGNAL(triggered()), this, SLOT(actionNamedayNowTriggered())      );
	connect( actionnamedaytomorrow, SIGNAL(triggered()), this, SLOT(actionNamedayTomorrowTriggered()) );
	connect( actionnamedaytheday  , SIGNAL(triggered()), this, SLOT(actionNamedayTheDayTriggered())   );
	connect( actionnamedaynextyear, SIGNAL(triggered()), this, SLOT(actionNamedayNextYearTriggered()) );
	action->setMenu( actionnamedaymenu );
	updateActionNamedayMenu( action );
}


void NExtInfo::updateActionsBirthday()
{
	foreach( Action *action, actionbirthday->actions() )
		updateActionBirthday( action );
}


void NExtInfo::updateActionsNameday()
{
	foreach( Action *action, actionnameday->actions() )
		updateActionNameday( action );
}


void NExtInfo::updateActionBirthday( Action *action )
{
	if( ! action )
		return;
	action->setChecked( false );
	action->setEnabled( false );
	// buddy
	Buddy buddy = action->context()->buddies().toBuddy();
	if( ! buddy )
		return;
	// check
	QPair< bool, QPair<int,int> > checkdata;
	checkdata = checkBirthdayNotify( buddy );
	if( checkdata.first )
	{
		action->setChecked( checkBirthdayRemind( buddy ) );
		action->setEnabled( true );
		updateActionBirthdayMenu( action );
	}
}


void NExtInfo::updateActionNameday( Action *action )
{
	if( ! action )
		return;
	action->setChecked( false );
	action->setEnabled( false );
	// buddy
	Buddy buddy = action->context()->buddies().toBuddy();
	if( ! buddy )
		return;
	// check
	QPair< bool, QPair<int,int> > checkdata;
	checkdata = checkNamedayNotify( buddy );
	if( checkdata.first )
	{
		action->setEnabled( true );
		action->setChecked( checkNamedayRemind( buddy ) );
		updateActionNamedayMenu( action );
	}
}


void NExtInfo::updateActionBirthdayMenu( Action *action )
{
	if( action->menu() == NULL )
		return ;
	// buddy
	Buddy buddy = action->context()->buddies().toBuddy();
	if( ! buddy )
		return;
	// check
	if( BuddyNExtInfoData::birthdayRemindDate( buddy ) <= QDate::currentDate() )
		action->menu()->actions().at(0)->setChecked( true );
	else if( QDate::currentDate().daysTo( BuddyNExtInfoData::birthdayRemindDate( buddy ) ) == 1 )
		action->menu()->actions().at(1)->setChecked( true );
	else if( BuddyNExtInfoData::birthdayRemindDate( buddy ) == BuddyNExtInfoData::nextBirthdayDate( buddy ) )
		action->menu()->actions().at(2)->setChecked( true );
	else
		action->menu()->actions().at(3)->setChecked( true );
}


void NExtInfo::updateActionNamedayMenu( Action *action )
{
	if( action->menu() == NULL )
		return ;
	// buddy
	Buddy buddy = action->context()->buddies().toBuddy();
	if( ! buddy )
		return;
	// check
	if( BuddyNExtInfoData::namedayRemindDate( buddy ) <= QDate::currentDate() )
		action->menu()->actions().at(0)->setChecked( true );
	else if( QDate::currentDate().daysTo( BuddyNExtInfoData::namedayRemindDate( buddy ) ) == 1 )
		action->menu()->actions().at(1)->setChecked( true );
	else if( BuddyNExtInfoData::namedayRemindDate( buddy ) == BuddyNExtInfoData::nextNamedayDate( buddy ) )
		action->menu()->actions().at(2)->setChecked( true );
	else
		action->menu()->actions().at(3)->setChecked( true );
}


void NExtInfo::actionBirthdayTriggered( QAction *sender, bool checked )
{
	// action
	Action *action = dynamic_cast<Action*>( sender );
	if( ! action )
		return;
	// buddy
	Buddy buddy = action->context()->buddies().toBuddy();
	if( ! buddy )
		return;
	// set
	setBirthdayRemind( buddy, checked ? RemindTimeNow : RemindTimeTomorrow );
	// update the menu
	updateActionBirthdayMenu( action );
}


void NExtInfo::actionNamedayTriggered( QAction *sender, bool checked )
{
	// action
	Action *action = dynamic_cast<Action*>( sender );
	if( ! action )
		return;
	// buddy
	Buddy buddy = action->context()->buddies().toBuddy();
	if( ! buddy )
		return;
	// set
	setNamedayRemind( buddy, checked ? RemindTimeNow : RemindTimeTomorrow );
	// update the menu
	updateActionNamedayMenu( action );
}


void NExtInfo::actionBirthdayNowTriggered()
{
	// main action
	QAction *qaction = dynamic_cast<QAction*>( sender() );
	if( ! qaction )
		return;
	Action *action = dynamic_cast<Action*>( qaction->data().value<QObject*>() );
	if( ! action )
		return;
	// buddy
	Buddy buddy = action->context()->buddies().toBuddy();
	if( ! buddy )
		return;
	// set
	setBirthdayRemind( buddy, RemindTimeNow );
	// update the main action
	updateActionsBirthday();
}


void NExtInfo::actionBirthdayTomorrowTriggered()
{
	// main action
	QAction *qaction = dynamic_cast<QAction*>( sender() );
	if( ! qaction )
		return;
	Action *action = dynamic_cast<Action*>( qaction->data().value<QObject*>() );
	if( ! action )
		return;
	// buddy
	Buddy buddy = action->context()->buddies().toBuddy();
	if( ! buddy )
		return;
	// set
	setBirthdayRemind( buddy, RemindTimeTomorrow );
	// update the main action
	updateActionsBirthday();
}


void NExtInfo::actionBirthdayTheDayTriggered()
{
	// main action
	QAction *qaction = dynamic_cast<QAction*>( sender() );
	if( ! qaction )
		return;
	Action *action = dynamic_cast<Action*>( qaction->data().value<QObject*>() );
	if( ! action )
		return;
	// buddy
	Buddy buddy = action->context()->buddies().toBuddy();
	if( ! buddy )
		return;
	// set
	setBirthdayRemind( buddy, RemindTimeTheDay );
	// update the main action
	updateActionsBirthday();
}


void NExtInfo::actionBirthdayNextYearTriggered()
{
	// main action
	QAction *qaction = dynamic_cast<QAction*>( sender() );
	if( ! qaction )
		return;
	Action *action = dynamic_cast<Action*>( qaction->data().value<QObject*>() );
	if( ! action )
		return;
	// buddy
	Buddy buddy = action->context()->buddies().toBuddy();
	if( ! buddy )
		return;
	// set
	setBirthdayRemind( buddy, RemindTimeNextYear );
	// update the main action
	updateActionsBirthday();
}


void NExtInfo::actionNamedayNowTriggered()
{
	// main action
	QAction *qaction = dynamic_cast<QAction*>( sender() );
	if( ! qaction )
		return;
	Action *action = dynamic_cast<Action*>( qaction->data().value<QObject*>() );
	if( ! action )
		return;
	// buddy
	Buddy buddy = action->context()->buddies().toBuddy();
	if( ! buddy )
		return;
	// set
	setNamedayRemind( buddy, RemindTimeNow );
	// update the main action
	updateActionsNameday();
}


void NExtInfo::actionNamedayTomorrowTriggered()
{
	// main action
	QAction *qaction = dynamic_cast<QAction*>( sender() );
	if( ! qaction )
		return;
	Action *action = dynamic_cast<Action*>( qaction->data().value<QObject*>() );
	if( ! action )
		return;
	// buddy
	Buddy buddy = action->context()->buddies().toBuddy();
	if( ! buddy )
		return;
	// set
	setNamedayRemind( buddy, RemindTimeTomorrow );
	// update the main action
	updateActionsNameday();
}


void NExtInfo::actionNamedayTheDayTriggered()
{
	// main action
	QAction *qaction = dynamic_cast<QAction*>( sender() );
	if( ! qaction )
		return;
	Action *action = dynamic_cast<Action*>( qaction->data().value<QObject*>() );
	if( ! action )
		return;
	// buddy
	Buddy buddy = action->context()->buddies().toBuddy();
	if( ! buddy )
		return;
	// set
	setNamedayRemind( buddy, RemindTimeTheDay );
	// update the main action
	updateActionsNameday();
}


void NExtInfo::actionNamedayNextYearTriggered()
{
	// main action
	QAction *qaction = dynamic_cast<QAction*>( sender() );
	if( ! qaction )
		return;
	Action *action = dynamic_cast<Action*>( qaction->data().value<QObject*>() );
	if( ! action )
		return;
	// buddy
	Buddy buddy = action->context()->buddies().toBuddy();
	if( ! buddy )
		return;
	// set
	setNamedayRemind( buddy, RemindTimeNextYear );
	// update the main action
	updateActionsNameday();
}


void NExtInfo::notifyBirthdayNameday()
{
	// for every contact
	foreach( Buddy buddy, BuddyManager::instance()->items() )
	{
		if( buddy.isAnonymous() )
			continue;
		QPair< bool, QPair<int,int> > checkdata;
		// check birthday
		checkdata = checkBirthdayNotify( buddy );
		if( checkdata.first )
		{
			if( checkBirthdayRemind( buddy ) )
			{
				// notify
				Notification *notification = new Notification( "NExtInfo", KaduIcon( "external_modules/nextinfo-birthday" ) );
				notification->setTitle( qApp->translate( "@nextinfo", "Birthday notification" ) );
				QString string; // 9 formats: %1 has {their|her|his} %2 birthday {today|tomorrow|in %3 days}
				string += "<b>%1</b> has ";
				if( buddy.gender() == 0 )
					string += "their";
				else if( buddy.gender() == 1 )
					string += "her";
				else if( buddy.gender() == 2 )
					string += "his";
				string += " <b>%2 birthday</b> ";
				if( checkdata.second.first == 0 )
					string += "<b>today</b>";
				else if( checkdata.second.first == 1 )
					string += "<b>tomorrow</b>";
				else
					string += "in <b>%3 days</b>";
				QString notificationtext = qApp->translate( "@nextinfo", string.toUtf8().data() );
				notificationtext = notificationtext.arg( buddy.display() );
				notificationtext = notificationtext.arg( ordinal( qApp->translate( "@nextinfo", "code(n-th birthday)" ), checkdata.second.second ) );
				if( checkdata.second.first >= 2 )
					notificationtext = notificationtext.arg( QString().setNum( checkdata.second.first ) );
				notification->setText( notificationtext );
				NotificationManager::instance()->notify( notification );
			}
		}
		// check name day
		checkdata = checkNamedayNotify( buddy );
		if( checkdata.first )
		{
			if( checkNamedayRemind( buddy ) )
			{
				// notify
				Notification *notification = new Notification( "NExtInfo", KaduIcon( "external_modules/nextinfo-nameday" ) );
				notification->setTitle( qApp->translate( "@nextinfo", "Name-day notification" ) );
				QString string; // 9 formats: %1 has {their|her|his} name-day {today|tomorrow|in %2 days}
				string += "<b>%1</b> has ";
				if( buddy.gender() == 0 )
					string += "their";
				else if( buddy.gender() == 1 )
					string += "her";
				else if( buddy.gender() == 2 )
					string += "his";
				string += " <b>name-day</b> ";
				if( checkdata.second.first == 0 )
					string += "<b>today</b>";
				else if( checkdata.second.first == 1 )
					string += "<b>tomorrow</b>";
				else
					string += "in <b>%2 days</b>";
				QString notificationtext = qApp->translate( "@nextinfo", string.toUtf8().data() );
				QString age = checkdata.second.second >= 0 ? QString(" (%1)").arg(checkdata.second.second) : "";
				notificationtext = notificationtext.arg( buddy.display() + age );
				if( checkdata.second.first >= 2 )
					notificationtext = notificationtext.arg( QString().setNum( checkdata.second.first ) );
				notification->setText( notificationtext );
				NotificationManager::instance()->notify( notification );
			}
		}
	}
}
#ifdef ADDITIONAL_TRANSLATIONS_FOR_LUPDATE
	QT_TRANSLATE_NOOP( "@nextinfo", "<b>%1</b> has their <b>%2 birthday</b> <b>today</b>"      );
	QT_TRANSLATE_NOOP( "@nextinfo", "<b>%1</b> has her <b>%2 birthday</b> <b>today</b>"        );
	QT_TRANSLATE_NOOP( "@nextinfo", "<b>%1</b> has his <b>%2 birthday</b> <b>today</b>"        );
	QT_TRANSLATE_NOOP( "@nextinfo", "<b>%1</b> has their <b>%2 birthday</b> <b>tomorrow</b>"   );
	QT_TRANSLATE_NOOP( "@nextinfo", "<b>%1</b> has her <b>%2 birthday</b> <b>tomorrow</b>"     );
	QT_TRANSLATE_NOOP( "@nextinfo", "<b>%1</b> has his <b>%2 birthday</b> <b>tomorrow</b>"     );
	QT_TRANSLATE_NOOP( "@nextinfo", "<b>%1</b> has their <b>%2 birthday</b> in <b>%3 days</b>" );
	QT_TRANSLATE_NOOP( "@nextinfo", "<b>%1</b> has her <b>%2 birthday</b> in <b>%3 days</b>"   );
	QT_TRANSLATE_NOOP( "@nextinfo", "<b>%1</b> has his <b>%2 birthday</b> in <b>%3 days</b>"   );
	QT_TRANSLATE_NOOP( "@nextinfo", "<b>%1</b> has their <b>name-day</b> <b>today</b>"         );
	QT_TRANSLATE_NOOP( "@nextinfo", "<b>%1</b> has her <b>name-day</b> <b>today</b>"           );
	QT_TRANSLATE_NOOP( "@nextinfo", "<b>%1</b> has his <b>name-day</b> <b>today</b>"           );
	QT_TRANSLATE_NOOP( "@nextinfo", "<b>%1</b> has their <b>name-day</b> <b>tomorrow</b>"      );
	QT_TRANSLATE_NOOP( "@nextinfo", "<b>%1</b> has her <b>name-day</b> <b>tomorrow</b>"        );
	QT_TRANSLATE_NOOP( "@nextinfo", "<b>%1</b> has his <b>name-day</b> <b>tomorrow</b>"        );
	QT_TRANSLATE_NOOP( "@nextinfo", "<b>%1</b> has their <b>name-day</b> in <b>%2 days</b>"    );
	QT_TRANSLATE_NOOP( "@nextinfo", "<b>%1</b> has her <b>name-day</b> in <b>%2 days</b>"      );
	QT_TRANSLATE_NOOP( "@nextinfo", "<b>%1</b> has his <b>name-day</b> in <b>%2 days</b>"      );
#endif


void NExtInfo::buddyDataWindowCreated( BuddyDataWindow *buddydatawindow )
{
	new ExtendedInformationWidgets( buddydatawindow );
}


void NExtInfo::buddyDataWindowDestroyed( BuddyDataWindow *buddydatawindow )
{
	Q_UNUSED( buddydatawindow );
}


void NExtInfo::importDataFromExtInfo()
{
	int imported = 0;
	// QFileDialog
	QString extinfopath = QFileDialog::getOpenFileName( NULL, qApp->translate( "@nextinfo", "Select ext_info data file to import" ), QDir::homePath() );
	if( ! extinfopath.isNull() )
	{
		if( extinfopath != "" )
		{
			QStringList lines;
			// QFile
			QFile file( extinfopath );
			if( file.open( QIODevice::ReadOnly ) )
			{
				QTextStream stream( &file );
				stream.setCodec( QTextCodec::codecForName("CP1250") );
				QString line;
				Buddy buddy;
				QString field, value;
				while( ! stream.atEnd() )
				{
					// read next line
					line = stream.readLine();
					if( ! line.isEmpty() )  // if the line is not empty
					{
						if( line.contains( QRegExp( "^\\[.*\\]$" ) ) )  // user header (display)
						{
							// get buddy display from file's line
							QString display = line.mid( 1, line.length()-2 );
							// search for such buddy
							buddy = BuddyManager::instance()->byDisplay( display, ActionReturnNull );
							if( ! buddy.isNull() )
								imported++;
						}
						else
						{
							if( ( ! buddy.isNull() ) && ( line.contains( QRegExp( "^[^=]+=[^=]+$" ) ) ) )
							{
								field = line.section( "=", 0, 0 );
								value = line.section( "=", 1, 1 );
								// first name
								if( ! value.isEmpty() )
								{
									QString notes = "";
									if( field == "DrugGG" )
									{
										QVector<Account> gaduaccounts = AccountManager::instance()->byProtocolName( "gadu" );
										if( gaduaccounts.count() > 0 )
										{
											Account account = gaduaccounts.first();
											QString id = value.trimmed();
											Contact contact = ContactManager::instance()->byId( account, id, ActionCreateAndAdd );
											contact.setOwnerBuddy( buddy );
											ContactManager::instance()->addItem( contact );
										}
									}
									else if( field == "Imie" )
									{
										if( buddy.firstName().isEmpty() )
											buddy.setFirstName( value );
										else
											notes += qApp->translate( "@nextinfo", "First name" ) + QString( ": %1\n" ).arg( value );
									}
									else if( field == "Nazwisko" )
									{
										if( buddy.lastName().isEmpty() )
											buddy.setLastName( value );
										else
											notes += qApp->translate( "@nextinfo", "Last name" ) + QString( ": %1\n" ).arg( value );
									}
									else if( field == "Pseudo" )
									{
										if( buddy.nickName().isEmpty() )
											buddy.setNickName( value );
										else
											notes += qApp->translate( "@nextinfo", "Nickname" ) + QString( ": %1\n" ).arg( value );
									}
									else if( field == "TelKom" )
									{
										if( buddy.mobile().isEmpty() )
											buddy.setMobile( value );
										else
											notes += qApp->translate( "@nextinfo", "Mobile" ) + QString( ": %1\n" ).arg( value );
									}
									else if( field == "Email1" )
									{
										if( buddy.email().isEmpty() )
											buddy.setEmail( value );
										else
											notes += qApp->translate( "@nextinfo", "E-Mail" ) + QString( ": %1\n" ).arg( value );
									}
									else if( field == "AdrUl" )
									{
										if( BuddyNExtInfoData::address( buddy ).isEmpty() )
											BuddyNExtInfoData::setAddress( buddy, value );
										else
											notes += qApp->translate( "@nextinfo", "Address" ) + QString( ": %1\n" ).arg( value );
									}
									else if( field == "AdrMiasto" )
									{
										if( BuddyNExtInfoData::city( buddy ).isEmpty() )
											BuddyNExtInfoData::setCity( buddy, value );
										else
											notes += qApp->translate( "@nextinfo", "City" ) + QString( ": %1\n" ).arg( value );
									}
									else if( field == "TelDom" )
									{
										if( buddy.homePhone().isEmpty() )
											buddy.setHomePhone( value );
										else
											notes += qApp->translate( "@nextinfo", "Phone" ) + QString( ": %1\n" ).arg( value );
									}
									else if( field == "Email2" )
									{
										if( BuddyNExtInfoData::email2( buddy ).isEmpty() )
											BuddyNExtInfoData::setEmail2( buddy, value );
									}
									else if( field == "WWW" )
									{
										if( buddy.website().isEmpty() )
											buddy.setWebsite( value );
										else
											notes += qApp->translate( "@nextinfo", "Website" ) + QString( ": %1\n" ).arg( value );
									}
									else if( field == "IrcNick" )
									{
										notes += qApp->translate( "@nextinfo", "IRC" ) + QString( ": %1\n" ).arg( value );
									}
									else if( field == "TlenN" )
									{
										notes += qApp->translate( "@nextinfo", "Tlen" ) + QString( ": %1\n" ).arg( value );
									}
									else if( field == "WP" )
									{
										notes += qApp->translate( "@nextinfo", "WP" ) + QString( ": %1\n" ).arg( value );
									}
									else if( field == "ICQ" )
									{
										notes += qApp->translate( "@nextinfo", "ICQ" ) + QString( ": %1\n" ).arg( value );
									}
									else if( field == "DataUr"    )
									{
										if( BuddyNExtInfoData::birthday( buddy ).isEmpty() )
											BuddyNExtInfoData::setBirthday( buddy, value );
										else
											notes += qApp->translate( "@nextinfo", "Birthday" ) + QString( ": %1\n" ).arg( value );
									}
									else if( field == "DataImien" )
									{
										if( BuddyNExtInfoData::nameday( buddy ).isEmpty() )
											BuddyNExtInfoData::setNameday( buddy, value );
										else
											notes += qApp->translate( "@nextinfo", "Name-day" ) + QString( ": %1\n" ).arg( value );
									}
									else if( field == "EdtZainter" )
									{
										QString text = "";
										if( BuddyNExtInfoData::interests( buddy ).isEmpty() )
											text += BuddyNExtInfoData::interests( buddy ) + "\n\n";
										text += value + "\n";
										BuddyNExtInfoData::setInterests( buddy, text );
									}
									else if( field == "MemoEx" )
									{
										QString text = "";
										if( ! BuddyNExtInfoData::notes( buddy ).isEmpty() )
											text += BuddyNExtInfoData::notes( buddy ) + "\n\n";
										text += value + "\n";
										BuddyNExtInfoData::setNotes( buddy, text );
									}
									else if( field == "Zdjecie" )
									{
										if( ! buddy.buddyAvatar() )
										{
											QPixmap pixmap( value );
											if( ! pixmap.isNull() )
											{
												Avatar avatar = Avatar::create();
												AvatarManager::instance()->addItem( avatar );
												avatar.setPixmap( pixmap );
												buddy.setBuddyAvatar( avatar );
											}
										}
									}
									if( ! notes.isEmpty() )
									{
										notes = BuddyNExtInfoData::notes( buddy ) + "\n\n" + notes;
										BuddyNExtInfoData::setNotes( buddy, notes );
									}
								}
							}
						}
					}
				}
				file.close();
			}
		}
		QMessageBox *messagebox = new QMessageBox(
				QMessageBox::Information,
				qApp->translate( "@nextinfo", "Kadu" ) + " - " + qApp->translate( "@nextinfo", "Extended information" ) + " - " + qApp->translate( "@nextinfo", "Data import" ),
				qApp->translate( "@nextinfo", "%n contact(s) imported.", 0, QApplication::CodecForTr, imported ),
				QMessageBox::Ok
			);
		messagebox->show();
	}
}


void NExtInfo::importOldData( int fromversion )
{
	if( fromversion <= 1 )
	{
		foreach( Buddy buddy, BuddyManager::instance()->items() )
		{
			if( buddy.isAnonymous() )
				continue;
			// additional notes
			QString notes = "";
			// nextinfo_gg2 -> Contact()
			if( ! buddy.customData( "nextinfo_gg2" ).isEmpty() )
			{
				QVector<Account> gaduaccounts = AccountManager::instance()->byProtocolName( "gadu" );
				if( gaduaccounts.count() > 0 )
				{
					Account account = gaduaccounts.first();
					QString id = buddy.customData( "nextinfo_gg2" ).trimmed();
					Contact contact = ContactManager::instance()->byId( account, id, ActionCreateAndAdd );
					contact.setOwnerBuddy( buddy );
					ContactManager::instance()->addItem( contact );
				}
			}
			buddy.removeCustomData( "nextinfo_gg2" );
			// nextinfo_sex -> ::gender
			if( ! buddy.customData( "nextinfo_sex" ).isEmpty() )
			{
				if( buddy.gender() == GenderUnknown )
					buddy.setGender( (BuddyGender)buddy.customData( "nextinfo_sex" ).toInt() );
			}
			buddy.removeCustomData( "nextinfo_sex" );
			// nextinfo_phone2 -> ::phone
			if( ! buddy.customData( "nextinfo_phone2" ).isEmpty() )
			{
				if( buddy.homePhone().isEmpty() )
					buddy.setHomePhone( buddy.customData( "nextinfo_phone2" ) );
				else
					notes += qApp->translate( "@nextinfo", "Phone" ) + QString( ": %1\n" ).arg( buddy.customData( "nextinfo_phone2" ) );
			}
			buddy.removeCustomData( "nextinfo_phone2" );
			// nextinfo_www -> ::website
			if( ! buddy.customData( "nextinfo_www" ).isEmpty() )
			{
				if( buddy.website().isEmpty() )
					buddy.setWebsite( buddy.customData( "nextinfo_www" ) );
				else
					notes += qApp->translate( "@nextinfo", "Website" ) + QString( ": %1\n" ).arg( buddy.customData( "nextinfo_www" ) );
			}
			buddy.removeCustomData( "nextinfo_www" );
			// nextinfo_address -> data::address
			if( ! buddy.customData( "nextinfo_address" ).isEmpty() )
			{
				BuddyNExtInfoData::setAddress( buddy, buddy.customData( "nextinfo_address" ) );
			}
			buddy.removeCustomData( "nextinfo_address" );
			// nextinfo_city -> data::city
			if( ! buddy.customData( "nextinfo_city" ).isEmpty() )
			{
				BuddyNExtInfoData::setCity( buddy, buddy.customData( "nextinfo_city" ) );
			}
			buddy.removeCustomData( "nextinfo_city" );
			// nextinfo_email2 -> data::email2
			if( ! buddy.customData( "nextinfo_email2" ).isEmpty() )
			{
				BuddyNExtInfoData::setEmail2( buddy, buddy.customData( "nextinfo_email2" ) );
			}
			buddy.removeCustomData( "nextinfo_email2" );
			// nextinfo_birthday -> data::birthday
			if( ! buddy.customData( "nextinfo_birthday" ).isEmpty() )
			{
				BuddyNExtInfoData::setBirthday( buddy, buddy.customData( "nextinfo_birthday" ) );
			}
			buddy.removeCustomData( "nextinfo_birthday" );
			// nextinfo_nameday -> data::nameday
			if( ! buddy.customData( "nextinfo_nameday" ).isEmpty() )
			{
				BuddyNExtInfoData::setNameday( buddy, buddy.customData( "nextinfo_nameday" ) );
			}
			buddy.removeCustomData( "nextinfo_nameday" );
			// nextinfo_interests -> data::interests
			if( ! buddy.customData( "nextinfo_interests" ).isEmpty() )
			{
				BuddyNExtInfoData::setInterests( buddy, buddy.customData( "nextinfo_interests" ).replace( "\\n", "\n" ).replace( "\\\\", "\\" ) );
			}
			buddy.removeCustomData( "nextinfo_interests" );
			// nextinfo_notes -> data::notes
			if( ! buddy.customData( "nextinfo_notes" ).isEmpty() )
			{
				BuddyNExtInfoData::setNotes( buddy, buddy.customData( "nextinfo_notes" ).replace( "\\n", "\n" ).replace( "\\\\", "\\" ) );
			}
			buddy.removeCustomData( "nextinfo_notes" );
			// nextinfo_irc >> data::notes
			if( ! buddy.customData( "nextinfo_irc" ).isEmpty() )
				notes += qApp->translate( "@nextinfo", "IRC" ) + QString( ": %1\n" ).arg( buddy.customData( "nextinfo_irc" ) );
			buddy.removeCustomData( "nextinfo_irc" );
			// nextinfo_tlen >> data::notes
			if( ! buddy.customData( "nextinfo_tlen" ).isEmpty() )
				notes += qApp->translate( "@nextinfo", "Tlen" ) + QString( ": %1\n" ).arg( buddy.customData( "nextinfo_tlen" ) );
			buddy.removeCustomData( "nextinfo_tlen" );
			// nextinfo_wp >> data::notes
			if( ! buddy.customData( "nextinfo_wp" ).isEmpty() )
				notes += qApp->translate( "@nextinfo", "WP" ) + QString( ": %1\n" ).arg( buddy.customData( "nextinfo_wp" ) );
			buddy.removeCustomData( "nextinfo_wp" );
			// nextinfo_icq >> data::notes
			if( ! buddy.customData( "nextinfo_icq" ).isEmpty() )
				notes += qApp->translate( "@nextinfo", "ICQ" ) + QString( ": %1\n" ).arg( buddy.customData( "nextinfo_icq" ) );
			buddy.removeCustomData( "nextinfo_icq" );
			// nextinfo_photo -> ::avatar
			if( ! buddy.customData( "nextinfo_photo" ).isEmpty() )
				if( ! buddy.buddyAvatar() )
				{
					QPixmap pixmap( buddy.customData( "nextinfo_photo" ) );
					if( ! pixmap.isNull() )
					{
						Avatar avatar = Avatar::create();
						AvatarManager::instance()->addItem( avatar );
						avatar.setPixmap( pixmap );
						buddy.setBuddyAvatar( avatar );
					}
				}
			buddy.removeCustomData( "nextinfo_photo" );
			// insert notes
			BuddyNExtInfoData::setNotes( buddy, BuddyNExtInfoData::notes( buddy ) + "\n\n" + notes );
		}
	}
}


void NExtInfo::showHelp()
{
	QString helpmessage =
		qApp->translate( "@nextinfo", "These tags are recognised by the Kadu's parser:" ) + "\n" +
		"#{nextinfo_middleName}" + "\n" +
		"#{nextinfo_address}"    + "\n" +
		"#{nextinfo_city}"       + "\n" +
		"#{nextinfo_email2}"     + "\n" +
		"#{nextinfo_birthday}"   + "\n" +
		"#{nextinfo_nameday}"    + "\n" +
		"#{nextinfo_interests}"  + "\n" +
		"#{nextinfo_notes}"      + "\n" +
		"\n";
	QMessageBox *messagebox = new QMessageBox(
			qApp->translate( "@nextinfo", "Kadu" ) + " - " + qApp->translate( "@nextinfo", "Extended information" ) + " - " + qApp->translate( "@nextinfo", "Help" ),
			helpmessage,
			QMessageBox::Information,
			QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton,
			qApp->activeWindow()
		);
	messagebox->show();
}


QString NExtInfo::ordinal( QString code, int n )
{
	QScriptEngine se;
	se.evaluate( QString( "function ordinal( n ) { %1 }" ).arg( code ) );
	QScriptValue result = se.evaluate( QString( "ordinal( %1 )" ).arg( n ) );
	if( se.hasUncaughtException() )
		return QString::number( n ) + ".";
	return result.toString();
}




Q_EXPORT_PLUGIN2( nextinfo, NExtInfo )
