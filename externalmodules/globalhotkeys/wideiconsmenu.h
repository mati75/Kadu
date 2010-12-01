/****************************************************************************
*                                                                           *
*   WideIconsMenu for Qt4                                                   *
*   Copyright (C) 2009  Piotr Dąbrowski ultr@ultr.pl                        *
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


#ifndef __WIDEICONSMENU_H
	#define __WIDEICONSMENU_H


	#include "proxystyle.h"


	#define  ICONHEIGHT  16  /*px*/
	#define  LEFTMARGIN   2  /*px*/


	class WideIconsMenu : public ProxyStyle
	{
		public:
			explicit WideIconsMenu( int _iconswidth, const QString &baseStyle ) : ProxyStyle( baseStyle ) { iconswidth = _iconswidth; }
			int pixelMetric( PixelMetric metric, const QStyleOption *option=0, const QWidget *widget=0 ) const;
			void drawControl( ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget=0 ) const;
		private:
			int iconswidth;
	};


#endif
