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
	const int NUMLOCK_LED    = 1;
	const int CAPSLOCK_LED   = 2;
	const int SCROLLLOCK_LED = 3;
}




class LedDriver::Impl
{
	public:
		Impl();
		void set( bool ledState );
	private:
		Display* display_;
		XKeyboardControl values_;
};


LedDriver::Impl::Impl()
{
	display_ = QX11Info::display();
	values_.led = SCROLLLOCK_LED;
}


void LedDriver::Impl::set( bool ledState )
{
	// old X
	values_.led_mode = ( ledState ? LedModeOn : LedModeOff );
	XChangeKeyboardControl( display_, KBLed | KBLedMode, &values_ );
	// new X
	int xkbmajor = XkbMajorVersion, xkbminor = XkbMinorVersion;
	int xkbopcode, xkbevent, xkberror;
	Atom ledatom;
	if( XkbQueryExtension( display_, &xkbopcode, &xkbevent, &xkberror, &xkbmajor, &xkbminor ) )
	{
		ledatom = XInternAtom( display_, "Scroll Lock", True );
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


void LedDriver::set( bool ledState )
{
	impl_->set( ledState );
}
