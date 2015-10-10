/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtWidgets/QAction>

#include "model/model-chain.h"
#include "model/roles.h"
#include "protocols/model/protocols-model-proxy.h"
#include "protocols/model/protocols-model.h"

#include "protocols-combo-box.h"

ProtocolsComboBox::ProtocolsComboBox(QWidget *parent) :
		ActionsComboBox(parent)
{
	addBeforeAction(new QAction(tr(" - Select network - "), this), NotVisibleWithOneRowSourceModel);

	ProxyModel = new ProtocolsModelProxy(this);
	ModelChain *chain = new ModelChain(this);
	chain->setBaseModel(new ProtocolsModel(chain));
	chain->addProxyModel(ProxyModel);
	setUpModel(ProtocolRole, chain);
}

ProtocolsComboBox::~ProtocolsComboBox()
{
}

void ProtocolsComboBox::setCurrentProtocol(ProtocolFactory *protocol)
{
	setCurrentValue(QVariant::fromValue<ProtocolFactory *>(protocol));
}

ProtocolFactory * ProtocolsComboBox::currentProtocol()
{
	return currentValue().value<ProtocolFactory *>();
}

void ProtocolsComboBox::addFilter(AbstractProtocolFilter *filter)
{
	ProxyModel->addFilter(filter);
}

void ProtocolsComboBox::removeFilter(AbstractProtocolFilter *filter)
{
	ProxyModel->removeFilter(filter);
}

#include "moc_protocols-combo-box.cpp"
