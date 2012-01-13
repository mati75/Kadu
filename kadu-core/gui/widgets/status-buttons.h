/*
 * %kadu copyright begin%
 * Copyright 2008, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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

#ifndef STATUS_BUTTONS_H
#define STATUS_BUTTONS_H

#include <QtCore/QMap>
#include <QtGui/QToolBar>

#include "status/status-container-aware-object.h"

class StatusButton;
class StatusContainer;

class KADUAPI StatusButtons : public QToolBar, private StatusContainerAwareObject
{
	Q_OBJECT

	QMap<StatusContainer *, StatusButton *> Buttons;

	void enableStatusName();
	void disableStatusName();

protected:
	virtual void statusContainerRegistered(StatusContainer *statusContainer);
	virtual void statusContainerUnregistered(StatusContainer *statusContainer);

public:
	explicit StatusButtons(QWidget *parent = 0);
	virtual ~StatusButtons();

};

#endif // STATUS_BUTTONS_H
