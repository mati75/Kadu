/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "gui/windows/main-configuration-window.h"
#include "misc/misc.h"

#include "mime_tex.h"

#include "mime_tex_plugin.h"

MimeTeX::MimeTeXPlugin::~MimeTeXPlugin()
{
}

int MimeTeX::MimeTeXPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	MainConfigurationWindow::registerUiFile(dataPath("plugins/configuration/mime_tex.ui"));
	MimeTeX::createInstance();

	return 0;
}

void MimeTeX::MimeTeXPlugin::done()
{
	MimeTeX::destroyInstance();
	MainConfigurationWindow::unregisterUiFile(dataPath("plugins/configuration/mime_tex.ui"));
}

Q_EXPORT_PLUGIN2(mime_tex, MimeTeX::MimeTeXPlugin)
