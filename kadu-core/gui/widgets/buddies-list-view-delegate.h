/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
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

#ifndef BUDDIES_LIST_VIEW_DELEGATE_H
#define BUDDIES_LIST_VIEW_DELEGATE_H

#include <QtGui/QItemDelegate>

#include "gui/widgets/buddies-list-view-delegate-configuration.h"

class AbstractBuddiesModel;
class Account;
class Avatar;
class Contact;
class Status;

class BuddiesListViewDelegate : public QItemDelegate
{
	Q_OBJECT

	AbstractBuddiesModel *Model;
	BuddiesListViewDelegateConfiguration Configuration;

	QStyleOptionViewItemV4 getOptions(const QModelIndex &index, const QStyleOptionViewItem &option) const;

private slots:
	void avatarUpdated(Avatar avatar);
	void contactUpdated(Contact &contact);
	void modelDestroyed();

public:
	explicit BuddiesListViewDelegate(QObject *parent = 0);
	virtual ~BuddiesListViewDelegate();

	virtual void setModel(AbstractBuddiesModel *model);

	virtual void setShowAccountName(bool show);

	BuddiesListViewDelegateConfiguration & configuration() { return Configuration; }

	virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

};

// for MOC
#include "buddies/avatar.h"
#include "contacts/contact.h"

#endif // BUDDIES_LIST_VIEW_DELEGATE_H
