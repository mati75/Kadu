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


static QString getTag_address( Talkable talkable )
{
	BuddyNExtInfoData *bdata = NExtInfo::bData( talkable.toBuddy() );
	if( ! bdata )
		return QString();
	return bdata->address();
}


static QString getTag_city( Talkable talkable )
{
	BuddyNExtInfoData *bdata = NExtInfo::bData( talkable.toBuddy() );
	if( ! bdata )
		return QString();
	return bdata->city();
}


static QString getTag_email2( Talkable talkable )
{
	BuddyNExtInfoData *bdata = NExtInfo::bData( talkable.toBuddy() );
	if( ! bdata )
		return QString();
	return bdata->email2();
}


static QString getTag_birthday( Talkable talkable )
{
	BuddyNExtInfoData *bdata = NExtInfo::bData( talkable.toBuddy() );
	if( ! bdata )
		return QString();
	return bdata->birthday();
}


static QString getTag_nameday( Talkable talkable )
{
	BuddyNExtInfoData *bdata = NExtInfo::bData( talkable.toBuddy() );
	if( ! bdata )
		return QString();
	return bdata->nameday();
}


static QString getTag_interests( Talkable talkable )
{
	BuddyNExtInfoData *bdata = NExtInfo::bData( talkable.toBuddy() );
	if( ! bdata )
		return QString();
	return bdata->interests().replace( "\n", "<br/>" );
}


static QString getTag_notes( Talkable talkable )
{
	BuddyNExtInfoData *bdata = NExtInfo::bData( talkable.toBuddy() );
	if( ! bdata )
		return QString();
	return bdata->notes().replace( "\n", "<br/>" );
}


#endif