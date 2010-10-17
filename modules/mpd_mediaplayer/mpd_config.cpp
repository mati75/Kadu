/*
 * %kadu copyright begin%
 * Copyright 2010 badboy (badboy@gen2.org)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include "configuration/configuration-file.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "misc/path-conversion.h"

#include "mpd_config.h"

MPDConfig::MPDConfig()
{
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/mpd_config.ui"));
	createDefaultConfiguration();
	host = config_file.readEntry("MediaPlayer", "MPDHost");
	port = config_file.readEntry("MediaPlayer", "MPDPort");
	timeout = config_file.readEntry("MediaPlayer", "MPDTimeout");
}

MPDConfig::~MPDConfig()
{
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/mpd_config.ui"));
}

void MPDConfig::createDefaultConfiguration()
{
	config_file.addVariable("MediaPlayer", "MPDHost", "localhost");
	config_file.addVariable("MediaPlayer", "MPDPort", "6600");
	config_file.addVariable("MediaPlayer", "MPDTimeout", "10");
}

void MPDConfig::configurationUpdated()
{
	host = config_file.readEntry("MediaPlayer", "MPDHost");
	port = config_file.readEntry("MediaPlayer", "MPDPort");
	timeout = config_file.readEntry("MediaPlayer", "MPDTimeout");
}
