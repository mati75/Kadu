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

#ifndef BUDDY_STATUS_DATES_MODEL_H
#define BUDDY_STATUS_DATES_MODEL_H

#include <QtCore/QAbstractListModel>
#include <QtCore/QDate>
#include <QtCore/QList>

#include "buddies/buddy.h"

class BuddyStatusDatesModel : public QAbstractListModel
{
	Q_OBJECT

	struct ItemCachedData
	{
		int size;
	};

	Buddy MyBuddy;
	QList<QDate> Dates;
	QMap<QDate, ItemCachedData> *Cache;

	int fetchSize(QDate date) const;
	ItemCachedData fetchCachedData(QDate date) const;

public:
	BuddyStatusDatesModel(Buddy buddy, QList<QDate> dates, QObject *parent = 0);
	virtual ~BuddyStatusDatesModel();

	virtual int columnCount(const QModelIndex &parent) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

	void setBuddy(Buddy buddy);
	void setDates(QList<QDate> dates);

	QModelIndex indexForDate(QDate date);

};

#endif // BUDDY_STATUS_DATES_MODEL_H
