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


#ifndef BUDDYNEXTINFODATA_H
	#define BUDDYNEXTINFODATA_H


#include <QDate>

#include "buddies/buddy.h"

#include "defines.h"


class BuddyNExtInfoData
{
	public:
		static QDate birthdayDate( Buddy buddy );
		static QDate nextBirthdayDate( Buddy buddy );
		static QDate nextNamedayDate( Buddy buddy );
		static int age( Buddy buddy );
		static int nextBirthdayAge( Buddy buddy );
		static QDate birthdayRemindDate( Buddy buddy );
		static QDate namedayRemindDate( Buddy buddy );
		static void setBirthdayRemindDate( Buddy buddy, QDate date );
		static void setNamedayRemindDate( Buddy buddy, QDate date );
		static QString middleName( Buddy buddy );
		static void setMiddleName( Buddy buddy, const QString &middlename );
		static QString address( Buddy buddy );
		static void setAddress( Buddy buddy, const QString &address );
		static QString city( Buddy buddy );
		static void setCity( Buddy buddy, const QString &city );
		static QString email2( Buddy buddy );
		static void setEmail2( Buddy buddy, const QString &email2 );
		static QString birthday( Buddy buddy );
		static void setBirthday( Buddy buddy, const QString &birthday );
		static QString nameday( Buddy buddy );
		static void setNameday( Buddy buddy, const QString &nameday );
		static QString interests( Buddy buddy );
		static void setInterests( Buddy buddy, const QString &interests );
		static QString notes( Buddy buddy );
		static void setNotes( Buddy buddy, const QString &notes );
		static int birthdayRemind( Buddy buddy );
		static void setBirthdayRemind( Buddy buddy, int birthdayremind );
		static int namedayRemind( Buddy buddy );
		static void setNamedayRemind( Buddy buddy, int namedayremind );
	protected:
		static QDate closestDate( int month, int day );
};


#endif
