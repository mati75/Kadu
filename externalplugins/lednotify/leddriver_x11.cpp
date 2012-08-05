/***************************************************************************
 *   Copyright (C) 2008-2011                                               *
 *     Michał Małek  michalm@jabster.pl                                    *
 *     Piotr Dąbrowski  ultr@ultr.pl                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/




#include <QX11Info>

#include <X11/Xlib.h>
#include <X11/XKBlib.h>

#include "leddriver.h"




namespace
{
	const int CAPSLOCK_LED   = 1;
	const int NUMLOCK_LED    = 2;
	const int SCROLLLOCK_LED = 3;
}




class LedDriver::Impl
{
	public:
		Impl();
		void set( LedDriver::Diode diode, bool ledState );
	private:
		Display* display_;
		XKeyboardControl values_;
};


LedDriver::Impl::Impl()
{
	display_ = QX11Info::display();
}


void LedDriver::Impl::set( LedDriver::Diode diode, bool ledState )
{
	// old X
	switch( diode )
	{
		case DiodeScrollLock:
			values_.led = SCROLLLOCK_LED;
			break;
		case DiodeNumLock:
			values_.led = NUMLOCK_LED;
			break;
		case DiodeCapsLock:
			values_.led = CAPSLOCK_LED;
			break;
	}
	values_.led_mode = ( ledState ? LedModeOn : LedModeOff );
	XChangeKeyboardControl( display_, KBLed | KBLedMode, &values_ );
	// new X
	int xkbmajor = XkbMajorVersion, xkbminor = XkbMinorVersion;
	int xkbopcode, xkbevent, xkberror;
	Atom ledatom = None;
	if( XkbQueryExtension( display_, &xkbopcode, &xkbevent, &xkberror, &xkbmajor, &xkbminor ) )
	{
		switch( diode )
		{
			case DiodeScrollLock:
				ledatom = XInternAtom( display_, "Scroll Lock", True );
				break;
			case DiodeNumLock:
				ledatom = XInternAtom( display_, "Num Lock", True );
				break;
			case DiodeCapsLock:
				ledatom = XInternAtom( display_, "Caps Lock", True );
				break;
		}
		if( ( ledatom != None ) && XkbGetNamedIndicator( display_, ledatom, NULL, NULL, NULL, NULL ) )
		{
			XkbSetNamedIndicator( display_, ledatom, True, ledState, False, NULL );
		}
	}
}


LedDriver::LedDriver() : impl_( new Impl )
{
}


LedDriver::~LedDriver()
{
}


void LedDriver::set( Diode diode, bool ledState )
{
	impl_->set( diode, ledState );
}
