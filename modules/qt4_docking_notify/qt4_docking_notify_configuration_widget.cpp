/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Kermit (plaza.maciej@gmail.com)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QApplication>

#include "configuration/notifier-configuration-data-manager.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/configuration/config-combo-box.h"
#include "gui/widgets/configuration/notify-group-box.h"
#include "gui/windows/configuration-window.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/path-conversion.h"

#include "activate.h"
#include "debug.h"

#include "qt4_docking_notify_configuration_widget.h"

Qt4NotifyConfigurationWidget::Qt4NotifyConfigurationWidget(QWidget *parent)
	: NotifierConfigurationWidget(parent), currentNotifyEvent("")
{
	QPushButton *configureButton = new QPushButton(tr("Configure"));
	connect(configureButton, SIGNAL(clicked()), this, SLOT(showConfigurationWindow()));

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->addWidget(configureButton);

	dynamic_cast<NotifyGroupBox *>(parent)->addWidget(this);
}

void Qt4NotifyConfigurationWidget::saveNotifyConfigurations()
{
}

void Qt4NotifyConfigurationWidget::switchToEvent(const QString &event)
{
	kdebugf();

	currentNotifyEvent = event;
}

void Qt4NotifyConfigurationWidget::showConfigurationWindow()
{
	NotifierConfigurationDataManager *dataManager = NotifierConfigurationDataManager::dataManagerForEvent(currentNotifyEvent);
	ConfigurationWindow *configWindow = new ConfigurationWindow("Qt4DockingNotifyEventConfiguration", tr("Tray icon baloon's look configuration"), "Qt4DockingNotify", dataManager);

	dataManager->configurationWindowCreated(configWindow);

	configWindow->widget()->appendUiFile(dataPath("kadu/modules/configuration/qt4-docking-notify.ui"));

	QString tooltip = qApp->translate("@default", MainConfigurationWindow::SyntaxTextNotify) +
	tr("\n%&t - title (eg. New message) %&m - notification text (eg. Message from Jim), %&d - details (eg. message quotation),\n%&i - notification icon");

	configWindow->widget()->widgetById("Title")->setToolTip(tooltip);
	configWindow->widget()->widgetById("Syntax")->setToolTip(tooltip);

	configWindow->show();
	_activateWindow(configWindow);
}
