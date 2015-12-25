/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "core/application.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/paths-provider.h"

#include "mime_tex.h"

#include "mime_tex_plugin.h"

MimeTeX::MimeTeXPlugin::~MimeTeXPlugin()
{
}

bool MimeTeX::MimeTeXPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	MainConfigurationWindow::registerUiFile(Application::instance()->pathsProvider()->dataPath() + "plugins/configuration/mime_tex.ui");
	MimeTeX::createInstance();

	return true;
}

void MimeTeX::MimeTeXPlugin::done()
{
	MimeTeX::destroyInstance();
	MainConfigurationWindow::unregisterUiFile(Application::instance()->pathsProvider()->dataPath() + "plugins/configuration/mime_tex.ui");
}

Q_EXPORT_PLUGIN2(mime_tex, MimeTeX::MimeTeXPlugin)

#include "moc_mime_tex_plugin.cpp"
