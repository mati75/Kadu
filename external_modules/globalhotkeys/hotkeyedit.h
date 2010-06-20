/***************************************************************************
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 3 of the License, or      *
*   (at your option) any later version.                                    *
*                                                                          *
***************************************************************************/


#ifndef HOTKEYEDIT_H
	#define HOTKEYEDIT_H


#include <QString>

#include "configuration_window_widgets.h"

#include "defines.h"


class HotkeyEdit : public ConfigLineEdit
{
	public:
		HotkeyEdit( const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, char *name=0 )
			: ConfigLineEdit( section, item, widgetCaption, toolTip, parentConfigGroupBox, name ) {}
		HotkeyEdit( ConfigGroupBox *parentConfigGroupBox, char *name=0 )
			: ConfigLineEdit( parentConfigGroupBox, name ) {}
	protected:
		virtual bool x11Event( XEvent *event );
		virtual void focusInEvent( QFocusEvent *event );
		virtual void focusOutEvent( QFocusEvent *event );
	private:
		QString lastvalidvalue;
};


#endif
