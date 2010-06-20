/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "debug.h"
#include "modules.h"
#include "../sms/sms.h"
#include "plus_pl_sms_ui_handler.h"
#include "sms_plus_pl_gateway.h"

plus_pl_sms::UIHandler uiHandler;

extern "C" int plus_pl_sms_init()
{
	kdebugf();
	smsConfigurationUiHandler->registerGateway("plus_pl", &SmsPlusPlGateway::isValidPlusPl);
	MainConfigurationWindow::registerUiFile(
			dataPath("kadu/modules/configuration/plus_pl_sms.ui").toAscii().constData(), &uiHandler);
	kdebugf2();
	return 0;
}

extern "C" void plus_pl_sms_close()
{
	kdebugf();
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/plus_pl_sms.ui"), &uiHandler);
	smsConfigurationUiHandler->unregisterGateway("plus_pl");
	kdebugf2();
}

