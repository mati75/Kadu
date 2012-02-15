/*
 * %kadu copyright begin%
 * Copyright 2008, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "gui/windows/main-configuration-window.h"
#include "misc/path-conversion.h"

#include "single-window.h"

#include "single-window-plugin.h"

SingleWindowPlugin::~SingleWindowPlugin()
{
}

int SingleWindowPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	SingleWindowManagerInstance = new SingleWindowManager(this);
	MainConfigurationWindow::registerUiFile(dataPath("plugins/configuration/single_window.ui"));

	return 0;
}

void SingleWindowPlugin::done()
{
	MainConfigurationWindow::unregisterUiFile(dataPath("plugins/configuration/single_window.ui"));
	delete SingleWindowManagerInstance;
	SingleWindowManagerInstance = 0;
}

Q_EXPORT_PLUGIN2(single_window, SingleWindowPlugin)
