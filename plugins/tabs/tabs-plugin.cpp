/*
 * %kadu copyright begin%
 * Copyright 2008, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "core/core.h"
#include "gui/widgets/chat-widget/chat-widget-container-handler-repository.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/kadu-paths.h"

#include "tabs.h"
#include "tabs-chat-widget-container-handler.h"

#include "tabs-plugin.h"

TabsPlugin::~TabsPlugin()
{
}

int TabsPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	ChatWidgetContainerHandler.reset(new TabsChatWidgetContainerHandler());
	TabsManagerInstance = new TabsManager(this);
	TabsManagerInstance->setChatWidgetRepository(Core::instance()->chatWidgetRepository());
	MainConfigurationWindow::registerUiFile(KaduPaths::instance()->dataPath() + QLatin1String("plugins/configuration/tabs.ui"));
	MainConfigurationWindow::registerUiHandler(TabsManagerInstance);

	ChatWidgetContainerHandler.data()->setTabsManager(TabsManagerInstance);
	Core::instance()->chatWidgetContainerHandlerRepository()->registerChatWidgetContainerHandler(ChatWidgetContainerHandler.data());

	TabsManagerInstance->openStoredChatTabs();

	return 0;
}

void TabsPlugin::done()
{
	Core::instance()->chatWidgetContainerHandlerRepository()->unregisterChatWidgetContainerHandler(ChatWidgetContainerHandler.data());

	MainConfigurationWindow::unregisterUiHandler(TabsManagerInstance);
	MainConfigurationWindow::unregisterUiFile(KaduPaths::instance()->dataPath() + QLatin1String("plugins/configuration/tabs.ui"));
	delete TabsManagerInstance;
	TabsManagerInstance = 0;
	ChatWidgetContainerHandler.reset();
}

Q_EXPORT_PLUGIN2(tabs, TabsPlugin)

#include "moc_tabs-plugin.cpp"
