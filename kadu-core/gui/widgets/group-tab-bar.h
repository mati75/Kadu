/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef GROUP_TAB_BAR_H
#define GROUP_TAB_BAR_H

#include <QtGui/QTabBar>

#include "configuration/configuration-aware-object.h"
#include "buddies/buddy-list.h"
#include "buddies/group.h"

class GroupBuddyFilter;

class GroupTabBar : public QTabBar, ConfigurationAwareObject
{
	Q_OBJECT

	GroupBuddyFilter *Filter;
	//for dnd support
	Group currentGroup;
	BuddyList currentBuddies;

	bool showAllGroup;

	int AutoGroupTabPosition;

	void updateGroup(Group group);

private slots:
	void currentChangedSlot(int index);
	void groupAdded(Group group);
	void groupRemoved(Group group);

	void groupUpdated();

	void addBuddy();
	void renameGroup();
	void deleteGroup();
	void createNewGroup();
	void groupProperties();

 	void addToGroup();
 	void moveToGroup();

	void saveGroupTabsPosition();

protected:
	void contextMenuEvent(QContextMenuEvent *event);

	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);

	virtual void configurationUpdated();

public:
	GroupTabBar(QWidget *parent = 0);
	~GroupTabBar();

	void addGroup(const Group group);

	GroupBuddyFilter * filter() { return Filter; }

signals:
	void currentGroupChanged(const Group group);

};

#endif // GROUP_TAB_BAR_H
