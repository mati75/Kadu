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


#include <QApplication>
#include <QIcon>
#include <QMenu>
#include <QPainter>
#include <QPixmap>
#include <QRect>
#include <QStyleOption>
#include <math.h>
#include "wideiconmenustyle.h"


int WideIconMenuStyle::defaultIconSize()
{
	return QApplication::style()->pixelMetric( QStyle::PM_SmallIconSize, 0, 0 );
}


WideIconMenuStyle::WideIconMenuStyle( int iconWidth, int iconHeight ) : QProxyStyle(), iconwidth( iconWidth ), iconheight( iconHeight )
{
	int defaulticonsize = defaultIconSize();
	if( iconwidth == 0 )
		iconwidth = defaulticonsize;
	if( iconheight == 0 )
		iconheight = defaulticonsize;
	defaultsize = ( iconwidth == defaulticonsize ) && ( iconheight == defaulticonsize );
}


void WideIconMenuStyle::drawControl( ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget ) const
{
	if( element == QStyle::CE_MenuItem )
	{
		QStyleOptionMenuItem *optionmenuitem = (QStyleOptionMenuItem*)( qstyleoption_cast<const QStyleOptionMenuItem*>( option ) );
		if( optionmenuitem )
		{
			// menuItemType
			if( optionmenuitem->menuItemType == QStyleOptionMenuItem::Normal )
			{
				const QMenu *menu = dynamic_cast<const QMenu*>( widget );
				if( menu )
					if( menu->actionAt( optionmenuitem->rect.center() )->property( "hasSubMenu" ).toBool() )
						optionmenuitem->menuItemType = QStyleOptionMenuItem::SubMenu;
			}
			if( defaultsize )
			{
				// draw the control normally
				QProxyStyle::drawControl( element, optionmenuitem, painter, widget );
			}
			else
			{
				int defaulticonsize = defaultIconSize();
				// save orginal icon
				QIcon icon = optionmenuitem->icon;
				// clear orginal icon
				QPixmap emptypixmap = QPixmap( icon.actualSize( QSize( iconwidth, iconheight ) ).width(), iconheight );
				emptypixmap.fill( Qt::transparent );
				optionmenuitem->icon = QIcon( emptypixmap );
				// maxIconWidth
				int oldmaxiconwidth = optionmenuitem->maxIconWidth;
				optionmenuitem->maxIconWidth = LEFTMARGIN + iconwidth + optionmenuitem->maxIconWidth - defaulticonsize + RIGHTMARGIN;
				// draw the control with no icon, so that the system style will not scale it down
				QProxyStyle::drawControl( element, optionmenuitem, painter, widget );
				// if wideicon: draw the orginal icon in its full size
				QPixmap pixmap = icon.pixmap( iconwidth, iconheight );
				QRect rect(
					optionmenuitem->rect.x() + LEFTMARGIN + round( ( oldmaxiconwidth - iconheight ) / 2.0 ),
					optionmenuitem->rect.y() + round( ( optionmenuitem->rect.height() - iconheight ) / 2.0 ),
					iconwidth,
					iconheight
				);
				QRect pixmaprect( 0, 0, iconwidth, iconheight );
				painter->drawPixmap( rect, pixmap, pixmaprect );
			}
			// done
			return;
		}
	}
	// otherwise do the orginal drawing
	QProxyStyle::drawControl( element, option, painter, widget );
}


QSize WideIconMenuStyle::sizeFromContents( ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget ) const
{
	if( ! defaultsize )
		if( type == QStyle::CT_MenuItem )
		{
			int defaulticonsize = defaultIconSize();
			QStyleOptionMenuItem *optionmenuitem = (QStyleOptionMenuItem*)( qstyleoption_cast<const QStyleOptionMenuItem*>( option ) );
			return QProxyStyle::sizeFromContents( type, option, size, widget ) + QSize( LEFTMARGIN + iconwidth + optionmenuitem->maxIconWidth - defaulticonsize + RIGHTMARGIN, iconheight - defaulticonsize );
		}
	return QProxyStyle::sizeFromContents( type, option, size, widget );
}


int WideIconMenuStyle::pixelMetric( PixelMetric metric, const QStyleOption *option, const QWidget *widget ) const
{
	if( ! defaultsize )
		if( metric == QStyle::PM_SmallIconSize )
		{
			// return icon size
			return iconheight;
		}
	// orginal pixelMetric
	return QProxyStyle::pixelMetric( metric, option, widget );
}
