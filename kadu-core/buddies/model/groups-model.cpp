/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2010 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include "buddies/group.h"
#include "buddies/group-manager.h"

#include "model/roles.h"

#include "protocols/protocol.h"

#include "groups-model.h"

GroupsModel::GroupsModel(QObject *parent)
	: QAbstractListModel(parent)
{
	connect(GroupManager::instance(), SIGNAL(groupAboutToBeAdded(Group)),
			this, SLOT(groupAboutToBeAdded(Group)));
	connect(GroupManager::instance(), SIGNAL(groupAdded(Group)),
			this, SLOT(groupAdded(Group)));
	connect(GroupManager::instance(), SIGNAL(groupAboutToBeRemoved(Group)),
			this, SLOT(groupAboutToBeRemoved(Group)));
	connect(GroupManager::instance(), SIGNAL(groupRemoved(Group)),
			this, SLOT(groupRemoved(Group)));
}

GroupsModel::~GroupsModel()
{
	disconnect(GroupManager::instance(), SIGNAL(groupAboutToBeAdded(Group)),
			this, SLOT(groupAboutToBeAdded(Group)));
	disconnect(GroupManager::instance(), SIGNAL(groupAdded(Group)),
			this, SLOT(groupAdded(Group)));
	disconnect(GroupManager::instance(), SIGNAL(groupAboutToBeRemoved(Group)),
			this, SLOT(groupAboutToBeRemoved(Group)));
	disconnect(GroupManager::instance(), SIGNAL(groupRemoved(Group)),
			this, SLOT(groupRemoved(Group)));
}

int GroupsModel::rowCount(const QModelIndex &parent) const
{
	return parent.isValid() ? 0 : GroupManager::instance()->count();
}

QVariant GroupsModel::data(const QModelIndex &index, int role) const
{
	Group grp = group(index);
	if (0 == grp)
		return QVariant();

	switch (role)
	{
		case Qt::DisplayRole:
			return grp.name();
		case Qt::DecorationRole:
			return grp.icon();
		case GroupRole:
			return QVariant::fromValue(grp);
		case ItemTypeRole:
			return GroupRole;
		default:
			return QVariant();
	}
}

QVariant GroupsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal)
		return QString("Column %1").arg(section);
	else
		return QString("Row %1").arg(section);
}

Group GroupsModel::group(const QModelIndex &index) const
{
	if (!index.isValid())
		return Group::null;

	if (index.row() < 0 || index.row() >= rowCount())
		return Group::null;

	return GroupManager::instance()->byIndex(index.row());
}

int GroupsModel::groupIndex(Group group) const
{
	return GroupManager::instance()->indexOf(group);
}


QModelIndex GroupsModel::indexForValue(const QVariant &value) const
{
	return createIndex(groupIndex(value.value<Group>()), 0, 0);
}

void GroupsModel::groupAboutToBeAdded(Group group)
{
	Q_UNUSED(group)

	int count = rowCount();
	beginInsertRows(QModelIndex(), count, count);
}

void GroupsModel::groupAdded(Group group)
{
	Q_UNUSED(group)

	endInsertRows();
}

void GroupsModel::groupAboutToBeRemoved(Group group)
{
	int index = groupIndex(group);
	beginRemoveRows(QModelIndex(), index, index);
}

void GroupsModel::groupRemoved(Group group)
{
	Q_UNUSED(group)

	endRemoveRows();
}
