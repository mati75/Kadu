/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include "accounts/account-manager.h"
#include "buddies/filter/abstract-buddy-filter.h"
#include "buddies/buddy.h"
#include "contacts/filter/abstract-contact-filter.h"
#include "contacts/contact.h"
#include "status/status.h"
#include "status/status-type.h"

#include "buddies-model.h"

#include "buddies-model-proxy.h"

BuddiesModelProxy::BuddiesModelProxy(QObject *parent)
	: QSortFilterProxyModel(parent), SourceBuddyModel(0), SortByStatus(true)
{
	setDynamicSortFilter(true);
	sort(0);

	BrokenStringCompare = (QString("a").localeAwareCompare(QString("B")) > 0);
	if (BrokenStringCompare)
		fprintf(stderr, "There's something wrong with native string compare function. Applying workaround (slower).\n");
}

BuddiesModelProxy::~BuddiesModelProxy()
{

}

void BuddiesModelProxy::setSourceModel(QAbstractItemModel *sourceModel)
{
	SourceBuddyModel = dynamic_cast<AbstractBuddiesModel *>(sourceModel);
	QSortFilterProxyModel::setSourceModel(sourceModel);

	if (sourceModel)
		connect(sourceModel, SIGNAL(destroyed(QObject *)), this, SLOT(modelDestroyed()));

	setDynamicSortFilter(true);
	sort(0);
}

int BuddiesModelProxy::compareNames(QString n1, QString n2) const
{
	return BrokenStringCompare
		? n1.toLower().localeAwareCompare(n2.toLower())
		: n1.localeAwareCompare(n2);
}

void BuddiesModelProxy::setSortByStatus(bool sortByStatus)
{
	if (SortByStatus == sortByStatus)
		return;

	SortByStatus = sortByStatus;
	invalidateFilter();
	invalidate();

	sort(1);
	sort(0);
}

void BuddiesModelProxy::modelDestroyed()
{
	SourceBuddyModel = 0;
}

bool BuddiesModelProxy::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
	if (!SourceBuddyModel)
		return QSortFilterProxyModel::lessThan(left, right);

	Buddy leftBuddy = SourceBuddyModel->buddyAt(left);
	Buddy rightBuddy = SourceBuddyModel->buddyAt(right);

	if (leftBuddy.isBlocked() && !rightBuddy.isBlocked())
		return false;
	if (!leftBuddy.isBlocked() && rightBuddy.isBlocked())
		return true;

	if (SortByStatus)
	{
		Account leftAccount = leftBuddy.prefferedAccount();
		Account rightAccount = rightBuddy.prefferedAccount();

		Contact leftContact = leftBuddy.prefferedContact();
		Contact rightContact = rightBuddy.prefferedContact();

		if (leftContact.isBlocking() && !rightContact.isBlocking())
			return false;

		if (!leftContact.isBlocking() && rightContact.isBlocking())
			return true;
		

		Status leftStatus = !leftContact.isNull()
				? leftContact.currentStatus()
				: Status::null;
		Status rightStatus = !rightContact.isNull()
				? rightContact.currentStatus()
				: Status::null;

		if (leftStatus.isDisconnected() && !rightStatus.isDisconnected())
			return false;

		if (!leftStatus.isDisconnected() && rightStatus.isDisconnected())
			return true;
	}

	int displayCompare = compareNames(leftBuddy.display(), rightBuddy.display());
	return displayCompare < 0;
}

bool BuddiesModelProxy::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
	if (sourceParent.isValid())
	{
		Contact contact = SourceBuddyModel->contactAt(sourceModel()->index(sourceRow, 0, sourceParent));
		foreach (AbstractContactFilter *filter, ContactFilters)
			if (!filter->acceptContact(contact))
				return false;
	}
	else
	{
		Buddy buddy = SourceBuddyModel->buddyAt(sourceModel()->index(sourceRow, 0, sourceParent));
		foreach (AbstractBuddyFilter *filter, BuddyFilters)
			if (!filter->acceptBuddy(buddy))
				return false;
	}

	return true;
}

void BuddiesModelProxy::addFilter(AbstractBuddyFilter *filter)
{
	BuddyFilters.append(filter);
	invalidateFilter();
	connect(filter, SIGNAL(filterChanged()), this, SLOT(invalidate()));
}

void BuddiesModelProxy::removeFilter(AbstractBuddyFilter *filter)
{
	BuddyFilters.removeAll(filter);
	invalidateFilter();
	connect(filter, SIGNAL(filterChanged()), this, SLOT(invalidate()));
}

void BuddiesModelProxy::addFilter(AbstractContactFilter *filter)
{
	ContactFilters.append(filter);
	invalidateFilter();
	connect(filter, SIGNAL(filterChanged()), this, SLOT(invalidate()));
}

void BuddiesModelProxy::removeFilter(AbstractContactFilter *filter)
{
	ContactFilters.removeAll(filter);
	invalidateFilter();
	connect(filter, SIGNAL(filterChanged()), this, SLOT(invalidate()));
}

Buddy BuddiesModelProxy::buddyAt(const QModelIndex &index) const
{
	if (!SourceBuddyModel)
		return Buddy::null;

	return SourceBuddyModel->buddyAt(mapToSource(index));
}

const QModelIndex BuddiesModelProxy::buddyIndex(Buddy buddy) const
{
	if (!SourceBuddyModel)
		return QModelIndex();

	return mapFromSource(SourceBuddyModel->buddyIndex(buddy));
}
