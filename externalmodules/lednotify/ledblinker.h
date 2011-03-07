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


#ifndef LEDBLINKER_H
	#define LEDBLINKER_H


#include <QObject>
#include <QTimer>

#include "configuration/configuration-aware-object.h"

#include "leddriver.h"


class LedBlinker : public QObject, ConfigurationAwareObject
{
	Q_OBJECT
	public:
		LedBlinker();
		~LedBlinker();
		/**
		 *	Starts infinite blinking of the LED.
		 */
		void startInfinite();
		/**
		 *	Blinks LED specified number of times.
		 */
		void startFinite();
		/**
		 *	Stops infinite blinking.
		 */
		void stop();
		/**
		 *	Checks if LED blinking is enabled.
		 */
		bool enabled() const	{ return enabled_; }
	private:
		virtual void configurationUpdated();
	private slots:
		void blink(void);
	private:
		LedDriver led_;
		QTimer timer_;
		bool value_;
		bool enabled_;
		bool countdown_;
		int delay_;
		int blinkCount_;
		int counter_;
};


#endif
