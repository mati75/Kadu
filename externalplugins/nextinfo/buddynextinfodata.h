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

#include "storage/module-data.h"

#include "defines.h"


#undef Property
#define Property( type, name, capitalized_name )  \
	public:                                         \
		type name()                                   \
		{                                             \
			ensureLoaded();                             \
			return capitalized_name;                    \
		}                                             \
		void set##capitalized_name(const type &name)  \
		{                                             \
			ensureLoaded();                             \
			capitalized_name = name;                    \
		}                                             \
	private:                                        \
		type capitalized_name;


class BuddyNExtInfoData : public ModuleData
{
	Q_OBJECT
	public:
		BuddyNExtInfoData( const QString &moduleName, StorableObject *parent, QObject *qobjectParent );
		virtual ~BuddyNExtInfoData();
		virtual void store();
		virtual QString name() const;
		QDate birthdayDate();
		QDate nextBirthdayDate();
		QDate nextNamedayDate();
		int age();
		int nextBirthdayAge();
		QDate birthdayRemindDate();
		QDate namedayRemindDate();
		void setBirthdayRemindDate( QDate date );
		void setNamedayRemindDate( QDate date );
		Property( QString, address       , Address        );
		Property( QString, city          , City           );
		Property( QString, email2        , Email2         );
		Property( QString, birthday      , Birthday       );
		Property( QString, nameday       , Nameday        );
		Property( QString, interests     , Interests      );
		Property( QString, notes         , Notes          );
		Property( int    , birthdayRemind, BirthdayRemind );
		Property( int    , namedayRemind , NamedayRemind  );
	protected:
		virtual void load();
	private:
		QDate closestDate( int month, int day );
};


#endif
