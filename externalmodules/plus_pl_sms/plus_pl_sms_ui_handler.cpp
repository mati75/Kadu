/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "configuration_window_widgets.h"
#include "debug.h"

#include "plus_pl_sms_ui_handler.h"

plus_pl_sms::UIHandler::UIHandler(QObject *parent, char *name)
{
	kdebugf();
}

plus_pl_sms::UIHandler::~UIHandler()
{
	kdebugf();
}

void plus_pl_sms::UIHandler::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	kdebugf();
	ConfigLineEdit *passLineEdit = dynamic_cast<ConfigLineEdit *>(mainConfigurationWindow->widgetById("plus_pl_sms/Password"));
	if(passLineEdit)
	{
		passLineEdit->setEchoMode(QLineEdit::Password);
	}
	kdebugf2();
}

