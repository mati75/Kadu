/****************************************************************************
*                                                                           *
*   GlobalHotkeys plugin for Kadu                                           *
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


#ifndef HOTKEYEDIT_H
	#define HOTKEYEDIT_H


#include <QString>

#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/config-line-edit.h"

#include "defines.h"


class HotkeyEdit : public ConfigLineEdit
{
	public:
		HotkeyEdit( const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager );
		HotkeyEdit( ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager );
	protected:
		virtual bool x11Event( XEvent *event );
		virtual void focusInEvent( QFocusEvent *event );
		virtual void focusOutEvent( QFocusEvent *event );
	private:
		QString LASTVALIDVALUE;
};


#endif
