/****************************************************************************
*                                                                           *
*   NExtInfo module for Kadu                                                *
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




#include <QtCore/QDate>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

#include "configuration/xml-configuration-file.h"
#include "storage/storage-point.h"
#include "debug.h"

#include "buddynextinfodata.h"




BuddyNExtInfoData::BuddyNExtInfoData( StorableObject *parent ) : ModuleData( parent )
{
}


BuddyNExtInfoData::~BuddyNExtInfoData()
{
}


QString BuddyNExtInfoData::name() const
{
	return QLatin1String( "nextinfo" );
}


void BuddyNExtInfoData::load()
{
	if( ! isValidStorage() )
		return;
	StorableObject::load();
	Address        = loadValue<QString>( "address"       , "" );
	City           = loadValue<QString>( "city"          , "" );
	Email2         = loadValue<QString>( "email2"        , "" );
	Birthday       = loadValue<QString>( "birthday"      , "" );
	Nameday        = loadValue<QString>( "nameday"       , "" );
	Interests      = loadValue<QString>( "interests"     , "" );
	Notes          = loadValue<QString>( "notes"         , "" );
	BirthdayRemind = loadValue<int>(     "birthdayremind",  0 );
	NamedayRemind  = loadValue<int>(     "namedayremind" ,  0 );
}


void BuddyNExtInfoData::store()
{
	if( ! isValidStorage() )
		return;
	storeValue( "address"       , Address        );
	storeValue( "city"          , City           );
	storeValue( "email2"        , Email2         );
	storeValue( "birthday"      , Birthday       );
	storeValue( "nameday"       , Nameday        );
	storeValue( "interests"     , Interests      );
	storeValue( "notes"         , Notes          );
	storeValue( "birthdayremind", BirthdayRemind );
	storeValue( "namedayremind" , NamedayRemind  );
}


QDate BuddyNExtInfoData::birthdayDate()
{
	ensureLoaded();
	if( Birthday.isEmpty() )
		return QDate();
	if( ! Birthday.contains( QRegExp( NEXTINFO_REGEXPBIRTHDAY ) ) ) // bad format
		return QDate();
	// get date elemeents
	QStringList dateelements = Birthday.split( '.' );
	int birthday_day   = dateelements.at(0).toInt();
	int birthday_month = dateelements.at(1).toInt();
	int birthday_year  = dateelements.at(2).toInt();
	return QDate( birthday_year, birthday_month, birthday_day );
}


QDate BuddyNExtInfoData::nextBirthdayDate()
{
	ensureLoaded();
	QDate birthdaydate = birthdayDate();
	if( ! birthdaydate.isValid() )
		return QDate();
	return closestDate( birthdaydate.month(), birthdaydate.day() );
}


QDate BuddyNExtInfoData::nextNamedayDate()
{
	ensureLoaded();
	if( Nameday.isEmpty() )
		return QDate();
	if( ! Nameday.contains( QRegExp( NEXTINFO_REGEXPNAMEDAY ) ) ) // bad format
		return QDate();
	// get date elemeents
	QStringList dateelements = Nameday.split( '.' );
	int nameday_day   = dateelements.at(0).toInt();
	int nameday_month = dateelements.at(1).toInt();
	return closestDate( nameday_month, nameday_day );
}


int BuddyNExtInfoData::age()
{
	ensureLoaded();
	QDate birthdaydate = birthdayDate();
	if( ! birthdaydate.isValid() )
		return -1;
	int a = QDate::currentDate().year() - birthdaydate.year();
	if( ( nextBirthdayDate() != QDate::currentDate() ) && ( nextBirthdayDate().year() == QDate::currentDate().year() ) )
		a--;
	return a;
}


int BuddyNExtInfoData::nextBirthdayAge()
{
	ensureLoaded();
	QDate birthdaydate = birthdayDate();
	if( ! birthdaydate.isValid() )
		return -1;
	QDate nextbirthdaydate = nextBirthdayDate();
	return nextbirthdaydate.year() - birthdaydate.year();
}


QDate BuddyNExtInfoData::birthdayRemindDate()
{
	ensureLoaded();
	QDateTime datetime;
	datetime.setTime_t( BirthdayRemind );
	return datetime.date();
}


QDate BuddyNExtInfoData::namedayRemindDate()
{
	ensureLoaded();
	QDateTime datetime;
	datetime.setTime_t( NamedayRemind );
	return datetime.date();
}


void BuddyNExtInfoData::setBirthdayRemindDate( QDate date )
{
	ensureLoaded();
	BirthdayRemind = QDateTime( date ).toTime_t();
}


void BuddyNExtInfoData::setNamedayRemindDate( QDate date )
{
	ensureLoaded();
	NamedayRemind = QDateTime( date ).toTime_t();
}


QDate BuddyNExtInfoData::closestDate( int month, int day )
{
	QDate currentdate = QDate::currentDate();
	int year = currentdate.year();
	if( ( month < currentdate.month() ) || ( ( month == currentdate.month() ) && ( day < currentdate.day() ) ) )
		year++;
	return QDate( year, month, day );
}
