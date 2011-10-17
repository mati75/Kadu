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




#include "hotkey.h"

#include <QStringList>
#include <QX11Info>

#include <X11/Xlib.h>




HotKey::HotKey()
{
	SHIFT   = false;
	CONTROL = false;
	ALT     = false;
	ALTGR   = false;
	SUPER   = false;
	KEYCODE = 0;
	STRING  = "";
}


HotKey::HotKey( bool shift, bool control, bool alt, bool altgr, bool super, int keycode, QString string )
{
	SHIFT   = shift;
	CONTROL = control;
	ALT     = alt;
	ALTGR   = altgr;
	SUPER   = super;
	KEYCODE = keycode;
	STRING  = string;
}


HotKey::HotKey( QString hotkeystring )
{
	// remove "white" characters from begining and end of the string
	hotkeystring = hotkeystring.trimmed();
	// initial values
	SHIFT   = false;
	CONTROL = false;
	ALT     = false;
	ALTGR   = false;
	SUPER   = false;
	KEYCODE = 0;
	STRING  = hotkeystring.isNull() ? "" : hotkeystring;
	// return if the string is empty
	if( hotkeystring.isEmpty() ) return;
	// split hotkeystring into parts using "+" as the separator
	QStringList parts = hotkeystring.split( "+", QString::KeepEmptyParts );
	// set Shift
	if( parts.contains( "Shift" ) > 0 )
		SHIFT = true;
	// set Control
	if( parts.contains( "Control" ) > 0 )
		CONTROL = true;
	// set Alt
	if( parts.contains( "Alt" ) > 0 )
		ALT = true;
	// set AltGr
	if( parts.contains( "AltGr" ) > 0 )
		ALTGR = true;
	// set Super
	if( parts.contains( "Super" ) > 0 )
		SUPER = true;
	bool ok;
	KEYCODE = parts.last().toInt( &ok );
	if( ! ok )  // if the last part of hotkey string is not an integer
	{
		KeySym keysym = XStringToKeysym( parts.last().toAscii().data() );
		if( keysym != NoSymbol )
		{
			KEYCODE = XKeysymToKeycode( QX11Info::display(), keysym );
		}
	}
}


HotKey::~HotKey() {}


bool HotKey::operator==( const HotKey &other )
{
	if( SHIFT   != other.SHIFT   ) return false;
	if( CONTROL != other.CONTROL ) return false;
	if( ALT     != other.ALT     ) return false;
	if( ALTGR   != other.ALTGR   ) return false;
	if( SUPER   != other.SUPER   ) return false;
	if( KEYCODE != other.KEYCODE ) return false;
	return true;
}


bool HotKey::isNull()
{
	return ( KEYCODE == 0 );
}
