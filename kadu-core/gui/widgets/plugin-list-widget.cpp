/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * %kadu copyright end%
 *
 * This file is derived from part of the KDE project
 * Copyright (C) 2007, 2006 Rafael Fernández López <ereslibre@kde.org>
 * Copyright (C) 2002-2003 Matthias Kretz <kretz@kde.org>
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

#define MARGIN 5

#include <QtGui/QApplication>
#include <QtGui/QBoxLayout>
#include <QtGui/QCheckBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPainter>
#include <QtGui/QPushButton>
#include <QtGui/QStyleOptionViewItemV4>

#include "configuration/configuration-manager.h"
#include "gui/widgets/categorized-list-view.h"
#include "gui/widgets/categorized-list-view-painter.h"
#include "gui/widgets/filter-widget.h"
#include "gui/widgets/plugin-list-view-delegate.h"
#include "gui/widgets/plugin-list-widget-item-delegate.h"
#include "gui/windows/message-dialog.h"
#include "icons/kadu-icon.h"
#include "model/categorized-sort-filter-proxy-model.h"
#include "plugins/model/plugin-model.h"
#include "plugins/model/plugin-proxy-model.h"
#include "plugins/plugin.h"
#include "plugins/plugin-info.h"
#include "plugins/plugins-manager.h"

#include "plugin-list-widget.h"


PluginListWidget::PluginListWidget(QWidget *parent) :
		QWidget(parent), ListView(0), ShowIcons(false)
{
	QVBoxLayout *layout = new QVBoxLayout;
	layout->setMargin(0);
	setLayout(layout);

	LineEdit = new FilterWidget(this);
	LineEdit->setAutoVisibility(false);
	ListView = new CategorizedListView(this);
	ListView->setVerticalScrollMode(QListView::ScrollPerPixel);
	ListView->setAlternatingRowColors(true);
	CategoryDrawer = new CategorizedListViewPainter(ListView);
	ListView->setCategoryDrawer(CategoryDrawer);

	Model = new PluginModel(this, this);
	Proxy = new ProxyModel(this, this);
	Proxy->setCategorizedModel(true);
	Proxy->setSourceModel(Model);
	Model->loadPluginData();
	ListView->setModel(Proxy);
	ListView->setAlternatingRowColors(true);

	Delegaet = new PluginListWidgetItemDelegate(this, this);
	ListView->setItemDelegate(Delegaet);

	ListView->setMouseTracking(true);
	ListView->viewport()->setAttribute(Qt::WA_Hover);

	LineEdit->setView(ListView);

	connect(LineEdit, SIGNAL(textChanged(QString)), Proxy, SLOT(invalidate()));
	connect(Delegaet, SIGNAL(changed(bool)), this, SIGNAL(changed(bool)));
	connect(Delegaet, SIGNAL(configCommitted(QByteArray)), this, SIGNAL(configCommitted(QByteArray)));

	layout->addWidget(LineEdit);
	layout->addWidget(ListView);
}

PluginListWidget::~PluginListWidget()
{
	delete ListView->itemDelegate();
	delete ListView;
	delete CategoryDrawer;
}

int PluginListWidget::dependantLayoutValue(int value, int width, int totalWidth) const
{
	if (ListView->layoutDirection() == Qt::LeftToRight)
		return value;

	return totalWidth - width - value;
}

void PluginListWidget::applyChanges()
{
	bool changeOccured = false;

	for (int i = 0; i < Model->rowCount(); i++)
	{
		const QModelIndex index = Model->index(i, 0);
		PluginEntry *pluginEntry = static_cast<PluginEntry*>(index.internalPointer());

		Plugin *plugin = PluginsManager::instance()->plugins().value(pluginEntry->pluginName);

		if (plugin && plugin->isActive() != pluginEntry->checked)
		{
			if (pluginEntry->checked)
				PluginsManager::instance()->activatePlugin(plugin, PluginActivationReasonUserRequest);
			else
				PluginsManager::instance()->deactivatePlugin(plugin, PluginDeactivationReasonUserRequest);

			changeOccured = true;
		}
	}

	Model->loadPluginData();

	if (changeOccured)
		ConfigurationManager::instance()->flush();

	emit changed(false);
}