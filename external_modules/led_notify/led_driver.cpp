/***************************************************************************
 *   Copyright (C) 2008 by Michał Małek                                    *
 *   michalm at the jabster.pl                                             *
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

#include "led_driver.h"
#include <QPaintDevice>
#include <X11/Xlib.h>

namespace
{
	const int NUMLOCK_LED = 1;
	const int CAPSLOCK_LED = 2;
	const int SCROLLLOCK_LED = 3;
} // namespace

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
	display_ = QPaintDevice::x11AppDisplay();
	values_.led = SCROLLLOCK_LED;
}

void LedDriver::Impl::set( bool ledState )
{
	values_.led_mode = ( ledState ? LedModeOn : LedModeOff );
	XChangeKeyboardControl( display_, KBLed | KBLedMode, &values_ );
}

LedDriver::LedDriver()
	: impl_( new Impl )
{
}

LedDriver::~LedDriver()
{
}

void LedDriver::set( bool ledState )
{
	impl_->set( ledState );
}
