/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/




#include <QApplication>
#include <QFileDialog>
#include <QList>
#include <QMessageBox>
#include <QPair>
#include <QRegExp>
#include <QTextCodec>
#include <QTextStream>

#include "config_file.h"
#include "debug.h"
#include "kadu.h"

#include "action.h"
#include "activate.h"
#include "chat_edit_box.h"
#include "kadu_parser.h"

#include "../notify/notify.h"

#include "nextinfo.h"
#include "extendedinformationwindow.h"

#include "gettags.h"
#include "defines_stringencoder.h"
NEXTINFO_ENCODESTRING




NExtInfo *nextinfo;




extern "C" int nextinfo_init( bool firstLoad )
{
	kdebugf();
	nextinfo = new NExtInfo( firstLoad );
	MainConfigurationWindow::registerUiFile( dataPath("kadu/modules/configuration/nextinfo.ui"), nextinfo );
	kdebugf2();
	return 0;
}


extern "C" void nextinfo_close()
{
	kdebugf();
	MainConfigurationWindow::unregisterUiFile( dataPath("kadu/modules/configuration/nextinfo.ui"), nextinfo );
	delete nextinfo;
	nextinfo = NULL;
	kdebugf2();
}




void action_extinfo( KaduAction *action )
{
	action->setEnabled( action->userListElements().count() == 1 );
}


void action_birthdaydontinform( KaduAction *action )
{
	if( action->userListElements().count() != 1 )
	{
		action->setVisible( false );
		return;
	}
	UserListElement user = action->userListElements().first();
	QPair< bool, QPair<int,int> > checkdata;
	checkdata = nextinfo->checkBirthday( user );
	if( checkdata.first != 0 )
	{
		if( user.data( "nextinfo_birthdayknown" ).toString().toInt() != QDate::currentDate().addDays( checkdata.second.first ).year() )
			action->setVisible( true );
		else
			action->setVisible( false );
	}
	else
	{
		action->setVisible( false );
	}
}


void action_birthdayinform( KaduAction *action )
{
	if( action->userListElements().count() != 1 )
	{
		action->setVisible( false );
		return;
	}
	UserListElement user = action->userListElements().first();
	QPair< bool, QPair<int,int> > checkdata;
	checkdata = nextinfo->checkBirthday( user );
	if( checkdata.first != 0 )
	{
		if( user.data( "nextinfo_birthdayknown" ).toString().toInt() != QDate::currentDate().addDays( checkdata.second.first ).year() )
			action->setVisible( false );
		else
			action->setVisible( true );
	}
	else
	{
		action->setVisible( false );
	}
}


void action_namedaydontinform( KaduAction *action )
{
	if( action->userListElements().count() != 1 )
	{
		action->setVisible( false );
		return;
	}
	UserListElement user = action->userListElements().first();
	QPair< bool, QPair<int,int> > checkdata;
	checkdata = nextinfo->checkNameday( user );
	if( checkdata.first != 0 )
	{
		if( user.data( "nextinfo_namedayknown" ).toString().toInt() != QDate::currentDate().addDays( checkdata.second.first ).year() )
			action->setVisible( true );
		else
			action->setVisible( false );
	}
	else
	{
		action->setVisible( false );
	}
}


void action_namedayinform( KaduAction *action )
{
	if( action->userListElements().count() != 1 )
	{
		action->setVisible( false );
		return;
	}
	UserListElement user = action->userListElements().first();
	QPair< bool, QPair<int,int> > checkdata;
	checkdata = nextinfo->checkNameday( user );
	if( checkdata.first != 0 )
	{
		if( user.data( "nextinfo_namedayknown" ).toString().toInt() != QDate::currentDate().addDays( checkdata.second.first ).year() )
			action->setVisible( false );
		else
			action->setVisible( true );
	}
	else
	{
		action->setVisible( false );
	}
}


NExtInfo::NExtInfo( bool firstLoad ) : QObject( NULL, "nextinfo" )
{
	// create default configuration if needed
	createDefaultConfiguration();
	// create birthdaynamedaytimer QTimer and connect it
	birthdaynamedaytimer = new QTimer();
	connect( birthdaynamedaytimer, SIGNAL(timeout()), this, SLOT(notifyBirthdayNameday()) );
	// read the configuration and force its usage
	configurationUpdated();
	// add extended information fields
	userlist->addPerContactNonProtocolConfigEntry( "nextinfo_address"       , "nextinfo_address"       );
	userlist->addPerContactNonProtocolConfigEntry( "nextinfo_city"          , "nextinfo_city"          );
	userlist->addPerContactNonProtocolConfigEntry( "nextinfo_sex"           , "nextinfo_sex"           );
	userlist->addPerContactNonProtocolConfigEntry( "nextinfo_phone2"        , "nextinfo_phone2"        );
	userlist->addPerContactNonProtocolConfigEntry( "nextinfo_email2"        , "nextinfo_email2"        );
	userlist->addPerContactNonProtocolConfigEntry( "nextinfo_www"           , "nextinfo_www"           );
	userlist->addPerContactNonProtocolConfigEntry( "nextinfo_gg2"           , "nextinfo_gg2"           );
	userlist->addPerContactNonProtocolConfigEntry( "nextinfo_irc"           , "nextinfo_irc"           );
	userlist->addPerContactNonProtocolConfigEntry( "nextinfo_tlen"          , "nextinfo_tlen"          );
	userlist->addPerContactNonProtocolConfigEntry( "nextinfo_wp"            , "nextinfo_wp"            );
	userlist->addPerContactNonProtocolConfigEntry( "nextinfo_icq"           , "nextinfo_icq"           );
	userlist->addPerContactNonProtocolConfigEntry( "nextinfo_birthday"      , "nextinfo_birthday"      );
	userlist->addPerContactNonProtocolConfigEntry( "nextinfo_birthdayknown" , "nextinfo_birthdayknown" );
	userlist->addPerContactNonProtocolConfigEntry( "nextinfo_nameday"       , "nextinfo_nameday"       );
	userlist->addPerContactNonProtocolConfigEntry( "nextinfo_namedayknown"  , "nextinfo_namedayknown"  );
	userlist->addPerContactNonProtocolConfigEntry( "nextinfo_interests"     , "nextinfo_interests"     );
	userlist->addPerContactNonProtocolConfigEntry( "nextinfo_notes"         , "nextinfo_notes"         );
	userlist->addPerContactNonProtocolConfigEntry( "nextinfo_photo"         , "nextinfo_photo"         );
	// register parser tags
	KaduParser::registerTag( "nextinfo_address"   , getTag_address   );
	KaduParser::registerTag( "nextinfo_city"      , getTag_city      );
	KaduParser::registerTag( "nextinfo_sex"       , getTag_sex       );
	KaduParser::registerTag( "nextinfo_sexN"      , getTag_sexN      );
	KaduParser::registerTag( "nextinfo_phone2"    , getTag_phone2    );
	KaduParser::registerTag( "nextinfo_email2"    , getTag_email2    );
	KaduParser::registerTag( "nextinfo_www"       , getTag_www       );
	KaduParser::registerTag( "nextinfo_gg2"       , getTag_gg2       );
	KaduParser::registerTag( "nextinfo_irc"       , getTag_irc       );
	KaduParser::registerTag( "nextinfo_tlen"      , getTag_tlen      );
	KaduParser::registerTag( "nextinfo_wp"        , getTag_wp        );
	KaduParser::registerTag( "nextinfo_icq"       , getTag_icq       );
	KaduParser::registerTag( "nextinfo_birthday"  , getTag_birthday  );
	KaduParser::registerTag( "nextinfo_nameday"   , getTag_nameday   );
	KaduParser::registerTag( "nextinfo_interests" , getTag_interests );
	KaduParser::registerTag( "nextinfo_notes"     , getTag_notes     );
	KaduParser::registerTag( "nextinfo_photo"     , getTag_photo     );
	KaduParser::registerTag( "nextinfo_photoimg1" , getTag_photoimg1 );
	KaduParser::registerTag( "nextinfo_photoimg2" , getTag_photoimg2 );
	KaduParser::registerTag( "nextinfo_photoimg3" , getTag_photoimg3 );
	// add NExtInfo items to UserBox context menu
	popupmenu_item_extinfo = new ActionDescription(
		ActionDescription::TypeUser, "popupmenu_item_extinfo"           , this, SLOT(showExtendedInformationWindow(QAction*,bool)), "EditUserInfo"  , qApp->translate( "@nextinfo", "Extended information"               ), false, "", action_extinfo );
		UserBox::addActionDescription( popupmenu_item_extinfo );
	popupmenu_item_birthdaydontinform = new ActionDescription(
		ActionDescription::TypeUser, "popupmenu_item_birthdaydontinform", this, SLOT(dontInformAboutTheBirthday(QAction*,bool))   , "OkWindowButton", qApp->translate( "@nextinfo", "Don't inform me about the birthday" ), false, "", action_birthdaydontinform );
		UserBox::addActionDescription( popupmenu_item_birthdaydontinform );
	popupmenu_item_birthdayinform = new ActionDescription(
		ActionDescription::TypeUser, "popupmenu_item_birthdayinform"    , this, SLOT(informAboutTheBirthday(QAction*,bool))       , "OkWindowButton", qApp->translate( "@nextinfo", "Inform me about the birthday"       ), false, "", action_birthdayinform );
		UserBox::addActionDescription( popupmenu_item_birthdayinform );
	popupmenu_item_namedaydontinform = new ActionDescription(
		ActionDescription::TypeUser, "popupmenu_item_namedaydontinform" , this, SLOT(dontInformAboutTheNameday(QAction*,bool))    , "OkWindowButton", qApp->translate( "@nextinfo", "Don't inform me about the name-day" ), false, "", action_namedaydontinform );
		UserBox::addActionDescription( popupmenu_item_namedaydontinform );
	popupmenu_item_namedayinform = new ActionDescription(
		ActionDescription::TypeUser, "popupmenu_item_namedayinform"     , this, SLOT(informAboutTheNameday(QAction*,bool))        , "OkWindowButton", qApp->translate( "@nextinfo", "Inform me about the name-day"       ), false, "", action_namedayinform );
		UserBox::addActionDescription( popupmenu_item_namedayinform );
	UserBox::addSeparator();
	// create toolbar button
	nextinfoaction = new ActionDescription(
		ActionDescription::TypeChat, "nextinfoaction", this, SLOT( showExtendedInformationWindow(QAction*,bool) ), "EditUserInfo", qApp->translate( "@nextinfo", "Extended information" ) );
	// register the notification
	notification_manager->registerEvent( "nextinfo/notification", QT_TRANSLATE_NOOP( "@nextinfo", "Birthday/name-day notification (nextinfo)" ), CallbackNotRequired );
	// check birthdays and name-days at startup
	notifyBirthdayNameday();
	// start the birthdaynamedaytimer timer
	birthdaynamedaytimer->start( notifyInterval * 1000 );
}


NExtInfo::~NExtInfo()
{
	// stop the birthdaynamedaytimer timer
	birthdaynamedaytimer->stop();
	// delete all Extended Information Windows
	for(
	     QList< QPointer<ExtendedInformationWindow> >::iterator window = extendedinformationwindows.begin();
	     window != extendedinformationwindows.end();
	     ++window
	   )
	{
		if( ! (*window).isNull() )
		{
			delete (*window);
		}
	}
	// unregister the notification
	notification_manager->unregisterEvent( "nextinfo/notification" );
	// remove toolbar button
	delete nextinfoaction;
	// remove NExtInfo items from UserBox context menu
	UserBox::removeActionDescription( popupmenu_item_extinfo            );
	UserBox::removeActionDescription( popupmenu_item_birthdaydontinform );
	UserBox::removeActionDescription( popupmenu_item_birthdayinform     );
	UserBox::removeActionDescription( popupmenu_item_namedaydontinform  );
	UserBox::removeActionDescription( popupmenu_item_namedayinform      );
	// unregister parser tags
	KaduParser::unregisterTag( "nextinfo_address"   , getTag_address   );
	KaduParser::unregisterTag( "nextinfo_city"      , getTag_city      );
	KaduParser::unregisterTag( "nextinfo_sex"       , getTag_sex       );
	KaduParser::unregisterTag( "nextinfo_sexN"      , getTag_sexN      );
	KaduParser::unregisterTag( "nextinfo_phone2"    , getTag_phone2    );
	KaduParser::unregisterTag( "nextinfo_email2"    , getTag_email2    );
	KaduParser::unregisterTag( "nextinfo_www"       , getTag_www       );
	KaduParser::unregisterTag( "nextinfo_gg2"       , getTag_gg2       );
	KaduParser::unregisterTag( "nextinfo_irc"       , getTag_irc       );
	KaduParser::unregisterTag( "nextinfo_tlen"      , getTag_tlen      );
	KaduParser::unregisterTag( "nextinfo_wp"        , getTag_wp        );
	KaduParser::unregisterTag( "nextinfo_icq"       , getTag_icq       );
	KaduParser::unregisterTag( "nextinfo_birthday"  , getTag_birthday  );
	KaduParser::unregisterTag( "nextinfo_nameday"   , getTag_nameday   );
	KaduParser::unregisterTag( "nextinfo_interests" , getTag_interests );
	KaduParser::unregisterTag( "nextinfo_notes"     , getTag_notes     );
	KaduParser::unregisterTag( "nextinfo_photo"     , getTag_photo     );
	KaduParser::unregisterTag( "nextinfo_photoimg1" , getTag_photoimg1 );
	KaduParser::unregisterTag( "nextinfo_photoimg2" , getTag_photoimg2 );
	KaduParser::unregisterTag( "nextinfo_photoimg3" , getTag_photoimg3 );
}


void NExtInfo::mainConfigurationWindowCreated( MainConfigurationWindow *mainConfigurationWindow )
{
	connect( mainConfigurationWindow->widgetById("nextinfo/enableNotifications"), SIGNAL(toggled(bool)), mainConfigurationWindow->widgetById("nextinfo/notifyAboutBirthdays")     , SLOT(setEnabled(bool)) );
	connect( mainConfigurationWindow->widgetById("nextinfo/enableNotifications"), SIGNAL(toggled(bool)), mainConfigurationWindow->widgetById("nextinfo/notifyAboutNamedays")      , SLOT(setEnabled(bool)) );
	connect( mainConfigurationWindow->widgetById("nextinfo/enableNotifications"), SIGNAL(toggled(bool)), mainConfigurationWindow->widgetById("nextinfo/notificationAdvance")      , SLOT(setEnabled(bool)) );
	connect( mainConfigurationWindow->widgetById("nextinfo/enableNotifications"), SIGNAL(toggled(bool)), mainConfigurationWindow->widgetById("nextinfo/delayBetweenNotifications"), SLOT(setEnabled(bool)) );
	connect( mainConfigurationWindow->widgetById("nextinfo/showHelp"), SIGNAL(clicked()), this, SLOT(showHelp()) );
	connect( mainConfigurationWindow->widgetById("nextinfo/importDataFromExtInfo"), SIGNAL(clicked()), this, SLOT(importDataFromExtInfo()) );
}


void NExtInfo::createDefaultConfiguration()
{
	config_file.addVariable( "NExtInfo", "EnableNotifications"      , true                                      );
	config_file.addVariable( "NExtInfo", "NotifyAboutBirthdays"     , true                                      );
	config_file.addVariable( "NExtInfo", "NotifyAboutNamedays"      , true                                      );
	config_file.addVariable( "NExtInfo", "NotificationAdvance"      , NEXTINFO_DEFAULTNOTIFICATIONADVANCE       );
	config_file.addVariable( "NExtInfo", "DelayBetweenNotifications", NEXTINFO_DEFAULTDELAYBETWEENNOTIFICATIONS );
	config_file.addVariable( "NExtInfo", "PhotoSize1Width"          , NEXTINFO_DEFAULTPHOTOSIZE1WIDTH           );
	config_file.addVariable( "NExtInfo", "PhotoSize1Height"         , NEXTINFO_DEFAULTPHOTOSIZE1HEIGHT          );
	config_file.addVariable( "NExtInfo", "PhotoSize2Width"          , NEXTINFO_DEFAULTPHOTOSIZE2WIDTH           );
	config_file.addVariable( "NExtInfo", "PhotoSize2Height"         , NEXTINFO_DEFAULTPHOTOSIZE2HEIGHT          );
	config_file.addVariable( "NExtInfo", "PhotoSize3Width"          , NEXTINFO_DEFAULTPHOTOSIZE3WIDTH           );
	config_file.addVariable( "NExtInfo", "PhotoSize3Height"         , NEXTINFO_DEFAULTPHOTOSIZE3HEIGHT          );
}


void NExtInfo::configurationUpdated()
{
	// update configuration data
	notify               = config_file.readBoolEntry( "NExtInfo", "EnableNotifications"       );
	notifyAboutBirthdays = config_file.readBoolEntry( "NExtInfo", "NotifyAboutBirthdays"      );
	notifyAboutNamedays  = config_file.readBoolEntry( "NExtInfo", "NotifyAboutNamedays"       );
	notifyAdvance        = config_file.readNumEntry(  "NExtInfo", "NotificationAdvance"       );
	notifyInterval       = config_file.readNumEntry(  "NExtInfo", "DelayBetweenNotifications" );
	// restart the birthdaynamedaytimer timer if it is running
	if( birthdaynamedaytimer->isActive() )
	{
		birthdaynamedaytimer->stop();
		birthdaynamedaytimer->start( notifyInterval * 1000 );
	}
}


int NExtInfo::closestYear( int month, int day )
{
	if( ( month < QDate::currentDate().month() ) || ( ( month == QDate::currentDate().month() ) && ( day < QDate::currentDate().day() ) ) )
	{
		return QDate::currentDate().year() + 1;
	}
	return QDate::currentDate().year();
}


QPair< bool, QPair<int,int> > NExtInfo::checkBirthday( UserListElement user )
{
	if( ( ! notify ) || ( ! notifyAboutBirthdays ) )
		return QPair< bool, QPair<int,int> >( false, QPair<int,int>( 0, 0 ) );
	QString birthday = user.data( "nextinfo_birthday"  ).toString();
	if( birthday.find( QRegExp( NEXTINFO_REGEXPBIRTHDAY ) ) == -1 )  // if bad format
		return QPair< bool, QPair<int,int> >( false, QPair<int,int>( 0, 0 ) );
	// get date elemeents
	QStringList dateelements = QStringList::split( '.', birthday );
	int birthday_day   = dateelements.at(0).toInt();
	int birthday_month = dateelements.at(1).toInt();
	int birthday_year  = dateelements.at(2).toInt();
	// count remaining days
	int remainingdays = QDate::currentDate().daysTo( QDate( closestYear( birthday_month, birthday_day ), birthday_month, birthday_day ) );
	// notify?
	if( remainingdays <= notifyAdvance )
	{
		return QPair< bool, QPair<int,int> >( true, QPair<int,int>( remainingdays, QDate::currentDate().year()-birthday_year ) );
	}
	return QPair< bool, QPair<int,int> >( false, QPair<int,int>( 0, 0 ) );
}


QPair< bool, QPair<int,int> > NExtInfo::checkNameday( UserListElement user )
{
	if( ( ! notify ) || ( ! notifyAboutNamedays ) )
		return QPair< bool, QPair<int,int> >( false, QPair<int,int>( 0, 0 ) );
	QString birthday = user.data( "nextinfo_nameday"  ).toString();
	if( birthday.find( QRegExp( NEXTINFO_REGEXPNAMEDAY ) ) == -1 )  // if bad format
		return QPair< bool, QPair<int,int> >( false, QPair<int,int>( 0, 0 ) );
	// get date elemeents
	QStringList dateelements = QStringList::split( '.', birthday );
	int nameday_day   = dateelements.at(0).toInt();
	int nameday_month = dateelements.at(1).toInt();
	// count remaining days
	int remainingdays = QDate::currentDate().daysTo( QDate( closestYear( nameday_month, nameday_day ), nameday_month, nameday_day ) );
	// notify?
	if( remainingdays <= notifyAdvance )
	{
		return QPair< bool, QPair<int,int> >( true, QPair<int,int>( remainingdays, 0 ) );
	}
	return QPair< bool, QPair<int,int> >( false, QPair<int,int>( 0, 0 ) );
}


void NExtInfo::showExtendedInformationWindow( QAction *sender, bool toggled )
{
	KaduAction *kaduAction = (dynamic_cast<KaduAction*>(sender));
	if( ! kaduAction )
		return;
	UserListElements users = kaduAction->userListElements();
	if( users.count() != 1 )
		return;
	// create new ExtendedInformationWindow
	QPointer<ExtendedInformationWindow> extendedinformationwindow = new ExtendedInformationWindow();
	// insert its guarded pointer into extendedinformationwindows QMap
	extendedinformationwindows.push_back( extendedinformationwindow );
	// load user data
	extendedinformationwindow->setUser( users.first() );
	// show and activate window
	extendedinformationwindow->show();
	activateWindow( extendedinformationwindow->winId() );
}


void NExtInfo::notifyBirthdayNameday()
{
	// for every contact
	for(
	     UserList::iterator userlistit = userlist->begin();
	     userlistit != userlist->end();
	     userlistit++
	   )
	{
		QPair< bool, QPair<int,int> > checkdata;
		// check birthday
		checkdata = checkBirthday( *userlistit );
		if( checkdata.first )
		{
			if( (*userlistit).data( "nextinfo_birthdayknown" ).toString().toInt() != QDate::currentDate().addDays( checkdata.second.first ).year() )
			{
				// notify
				Notification *notification = new Notification( "nextinfo/notification", "NEXTINFO", UserListElements() );
				notification->setIcon( "History" );
				notification->setTitle( qApp->translate( "@nextinfo", "Birthday notification" ) );
				QString notificationtext;
				if( checkdata.second.first == 0 )  // today
				{
					notificationtext = qApp->translate( "@nextinfo", "%1% has %2%. birthday today" );
					notificationtext = notificationtext.replace( "%1%", (*userlistit).altNick()                     );
					notificationtext = notificationtext.replace( "%2%", QString().setNum( checkdata.second.second ) );
				}
				else if( checkdata.second.first == 1 )  // tomorrow
				{
					notificationtext = qApp->translate( "@nextinfo", "%1% has %2%. birthday tomorrow" );
					notificationtext = notificationtext.replace( "%1%", (*userlistit).altNick()                     );
					notificationtext = notificationtext.replace( "%2%", QString().setNum( checkdata.second.second ) );
				}
				else
				{
					notificationtext = qApp->translate( "@nextinfo", "%1% has %2%. birthday in %3% days" );
					notificationtext = notificationtext.replace( "%1%", (*userlistit).altNick()                     );
					notificationtext = notificationtext.replace( "%2%", QString().setNum( checkdata.second.second ) );
					notificationtext = notificationtext.replace( "%3%", QString().setNum( checkdata.second.first  ) );
				}
				notification->setText( notificationtext );
				notification_manager->notify( notification );
			}
		}
		// check name day
		checkdata = checkNameday( *userlistit );
		if( checkdata.first )
		{
			if( (*userlistit).data( "nextinfo_namedayknown" ).toString().toInt() != QDate::currentDate().addDays( checkdata.second.first ).year() )
			{
				// notify
				Notification *notification = new Notification( "nextinfo/notification", "NEXTINFO", UserListElements() );
				notification->setIcon( "History" );
				notification->setTitle( qApp->translate( "@nextinfo", "Name-day notification" ) );
				QString notificationtext;
				if( checkdata.second.first == 0 )  // today
				{
					notificationtext = qApp->translate( "@nextinfo", "%1% has name-day today" );
					notificationtext = notificationtext.replace( "%1%", (*userlistit).altNick() );
				}
				else if( checkdata.second.first == 1 )  // tomorrow
				{
					notificationtext = qApp->translate( "@nextinfo", "%1% has name-day tomorrow" );
					notificationtext = notificationtext.replace( "%1%", (*userlistit).altNick() );
				}
				else
				{
					notificationtext = qApp->translate( "@nextinfo", "%1% has name-day in %2% days" );
					notificationtext = notificationtext.replace( "%1%", (*userlistit).altNick()                    );
					notificationtext = notificationtext.replace( "%2%", QString().setNum( checkdata.second.first ) );
				}
				notification->setText( notificationtext );
				notification_manager->notify( notification );
			}
		}
	}
}


void NExtInfo::dontInformAboutTheBirthday( QAction *sender, bool toggled )
{
	KaduAction *kaduAction = (dynamic_cast<KaduAction*>(sender));
	if( ! kaduAction )
		return;
	UserListElements users = kaduAction->userListElements();
	if( users.count() != 1 )
		return;
	UserListElement user = users.first();
	QPair< bool, QPair<int,int> > checkdata;
	checkdata = checkBirthday( user );
	if( checkdata.first != 0 )
	{
		user.setData( "nextinfo_birthdayknown" , QVariant( QString().setNum( QDate::currentDate().addDays( checkdata.second.first ).year() ) ) );
	}
}
void NExtInfo::informAboutTheBirthday( QAction *sender, bool toggled )
{
	KaduAction *kaduAction = (dynamic_cast<KaduAction*>(sender));
	if( ! kaduAction )
		return;
	UserListElements users = kaduAction->userListElements();
	if( users.count() != 1 )
		return;
	UserListElement user = users.first();
	user.setData( "nextinfo_birthdayknown" , QVariant( QString().setNum( -1 ) ) );
}


void NExtInfo::dontInformAboutTheNameday( QAction *sender, bool toggled )
{
	KaduAction *kaduAction = (dynamic_cast<KaduAction*>(sender));
	if( ! kaduAction )
		return;
	UserListElements users = kaduAction->userListElements();
	if( users.count() != 1 )
		return;
	UserListElement user = users.first();
	QPair< bool, QPair<int,int> > checkdata;
	checkdata = checkNameday( user );
	if( checkdata.first != 0 )
	{
		user.setData( "nextinfo_namedayknown" , QVariant( QString().setNum( QDate::currentDate().addDays( checkdata.second.first ).year() ) ) );
	}
}
void NExtInfo::informAboutTheNameday( QAction *sender, bool toggled )
{
	KaduAction *kaduAction = (dynamic_cast<KaduAction*>(sender));
	if( ! kaduAction )
		return;
	UserListElements users = kaduAction->userListElements();
	if( users.count() != 1 )
		return;
	UserListElement user = users.first();
	user.setData( "nextinfo_namedayknown" , QVariant( QString().setNum( -1 ) ) );
}


void NExtInfo::showHelp()
{
	QString helpmessage =
		qApp->translate( "@nextinfo", "These tags are recognised by the Kadu's parser:" ) + "\n" +
		"#{nextinfo_address}"   + "\n" +
		"#{nextinfo_city}"      + "\n" +
		"#{nextinfo_sex}"       + "\n" +
		"#{nextinfo_sexN}"      + "\n" +
		"#{nextinfo_phone2}"    + "\n" +
		"#{nextinfo_email2}"    + "\n" +
		"#{nextinfo_www}"       + "\n" +
		"#{nextinfo_gg2}"       + "\n" +
		"#{nextinfo_irc}"       + "\n" +
		"#{nextinfo_tlen}"      + "\n" +
		"#{nextinfo_wp}"        + "\n" +
		"#{nextinfo_icq}"       + "\n" +
		"#{nextinfo_birthday}"  + "\n" +
		"#{nextinfo_nameday}"   + "\n" +
		"#{nextinfo_interests}" + "\n" +
		"#{nextinfo_notes}"     + "\n" +
		"#{nextinfo_photo}"     + "\n" +
		"#{nextinfo_photoimg1}" + "\n" +
		"#{nextinfo_photoimg2}" + "\n" +
		"#{nextinfo_photoimg3}" + "\n" +
		"\n" +
		qApp->translate( "@nextinfo", "Each photoimg tag inserts an image scaled to given maximum size." ) + "\n" +
		"\n";
	QMessageBox *messagebox = new QMessageBox(
			qApp->translate( "@nextinfo", "Kadu - extended information - help" ),
			helpmessage,
			QMessageBox::Information,
			QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton,
			qApp->activeWindow()
		);
	messagebox->show();
}


void NExtInfo::importDataFromExtInfo()
{
	int imported = 0;
	// QFileDialog
	QString extinfopath = QFileDialog::getOpenFileName( "/home/", QString::null, qApp->activeWindow(), 0, qApp->translate( "@nextinfo", "Select ext_info data file to import" ) );
	if( ! extinfopath.isNull() )
	{
		if( extinfopath != "" )
		{
			QStringList lines;
			// QFile
			QFile file( extinfopath );
			if( file.open( IO_ReadOnly ) )
			{
				QTextStream stream( &file );
				stream.setCodec( QTextCodec::codecForName("CP1250") );
				QString line;
				QString useraltnick;
				UserListElement *user;
				QString field, value;
				while( ! stream.atEnd() )
				{
					// read next line
					line = stream.readLine();
					if( ! line.isEmpty() )  // if the line is not empty
					{
						if( line.find( QRegExp( "^\\[.*\\]$" ) ) != -1 )  // user header (altNick)
						{
							// get user altNick from file's line
							useraltnick = line.mid( 1, line.length()-2 );
							// search for such user
							user = NULL;
							for(
									UserList::iterator userlistit = userlist->begin();
									userlistit != userlist->end();
									userlistit++
								)
							{
								if( (*userlistit).altNick() == useraltnick )
								{
									user = new UserListElement( *userlistit );
									imported++;
									break;
								}
							}
						}
						else
						{
							if( ( user != NULL ) && ( line.find( QRegExp( "^[^=]+=[^=]+$" ) ) != -1 ) )
							{
								field = line.section( "=", 0, 0 );
								value = line.section( "=", 1, 1 );
								// save simple information
								if(      ( field == "Imie"     ) && ( user->firstName() == "" ) ) { user->setData( "FirstName", QVariant( value ), false, true ); }
								else if( ( field == "Nazwisko" ) && ( user->lastName()  == "" ) ) { user->setData( "LastName" , QVariant( value ), false, true ); }
								else if( ( field == "Pseudo"   ) && ( user->nickName()  == "" ) ) { user->setData( "NickName" , QVariant( value ), false, true ); }
								else if( ( field == "TelKom"   ) && ( user->mobile()    == "" ) ) { user->setData( "Mobile"   , QVariant( value ), false, true ); }
								else if( ( field == "Email1"   ) && ( user->email()     == "" ) ) { user->setData( "Email"    , QVariant( value ), false, true ); }
								// save extended information
								else if( ( field == "AdrUl"     ) && ( user->data( "nextinfo_address"  ).toString() == "" ) ) { user->setData( "nextinfo_address" , QVariant( value ), false, true ); }
								else if( ( field == "AdrMiasto" ) && ( user->data( "nextinfo_city"     ).toString() == "" ) ) { user->setData( "nextinfo_city"    , QVariant( value ), false, true ); }
								else if( ( field == "TelDom"    ) && ( user->data( "nextinfo_phone2"   ).toString() == "" ) ) { user->setData( "nextinfo_phone2"  , QVariant( value ), false, true ); }
								else if( ( field == "Email2"    ) && ( user->data( "nextinfo_email2"   ).toString() == "" ) ) { user->setData( "nextinfo_email2"  , QVariant( value ), false, true ); }
								else if( ( field == "WWW"       ) && ( user->data( "nextinfo_www"      ).toString() == "" ) ) { user->setData( "nextinfo_www"     , QVariant( value ), false, true ); }
								else if( ( field == "DrugGG"    ) && ( user->data( "nextinfo_gg2"      ).toString() == "" ) ) { user->setData( "nextinfo_gg2"     , QVariant( value ), false, true ); }
								else if( ( field == "IrcNick"   ) && ( user->data( "nextinfo_irc"      ).toString() == "" ) ) { user->setData( "nextinfo_irc"     , QVariant( value ), false, true ); }
								else if( ( field == "TlenN"     ) && ( user->data( "nextinfo_tlen"     ).toString() == "" ) ) { user->setData( "nextinfo_tlen"    , QVariant( value ), false, true ); }
								else if( ( field == "WP"        ) && ( user->data( "nextinfo_wp"       ).toString() == "" ) ) { user->setData( "nextinfo_wp"      , QVariant( value ), false, true ); }
								else if( ( field == "ICQ"       ) && ( user->data( "nextinfo_icq"      ).toString() == "" ) ) { user->setData( "nextinfo_icq"     , QVariant( value ), false, true ); }
								else if( ( field == "DataUr"    ) && ( user->data( "nextinfo_birthday" ).toString() == "" ) ) { user->setData( "nextinfo_birthday", QVariant( value ), false, true ); }
								else if( ( field == "DataImien" ) && ( user->data( "nextinfo_nameday"  ).toString() == "" ) ) { user->setData( "nextinfo_nameday" , QVariant( value ), false, true ); }
								else if( ( field == "Zdjecie"   ) && ( user->data( "nextinfo_photo"    ).toString() == "" ) ) { user->setData( "nextinfo_photo"   , QVariant( value ), false, true ); }
								// save multiline information
								else if( ( field == "EdtZainter" ) && ( decodeString( user->data( "nextinfo_interests" ).toString() ) == "" ) )
									{ user->setData( "nextinfo_interests", QVariant( encodeString( value.replace( "~~", "\n" ) ) ), false, true ); }
								else if( ( field == "MemoEx"     ) && ( decodeString( user->data( "nextinfo_notes"     ).toString() ) == "" ) )
									{ user->setData( "nextinfo_notes"    , QVariant( encodeString( value.replace( "~~", "\n" ) ) ), false, true ); }
							}
						}
					}
				}
				file.close();
			}
		}
		QMessageBox *messagebox = new QMessageBox(
				qApp->translate( "@nextinfo", "Kadu - extended information - data import" ),
				qApp->translate( "@nextinfo", "%% contacts imported." ).replace( "%%", QString().setNum( imported ) ),
				QMessageBox::Information,
				QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton,
				qApp->activeWindow()
			);
		messagebox->show();
	}
}
