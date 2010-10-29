/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QApplication>
#include <QtGui/QLabel>

#include "gui/widgets/configuration/config-hot-key-edit.h"
#include "gui/widgets/configuration/config-group-box.h"

#include "debug.h"

ConfigHotKeyEdit::ConfigHotKeyEdit(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager) :
		HotKeyEdit(parentConfigGroupBox->widget()), ConfigWidgetValue(section, item, widgetCaption, toolTip, parentConfigGroupBox, dataManager), label(0)
{
	createWidgets();
}

ConfigHotKeyEdit::ConfigHotKeyEdit(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager) :
		HotKeyEdit(parentConfigGroupBox->widget()), ConfigWidgetValue(parentConfigGroupBox, dataManager), label(0)
{
}

ConfigHotKeyEdit::~ConfigHotKeyEdit()
{
	if (label)
		delete label;
}

void ConfigHotKeyEdit::createWidgets()
{
	kdebugf();

	label = new QLabel(qApp->translate("@default", widgetCaption.toAscii().data()) + ':', parentConfigGroupBox->widget());
	parentConfigGroupBox->addWidgets(label, this);

	if (!ConfigWidget::toolTip.isEmpty())
	{
		setToolTip(qApp->translate("@default", ConfigWidget::toolTip.toAscii().data()));
		label->setToolTip(qApp->translate("@default", ConfigWidget::toolTip.toAscii().data()));
	}
}

void ConfigHotKeyEdit::loadConfiguration()
{
	if (!dataManager)
		return;
	setShortCut(dataManager->readEntry(section, item).toString());
}

void ConfigHotKeyEdit::saveConfiguration()
{
	if (!dataManager)
		return;
	dataManager->writeEntry(section, item, QVariant(shortCutString()));
}

void ConfigHotKeyEdit::show()
{
	label->show();
	HotKeyEdit::show();
}

void ConfigHotKeyEdit::hide()
{
	label->hide();
	HotKeyEdit::hide();
}
