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


#ifndef GETTAGS_H
	#define GETTAGS_H


#include <buddies/buddy.h>
#include <buddies/buddy-shared.h>
#include <talkable/talkable.h>

#include "buddynextinfodata.h"
#include "nextinfo.h"

#include "defines.h"


static QString getTag_middleName( Talkable talkable )
{
	return BuddyNExtInfoData::middleName( talkable.toBuddy() );
}


static QString getTag_address( Talkable talkable )
{
	return BuddyNExtInfoData::address( talkable.toBuddy() );
}


static QString getTag_city( Talkable talkable )
{
	return BuddyNExtInfoData::city( talkable.toBuddy() );
}


static QString getTag_email2( Talkable talkable )
{
	return BuddyNExtInfoData::email2( talkable.toBuddy() );
}


static QString getTag_birthday( Talkable talkable )
{
	return BuddyNExtInfoData::birthday( talkable.toBuddy() );
}


static QString getTag_nameday( Talkable talkable )
{
	return BuddyNExtInfoData::nameday( talkable.toBuddy() );
}


static QString getTag_interests( Talkable talkable )
{
	return BuddyNExtInfoData::interests( talkable.toBuddy() ).replace( "\n", "<br/>" );
}


static QString getTag_notes( Talkable talkable )
{
	return BuddyNExtInfoData::notes( talkable.toBuddy() ).replace( "\n", "<br/>" );
}


#endif
