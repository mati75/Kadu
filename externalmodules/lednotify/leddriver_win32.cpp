/***************************************************************************
 *   Copyright (C) 2008                                                    *
 *     Michał Małek  michalm@jabster.pl                                    *
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
 ***************************************************************************
 *   Implementation of LED control for Windows                             *
 *   Based on Mark McGinty's code (free domain):                           *
 *   http://www.codeguru.com/cpp/w-p/system/keyboard/article.php/c2825     *
 ***************************************************************************/


#include "leddriver.h"

#include <windows.h>
#include <winioctl.h>


namespace
{
	const USHORT KEYBOARD_SCROLL_LOCK_ON = 1;
	const USHORT KEYBOARD_NUM_LOCK_ON    = 2;
	const USHORT KEYBOARD_CAPS_LOCK_ON   = 4;
	#define IOCTL_KEYBOARD_SET_INDICATORS   CTL_CODE(FILE_DEVICE_KEYBOARD, 0x0002, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define IOCTL_KEYBOARD_QUERY_TYPEMATIC  CTL_CODE(FILE_DEVICE_KEYBOARD, 0x0008, METHOD_BUFFERED, FILE_ANY_ACCESS)
	#define IOCTL_KEYBOARD_QUERY_INDICATORS CTL_CODE(FILE_DEVICE_KEYBOARD, 0x0010, METHOD_BUFFERED, FILE_ANY_ACCESS)
	struct KEYBOARD_INDICATOR_PARAMETERS
	{
		USHORT  UnitId;
		USHORT  LedFlags;
	};
}


class LedDriver::Impl
{
	public:
		Impl();
		~Impl();
		void set( bool ledState );
	private:
		HANDLE device_;
		KEYBOARD_INDICATOR_PARAMETERS inputBuffer_;
		KEYBOARD_INDICATOR_PARAMETERS outputBuffer_;
};


LedDriver::Impl::Impl()
{
	::DefineDosDevice( DDD_RAW_TARGET_PATH, TEXT("Kbd"), TEXT("\\Device\\KeyboardClass0") );
	device_ = ::CreateFile( TEXT("\\\\.\\Kbd"), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL );
	inputBuffer_.UnitId = 0;
	inputBuffer_.LedFlags = 0;
	outputBuffer_.UnitId = 0;
	outputBuffer_.LedFlags = 0;
}


LedDriver::Impl::~Impl()
{
	::DefineDosDevice( DDD_REMOVE_DEFINITION, TEXT("Kbd"), NULL );
}


void LedDriver::Impl::set( bool ledState )
{
	if( device_ != INVALID_HANDLE_VALUE )
	{
		static ULONG ReturnedLength;
		// Preserve current indicators' state
		::DeviceIoControl( device_, IOCTL_KEYBOARD_QUERY_INDICATORS,
			&inputBuffer_, sizeof(KEYBOARD_INDICATOR_PARAMETERS),
			&outputBuffer_, sizeof(KEYBOARD_INDICATOR_PARAMETERS),
			&ReturnedLength, NULL );
		// Set bits
		if( ledState )
			inputBuffer_.LedFlags = outputBuffer_.LedFlags | KEYBOARD_SCROLL_LOCK_ON;
		else
			inputBuffer_.LedFlags = outputBuffer_.LedFlags & (~KEYBOARD_SCROLL_LOCK_ON);
		//
		::DeviceIoControl( device_, IOCTL_KEYBOARD_SET_INDICATORS,
			&inputBuffer_, sizeof(KEYBOARD_INDICATOR_PARAMETERS),
			NULL, 0,  &ReturnedLength, NULL );
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
