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


#ifndef LEDDRIVER_H
	#define LEDDRIVER_H


#include <memory>


class LedDriver
{
	public:
		enum Diode
		{
			DiodeScrollLock = 0,
			DiodeNumLock    = 1,
			DiodeCapsLock   = 2
		};
		LedDriver();
		~LedDriver();
		void set( Diode diode, bool ledState );
	private:
		class Impl;
		std::auto_ptr<Impl> impl_;
};


#endif