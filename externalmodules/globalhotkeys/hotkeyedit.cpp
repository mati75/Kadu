/****************************************************************************
*                                                                           *
*   GlobalHotkeys module for Kadu                                           *
*   Copyright (C) 2008-2010  Piotr Dąbrowski ultr@ultr.pl                   *
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




#include "hotkeyedit.h"

#include <QX11Info>

#include <X11/Xlib.h>
#include <X11/keysym.h>




HotkeyEdit::HotkeyEdit( const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager )
	: ConfigLineEdit( section, item, widgetCaption, toolTip, parentConfigGroupBox, dataManager )
{
	LASTVALIDVALUE = "";
}


HotkeyEdit::HotkeyEdit( ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager )
	: ConfigLineEdit( parentConfigGroupBox, dataManager )
{
}


bool HotkeyEdit::x11Event( XEvent *event )
{
	if( ( event->type == KeyPress ) || ( event->type == KeyRelease ) )  // is it mouse KeyPress or KeyRelease event?
	{
		// event data
		uint keycode = event->xkey.keycode;
		KeySym keysym = XKeycodeToKeysym( QX11Info::display(), keycode, 0 );
		// result string
		QString hotkeystring = "";
		// get the modifiers
		bool mod_shift;
		bool mod_control;
		bool mod_alt;
		bool mod_altgr;
		bool mod_super;
		mod_shift   = ( ( event->xkey.state & GLOBALHOTKEYS_X11SHIFTMASK   ) != 0 );
		mod_control = ( ( event->xkey.state & GLOBALHOTKEYS_X11CONTROLMASK ) != 0 );
		mod_alt     = ( ( event->xkey.state & GLOBALHOTKEYS_X11ALTMASK     ) != 0 );
		mod_altgr   = ( ( event->xkey.state & GLOBALHOTKEYS_X11ALTGRMASK   ) != 0 );
		mod_super   = ( ( event->xkey.state & GLOBALHOTKEYS_X11SUPERMASK   ) != 0 );
		// checking events
		if( event->type == KeyPress )  // is it mouse KeyPress event?
		{
			// build hotkeystring
			if(
				( keysym != GLOBALHOTKEYS_X11LSHIFT   ) &&
				( keysym != GLOBALHOTKEYS_X11RSHIFT   ) &&
				( keysym != GLOBALHOTKEYS_X11LCONTROL ) &&
				( keysym != GLOBALHOTKEYS_X11RCONTROL ) &&
				( keysym != GLOBALHOTKEYS_X11LALT     ) &&
				( keysym != GLOBALHOTKEYS_X11RALT     ) &&
				( keysym != GLOBALHOTKEYS_X11ALTGR    ) &&
				( keysym != GLOBALHOTKEYS_X11LSUPER   ) &&
				( keysym != GLOBALHOTKEYS_X11RSUPER   )
			)
			{
				if(
						( ! mod_shift ) && ( ! mod_control ) && ( ! mod_alt ) && ( ! mod_altgr ) && ( ! mod_super ) &&
						( keysym == GLOBALHOTKEYS_CONFIGURATIONCLEARKEY )
					)
				{
					// clear key was pressed
					setText( "" );
					LASTVALIDVALUE = "";
				}
				else
				{
					hotkeystring = "";
					hotkeystring += mod_shift   ? "Shift+"   : "";
					hotkeystring += mod_control ? "Control+" : "";
					hotkeystring += mod_alt     ? "Alt+"     : "";
					hotkeystring += mod_altgr   ? "AltGr+"   : "";
					hotkeystring += mod_super   ? "Super+"   : "";
					// keysym string or key code number
					QString keystring;
					if( keysym != NoSymbol )
					{
						keystring = XKeysymToString( keysym );
						if( keystring.isEmpty() )
						{
							// keycode instead of keysym string
							keystring.setNum( keycode );
						}
						else
						{
							// if the keysym is a single lowercase letter
							if( ( keystring.length() == 1 ) && ( keystring.at(0) >= 'a' ) && ( keystring.at(0) <= 'z' ) )
							{
								// make the letter uppercase (just to have a nice looking shortcut)
								keystring = keystring.toUpper();
							}
						}
					}
					else
					{
						// keycode instead of keysym string
						keystring.setNum( keycode );
					}
					hotkeystring += keystring;
					// set edit field text
					setText( hotkeystring );
				}
			}
			else
			{
				if( ( keysym == GLOBALHOTKEYS_X11LSHIFT   ) || ( keysym == GLOBALHOTKEYS_X11RSHIFT   ) )
					mod_shift   = true;
				if( ( keysym == GLOBALHOTKEYS_X11LCONTROL ) || ( keysym == GLOBALHOTKEYS_X11RCONTROL ) )
					mod_control = true;
				if( ( keysym == GLOBALHOTKEYS_X11LALT     ) || ( keysym == GLOBALHOTKEYS_X11RALT     ) )
					mod_alt     = true;
				if(   keysym == GLOBALHOTKEYS_X11ALTGR                                                 )
					mod_altgr   = true;
				if( ( keysym == GLOBALHOTKEYS_X11LSUPER   ) || ( keysym == GLOBALHOTKEYS_X11RSUPER   ) )
					mod_super   = true;
				hotkeystring = "";
				hotkeystring += mod_shift   ? "Shift+"   : "";
				hotkeystring += mod_control ? "Control+" : "";
				hotkeystring += mod_alt     ? "Alt+"     : "";
				hotkeystring += mod_altgr   ? "AltGr+"   : "";
				hotkeystring += mod_super   ? "Super+"   : "";
				// set edit field text
				setText( hotkeystring );
			}
		}
		else if( event->type == KeyRelease )
		{
			if( ( ! text().isEmpty() ) && ( text().at( text().length() - 1 ) == '+' ) )  // if the hotkey typing is not finished yet ("+" at the end)
			{
				if( ! (
					( keysym != GLOBALHOTKEYS_X11LSHIFT   ) &&
					( keysym != GLOBALHOTKEYS_X11RSHIFT   ) &&
					( keysym != GLOBALHOTKEYS_X11LCONTROL ) &&
					( keysym != GLOBALHOTKEYS_X11RCONTROL ) &&
					( keysym != GLOBALHOTKEYS_X11LALT     ) &&
					( keysym != GLOBALHOTKEYS_X11RALT     ) &&
					( keysym != GLOBALHOTKEYS_X11ALTGR    ) &&
					( keysym != GLOBALHOTKEYS_X11LSUPER   ) &&
					( keysym != GLOBALHOTKEYS_X11RSUPER   )
				) )
				{
					if( ( keysym == GLOBALHOTKEYS_X11LSHIFT   ) || ( keysym == GLOBALHOTKEYS_X11RSHIFT   ) )
						mod_shift   = false;
					if( ( keysym == GLOBALHOTKEYS_X11LCONTROL ) || ( keysym == GLOBALHOTKEYS_X11RCONTROL ) )
						mod_control = false;
					if( ( keysym == GLOBALHOTKEYS_X11LALT     ) || ( keysym == GLOBALHOTKEYS_X11RALT     ) )
						mod_alt     = false;
					if(   keysym == GLOBALHOTKEYS_X11ALTGR                                                 )
						mod_altgr   = false;
					if( ( keysym == GLOBALHOTKEYS_X11LSUPER   ) || ( keysym == GLOBALHOTKEYS_X11RSUPER   ) )
						mod_super   = false;
					hotkeystring = "";
					hotkeystring += mod_shift   ? "Shift+"   : "";
					hotkeystring += mod_control ? "Control+" : "";
					hotkeystring += mod_alt     ? "Alt+"     : "";
					hotkeystring += mod_altgr   ? "AltGr+"   : "";
					hotkeystring += mod_super   ? "Super+"   : "";
					// set edit field text
					if( ! hotkeystring.isEmpty() )
					{
						setText( hotkeystring );
					}
					else
					{
						// reset the text to the last valid value
						setText( LASTVALIDVALUE );
					}
				}
			}
			else
			{
				LASTVALIDVALUE = text();
			}
		}
		// don't forward the event
		return true;
	}
	else if( ( event->type == ButtonPress ) || ( event->type == ButtonRelease ) )  // is it mouse ButtonPress or ButtonRelease event?
	{
		if( event->xbutton.button == Button1 )  // is if left mouse button?
		{
			return false;  // forward the event to Qt
		}
		return true;  // don't forward the event
	}
	return false;  // forward the event to Qt
}


void HotkeyEdit::focusInEvent( QFocusEvent *event )
{
	LASTVALIDVALUE = text();
	// important: call the default focusInEvent
	QLineEdit::focusInEvent( event );
}


void HotkeyEdit::focusOutEvent( QFocusEvent *event )
{
	if( ! text().isEmpty() )
		if( text().at( text().length() - 1 ) == '+' )  // if the hotkey typing is not finished yet ("+" at the end)
			setText( LASTVALIDVALUE ); // reset the text to the last valid value
	// important: call the default focusOutEvent
	QLineEdit::focusOutEvent( event );
}
