/****************************************************************************
*                                                                           *
*   NExtInfo plugin for Kadu                                                *
*   Copyright (C) 2008-2011  Piotr Dąbrowski ultr@ultr.pl                   *
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




#include "photowidget.h"

#include "avatars/avatar.h"
#include "buddies/buddy-preferred-manager.h"




PhotoWidget::PhotoWidget( QWidget *parent ) : QScrollArea( parent )
{
	setWindowRole( "kadu-nextinfo-photo" );
	setFrameStyle( QFrame::NoFrame );
	setLineWidth( 0 );
	setAlignment( Qt::AlignCenter );
	QWidget *photoscrollareaviewport = new QWidget( this );
	photoscrollareaviewport->setAttribute( Qt::WA_NoSystemBackground, true );
	setViewport( photoscrollareaviewport );
	photo.setAlignment( Qt::AlignCenter );
	photo.setAttribute( Qt::WA_NoSystemBackground, true );
	setWidget( &photo );
	photo.setAutoFillBackground( false ); // must be called after setWidget( photo );
}


bool PhotoWidget::setBuddy( Buddy buddy )
{
	photo.clear();
	if( ! buddy )
		return false;
	setWindowTitle( buddy.display() );
	QPixmap pixmap;
	if( ! buddy.buddyAvatar().filePath().isEmpty() )
	{
		pixmap = QPixmap( buddy.buddyAvatar().filePath() );
	}
	else
	{
		Contact preferredContact = BuddyPreferredManager::instance()->preferredContact( buddy );
		if( preferredContact && ( ! preferredContact.contactAvatar().filePath().isEmpty() ) )
			pixmap = QPixmap( preferredContact.contactAvatar().filePath() );
	}
	photo.setPixmap( pixmap );
	photo.adjustSize();
	return true;
}
