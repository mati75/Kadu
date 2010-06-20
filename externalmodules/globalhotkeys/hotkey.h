/***************************************************************************
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 3 of the License, or      *
*   (at your option) any later version.                                    *
*                                                                          *
***************************************************************************/


#ifndef HOTKEY_H
	#define HOTKEY_H


#include <QString>

#include "defines.h"


class Hotkey
{
	public:
		Hotkey( bool _shift, bool _control, bool _alt, bool _altgr, bool _super, int _keycode, QString _comment = "" );
		Hotkey( QString hotkeystring );
		bool equals( Hotkey *h );
		bool shift;
		bool control;
		bool alt;
		bool altgr;
		bool super;
		int keycode;
		QString comment;
};


#endif
