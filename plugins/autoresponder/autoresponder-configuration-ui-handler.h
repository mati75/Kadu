/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef AUTORESPONDER_CONFIGURATION_UI_HOLDER_H
#define AUTORESPONDER_CONFIGURATION_UI_HOLDER_H

#include "gui/windows/main-configuration-window.h"

class AutoresponderConfigurationUiHolder : public ConfigurationUiHandler
{
	Q_OBJECT

protected:
	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

public:
	explicit AutoresponderConfigurationUiHolder(QObject *parent = 0);
	virtual ~AutoresponderConfigurationUiHolder();

};

#endif // AUTORESPONDER_CONFIGURATION_UI_HOLDER_H
