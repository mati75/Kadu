/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QAction>
#include <QtGui/QInputDialog>
#include <QtGui/QLineEdit>

#include "identities/identity-manager.h"
#include "identities/model/identity-model.h"
#include "model/roles.h"

#include "identities-combo-box.h"

IdentitiesComboBox::IdentitiesComboBox(QWidget *parent) :
		KaduComboBox<Identity>(parent)
{
	setUpModel(new IdentityModel(this));

	CreateNewIdentityAction = new QAction(tr("Create a new identity..."), this);
	CreateNewIdentityAction->setData("createNewIdentity");

	ActionsModel->addAfterAction(CreateNewIdentityAction);

	setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

	connect(model(), SIGNAL(rowsAboutToBeRemoved(const QModelIndex &, int, int)),
			this, SLOT(updateValueBeforeChange()));
	connect(model(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
			this, SLOT(rowsRemoved(const QModelIndex &, int, int)));
	connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChangedSlot(int)));
}

IdentitiesComboBox::~IdentitiesComboBox()
{
}

void IdentitiesComboBox::setCurrentIdentity(Identity identity)
{
	if (setCurrentValue(identity))
		emit identityChanged(CurrentValue);
}

Identity IdentitiesComboBox::currentIdentity()
{
	return currentValue();
}

void IdentitiesComboBox::currentIndexChangedSlot(int index)
{
	QModelIndex modelIndex = this->model()->index(index, modelColumn(), rootModelIndex());
	QAction *action = modelIndex.data(ActionRole).value<QAction *>();

	if (action != CreateNewIdentityAction)
	{
		if (KaduComboBox<Identity>::currentIndexChangedSlot(index))
			emit identityChanged(CurrentValue);
		return;
	}

	bool ok;

	QString identityName = QInputDialog::getText(this, tr("New Identity"),
			tr("Please enter the name for the new identity:"), QLineEdit::Normal,
			QString::null, &ok);

	if (!ok || identityName.isEmpty() || IdentityManager::instance()->byName(identityName, false))
	{
		setCurrentIndex(0);
		return;
	}

	setCurrentIdentity(IdentityManager::instance()->byName(identityName, true));
}

void IdentitiesComboBox::updateValueBeforeChange()
{
	KaduComboBox<Identity>::updateValueBeforeChange();
}

void IdentitiesComboBox::rowsRemoved(const QModelIndex &parent, int start, int end)
{
	KaduComboBox<Identity>::rowsRemoved(parent, start, end);
}

int IdentitiesComboBox::preferredDataRole() const
{
	return IdentityRole;
}

QString IdentitiesComboBox::selectString() const
{
	return tr(" - Select identity - ");
}
