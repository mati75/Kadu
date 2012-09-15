/****************************************************************************
*                                                                           *
*   NExtInfo plugin for Kadu                                                *
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


QDate BuddyNExtInfoData::birthdayDate( Buddy buddy )
{
	QString Birthday = birthday( buddy );
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


QDate BuddyNExtInfoData::nextBirthdayDate( Buddy buddy )
{
	QDate birthdaydate = birthdayDate( buddy );
	if( ! birthdaydate.isValid() )
		return QDate();
	return closestDate( birthdaydate.month(), birthdaydate.day() );
}


QDate BuddyNExtInfoData::nextNamedayDate( Buddy buddy )
{
	QString Nameday = nameday( buddy );
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


int BuddyNExtInfoData::age( Buddy buddy )
{
	QDate birthdaydate = birthdayDate( buddy );
	if( ! birthdaydate.isValid() )
		return -1;
	int a = QDate::currentDate().year() - birthdaydate.year();
	if( ( nextBirthdayDate( buddy ) != QDate::currentDate() ) && ( nextBirthdayDate( buddy ).year() == QDate::currentDate().year() ) )
		a--;
	return a;
}


int BuddyNExtInfoData::nextBirthdayAge( Buddy buddy )
{
	QDate birthdaydate = birthdayDate( buddy );
	if( ! birthdaydate.isValid() )
		return -1;
	QDate nextbirthdaydate = nextBirthdayDate( buddy );
	return nextbirthdaydate.year() - birthdaydate.year();
}


QDate BuddyNExtInfoData::birthdayRemindDate( Buddy buddy )
{
	QDateTime datetime;
	datetime.setTime_t( birthdayRemind( buddy ) );
	return datetime.date();
}


QDate BuddyNExtInfoData::namedayRemindDate( Buddy buddy )
{
	QDateTime datetime;
	datetime.setTime_t( namedayRemind( buddy ) );
	return datetime.date();
}


void BuddyNExtInfoData::setBirthdayRemindDate( Buddy buddy, QDate date )
{
	int BirthdayRemind = QDateTime( date ).toTime_t();
	setBirthdayRemind( buddy, BirthdayRemind );
}


void BuddyNExtInfoData::setNamedayRemindDate( Buddy buddy, QDate date )
{
	int NamedayRemind = QDateTime( date ).toTime_t();
	setNamedayRemind( buddy, NamedayRemind );
}


QString BuddyNExtInfoData::middleName( Buddy buddy )
{
	return buddy.property( "nextinfo:middleName", "" ).toString();
}


void BuddyNExtInfoData::setMiddleName( Buddy buddy, const QString &middlename )
{
	buddy.addProperty( "nextinfo:middleName", middlename, CustomProperties::Storable );
}


QString BuddyNExtInfoData::address( Buddy buddy )
{
	return buddy.property( "nextinfo:address", "" ).toString();
}


void BuddyNExtInfoData::setAddress( Buddy buddy, const QString &address )
{
	buddy.addProperty( "nextinfo:address", address, CustomProperties::Storable );
}


QString BuddyNExtInfoData::city( Buddy buddy )
{
	return buddy.property( "nextinfo:city", "" ).toString();
}


void BuddyNExtInfoData::setCity( Buddy buddy, const QString &city )
{
	buddy.addProperty( "nextinfo:city", city, CustomProperties::Storable );
}


QString BuddyNExtInfoData::email2( Buddy buddy )
{
	return buddy.property( "nextinfo:email2", "" ).toString();
}


void BuddyNExtInfoData::setEmail2( Buddy buddy, const QString &email2 )
{
	buddy.addProperty( "nextinfo:email2", email2, CustomProperties::Storable );
}


QString BuddyNExtInfoData::birthday( Buddy buddy )
{
	return buddy.property( "nextinfo:birthday", "" ).toString();
}


void BuddyNExtInfoData::setBirthday( Buddy buddy, const QString &birthday )
{
	buddy.addProperty( "nextinfo:birthday", birthday, CustomProperties::Storable );
}


QString BuddyNExtInfoData::nameday( Buddy buddy )
{
	return buddy.property( "nextinfo:nameday", "" ).toString();
}


void BuddyNExtInfoData::setNameday( Buddy buddy, const QString &nameday )
{
	buddy.addProperty( "nextinfo:nameday", nameday, CustomProperties::Storable );
}


QString BuddyNExtInfoData::interests( Buddy buddy )
{
	return buddy.property( "nextinfo:interests", "" ).toString();
}


void BuddyNExtInfoData::setInterests( Buddy buddy, const QString &interests )
{
	buddy.addProperty( "nextinfo:interests", interests, CustomProperties::Storable );
}


QString BuddyNExtInfoData::notes( Buddy buddy )
{
	return buddy.property( "nextinfo:notes", "" ).toString();
}


void BuddyNExtInfoData::setNotes( Buddy buddy, const QString &notes )
{
	buddy.addProperty( "nextinfo:notes", notes, CustomProperties::Storable );
}


int BuddyNExtInfoData::birthdayRemind( Buddy buddy )
{
	return buddy.property( "nextinfo:birthdayremind", 0 ).toInt();
}


void BuddyNExtInfoData::setBirthdayRemind( Buddy buddy, int birthdayremind )
{
	buddy.addProperty( "nextinfo:birthdayremind", birthdayremind, CustomProperties::Storable );
}


int BuddyNExtInfoData::namedayRemind( Buddy buddy )
{
	return buddy.property( "nextinfo:namedayremind", 0 ).toInt();
}


void BuddyNExtInfoData::setNamedayRemind( Buddy buddy, int namedayremind )
{
	buddy.addProperty( "nextinfo:namedayremind", namedayremind, CustomProperties::Storable );
}


QDate BuddyNExtInfoData::closestDate( int month, int day )
{
	QDate currentdate = QDate::currentDate();
	int year = currentdate.year();
	if( ( month < currentdate.month() ) || ( ( month == currentdate.month() ) && ( day < currentdate.day() ) ) )
		year++;
	return QDate( year, month, day );
}
