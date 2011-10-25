/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "gui/windows/proxy-edit-window.h"
#include "model/roles.h"
#include "network/proxy/model/network-proxy-model.h"
#include "network/proxy/model/network-proxy-proxy-model.h"

#include "proxy-combo-box.h"

#define DEFAULT_PROXY_INDEX 1

ProxyComboBox::ProxyComboBox(QWidget *parent) :
		KaduComboBox(parent), InActivatedSlot(false), DefaultProxyAction(0)
{
	setDataRole(NetworkProxyRole);

	Model = new NetworkProxyModel(this);
	setUpModel(Model, new NetworkProxyProxyModel(this));
	addBeforeAction(new QAction(tr(" - No proxy - "), this));

	EditProxyAction = new QAction(tr("Edit proxy configuration..."), this);
	QFont editProxyActionFont = EditProxyAction->font();
	editProxyActionFont.setItalic(true);
	EditProxyAction->setFont(editProxyActionFont);
	EditProxyAction->setData("editProxyConfiguration");
	addAfterAction(EditProxyAction);

	connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChangedSlot(int)));
}

ProxyComboBox::~ProxyComboBox()
{
}

void ProxyComboBox::enableDefaultProxyAction()
{
	DefaultProxyAction = new QAction(tr(" - Use Default Proxy - "), this);
	DefaultProxyAction->setData("defaultProxy");
	DefaultProxyAction->setFont(QFont());
	addBeforeAction(DefaultProxyAction);
}

void ProxyComboBox::selectDefaultProxy()
{
	if (DefaultProxyAction)
		setCurrentIndex(DEFAULT_PROXY_INDEX);
}

bool ProxyComboBox::isDefaultProxySelected()
{
	if (DefaultProxyAction)
		return DEFAULT_PROXY_INDEX == currentIndex();
	else
		return false;
}

void ProxyComboBox::setCurrentProxy(const NetworkProxy &networkProxy)
{
	setCurrentValue(networkProxy);
}

NetworkProxy ProxyComboBox::currentProxy()
{
	return currentValue().value<NetworkProxy>();
}

void ProxyComboBox::currentIndexChangedSlot(int index)
{
	QModelIndex modelIndex = this->model()->index(index, modelColumn(), rootModelIndex());
	QAction *action = modelIndex.data(ActionRole).value<QAction *>();

	if (action == EditProxyAction)
	{
		ProxyEditWindow::show();
		setCurrentValue(CurrentValue);
		return;
	}

	if (KaduComboBox::currentIndexChangedSlot(index))
		emit proxyChanged(CurrentValue.value<NetworkProxy>(), ValueBeforeChange.value<NetworkProxy>());
}

bool ProxyComboBox::compare(QVariant value, QVariant previousValue) const
{
	return value.value<NetworkProxy>() == previousValue.value<NetworkProxy>();
}
