/***************************************************************************
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 3 of the License, or      *
*   (at your option) any later version.                                    *
*                                                                          *
***************************************************************************/




#include "hotkey.h"

#include <QStringList>
#include <QX11Info>

#include <X11/Xlib.h>




Hotkey::Hotkey( bool _shift, bool _control, bool _alt, bool _altgr, bool _super, int _keycode, QString _comment )
{
	shift   = _shift;
	control = _control;
	alt     = _alt;
	altgr   = _altgr;
	super   = _super;
	keycode = _keycode;
	comment = _comment;
}


Hotkey::Hotkey( QString hotkeystring )
{
	// remove "white" characters from begining and end of the string
	hotkeystring = hotkeystring.stripWhiteSpace();
	// initial values
	shift   = false;
	control = false;
	alt     = false;
	altgr   = false;
	super   = false;
	keycode = 0;
	// return if the string is empty
	if( hotkeystring.isEmpty() ) return;
	// split hotkeystring into parts using "+" as the separator
	QStringList parts = QStringList::split( QString("+"), hotkeystring, true );
	// set Shift
	if( parts.contains( "Shift" ) > 0 )
		shift = true;
	// set Control
	if( parts.contains( "Control" ) > 0 )
		control = true;
	// set Alt
	if( parts.contains( "Alt" ) > 0 )
		alt = true;
	// set AltGr
	if( parts.contains( "AltGr" ) > 0 )
		altgr = true;
	// set Super
	if( parts.contains( "Super" ) > 0 )
		super = true;
	bool ok;
	keycode = parts.last().toInt( &ok );
	if( ! ok )  // if the last part of hotkey string is not an integer
	{
		KeySym keysym = XStringToKeysym( parts.last().ascii() );
		if( keysym != NoSymbol )
		{
			keycode = XKeysymToKeycode( QX11Info::display(), keysym );
		}
	}
	comment = hotkeystring;
}


bool Hotkey::equals( Hotkey *h )
{
	if( shift   != h->shift   ) return false;
	if( control != h->control ) return false;
	if( alt     != h->alt     ) return false;
	if( altgr   != h->altgr   ) return false;
	if( super   != h->super   ) return false;
	if( keycode != h->keycode ) return false;
	return true;
}
