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


#include <QIcon>
#include <QPainter>
#include <QPixmap>
#include <QRect>
#include <QStyleOption>
#include <math.h>
#include "wideiconsmenu.h"


WideIconsMenu::WideIconsMenu( int iconswidth ) : QProxyStyle(), iconswidth(iconswidth)
{
}


int WideIconsMenu::pixelMetric( PixelMetric metric, const QStyleOption *option, const QWidget *widget ) const
{
	if( metric == QStyle::PM_SmallIconSize )
	{
		// return icon size
		return iconswidth;
	}
	// orginal pixelMetric
	return QProxyStyle::pixelMetric( metric, option, widget );
}


void WideIconsMenu::drawControl( ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget ) const
{
	if( element == QStyle::CE_MenuItem )
	{
		QStyleOptionMenuItem *optionmenuitem = (QStyleOptionMenuItem*)( qstyleoption_cast<const QStyleOptionMenuItem*>( option ) );
		if( optionmenuitem )
		{
			// save orginal icon
			QIcon icon = optionmenuitem->icon;
			// clear orginal icon
			QPixmap emptypixmap = QPixmap( icon.actualSize( QSize( iconswidth, ICONHEIGHT ) ).width(), ICONHEIGHT );
			emptypixmap.fill( Qt::transparent );
			optionmenuitem->icon = QIcon( emptypixmap );
			// draw the control with no icon, so Style will not scale it down
			QProxyStyle::drawControl( element, optionmenuitem, painter, widget );
			// draw orginal icon in its full size
			QPixmap pixmap = icon.pixmap( iconswidth, ICONHEIGHT );
			QRect rect(
				optionmenuitem->rect.x() + LEFTMARGIN,
				optionmenuitem->rect.y() + round( ( optionmenuitem->rect.height() - ICONHEIGHT ) / 2.0 ),
				iconswidth,
				ICONHEIGHT
				);
			QRect pixmaprect( 0, 0, iconswidth, ICONHEIGHT );
			painter->drawPixmap( rect, pixmap, pixmaprect );
			// done
			return;
		}
	}
	// otherwise do the orginal drawing
	QProxyStyle::drawControl( element, option, painter, widget );
}
