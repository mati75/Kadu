/****************************************************************************
*                                                                           *
*   WideIconMenuStyle 2.0                                                   *
*   Copyright (C) 2009-2012  Piotr Dąbrowski ultr@ultr.pl                   *
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


#ifndef WIDEICONMENUSTYLE_H
	#define WIDEICONMENUSTYLE_H


	#include <QProxyStyle>


	#define  LEFTMARGIN    2  /*px*/
	#define  RIGHTMARGIN   1  /*px*/


	class WideIconMenuStyle : public QProxyStyle
	{
		public:
			static int defaultIconSize();
		public:
			WideIconMenuStyle( int iconWidth = 0, int iconHeight = 0 );
		public:
			virtual void  drawControl( ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = 0 ) const;
			virtual int   pixelMetric( PixelMetric metric, const QStyleOption *option, const QWidget *widget ) const;
			virtual QSize sizeFromContents( ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget ) const;
		protected:
			int  iconwidth;
			int  iconheight;
			bool defaultsize;
	};


#endif
