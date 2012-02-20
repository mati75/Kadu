/****************************************************************************
*                                                                           *
*   GlobalHotkeys plugin for Kadu                                           *
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


#ifndef HOTKEY_H
	#define HOTKEY_H


#include <QString>

#include "property.h"

#include "defines.h"


class HotKey
{
	public:
		HotKey();
		HotKey( bool shift, bool control, bool alt, bool altgr, bool super, int keycode, QString comment = "" );
		HotKey( QString hotkeystring );
		~HotKey();
		bool operator==( const HotKey &other );
		bool isNull();
		PROPERTY_RW( bool   , SHIFT  , shift  , setShift   );
		PROPERTY_RW( bool   , CONTROL, control, setControl );
		PROPERTY_RW( bool   , ALT    , alt    , setAlt     );
		PROPERTY_RW( bool   , ALTGR  , altGr  , setAltGr   );
		PROPERTY_RW( bool   , SUPER  , super  , setSuper   );
		PROPERTY_RW( int    , KEYCODE, keyCode, setKeyCode );
		PROPERTY_RW( QString, STRING , string , setString  );
};


#endif
