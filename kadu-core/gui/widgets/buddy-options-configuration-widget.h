/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2002, 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002, 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2005 Marcin Ślusarz (joi@kadu.net)
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

#ifndef BUDDY_OPTIONS_CONFIGURATION_WIDGET_H
#define BUDDY_OPTIONS_CONFIGURATION_WIDGET_H

#include <QtGui/QWidget>

#include "buddies/buddy.h"
#include "configuration/configuration-aware-object.h"

#include "exports.h"

class QCheckBox;

class KADUAPI BuddyOptionsConfigurationWidget : public QWidget, ConfigurationAwareObject
{
	Q_OBJECT

	Buddy MyBuddy;

	QCheckBox *BlockCheckBox;
	QCheckBox *OfflineToCheckBox;
	QCheckBox *NotifyCheckBox;
	QCheckBox *HideDescriptionCheckBox;

	void createGui();

private slots:
	void updateOfflineTo();
	void offlineToToggled(bool toggled);

protected:
	virtual void configurationUpdated();

public:
	explicit BuddyOptionsConfigurationWidget(const Buddy &buddy, QWidget *parent = 0);
	virtual ~BuddyOptionsConfigurationWidget();

	void save();

};

#endif // BUDDY_OPTIONS_CONFIGURATION_WIDGET_H