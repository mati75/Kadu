/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ACTIONS_H
#define ACTIONS_H

#include <QtCore/QMap>
#include <QtCore/QObject>

#include "exports.h"

class QAction;

class Action;
class ActionDescription;
class MainWindow;

class KADUAPI Actions : public QObject, public QMap<QString, ActionDescription *>
{
	Q_OBJECT

	void insert(ActionDescription *action);
	void remove(ActionDescription *action);
	friend class ActionDescription;

public:
	Actions();
	virtual ~Actions() {}

	QAction * createAction(const QString &name, MainWindow *kaduMainWindow);

signals:
	void actionCreated(Action *);
	void actionLoaded(const QString &actionName);
	void actionUnloaded(const QString &actionName);

};

extern KADUAPI Actions KaduActions;

#endif // ACTIONS_H
