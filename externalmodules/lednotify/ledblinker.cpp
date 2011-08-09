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




#include "configuration/configuration-file.h"

#include "ledblinker.h"




LedBlinker::LedBlinker() : value_( false ), enabled_( false ), countdown_( false ), counter_( 0 )
{
	timer_.setSingleShot( true );
	configurationUpdated();
	connect( &timer_, SIGNAL(timeout()), this, SLOT(blink()) );
}


LedBlinker::~LedBlinker()
{
	disconnect( &timer_, SIGNAL(timeout()), this, SLOT(blink()) );
	if( value_ )
		led_.set( false );
}


void LedBlinker::startInfinite()
{
	if( !enabled_ || countdown_ )
	{
		countdown_ = false;
		enabled_   = true;
		blink();
	}
}


void LedBlinker::startFinite()
{
	if( !enabled_ || countdown_ )
	{
		enabled_   = true;
		countdown_ = true;
		counter_   = blinkCount_;
		blink();
	}
}


void LedBlinker::stop()
{
	timer_.stop();
	enabled_ = false;
	countdown_ = false;
	value_ = false;
	led_.set( false );
}


void LedBlinker::configurationUpdated()
{
	delay_ = config_file.readNumEntry("LedNotify", "LEDdelay");
	blinkCount_ = config_file.readNumEntry("LedNotify", "LEDcount");
}


void LedBlinker::blink(void)
{
	value_ = !value_;
	led_.set( value_ );
	// Count all "on" states
	if( countdown_ && value_ )
	{
		if( counter_ <= 1 )
		{
			enabled_ = false;
			timer_.start( delay_ );
		}
		else
			--counter_;
	}
	if( enabled_ )
		timer_.start( delay_ );
}
