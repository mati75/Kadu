/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtCore/QVariant>

#include "configuration/xml-configuration-file.h"
#include "storage/storage-point.h"

#include "misc/misc.h"

#include "contact-notify-data.h"

ContactNotifyData::ContactNotifyData(StorableObject *parent)
		: ModuleData(parent), Notify(false)
{
}

ContactNotifyData::~ContactNotifyData()
{
}

void ContactNotifyData::load()
{
	if (!isValidStorage())
		return;

	StorableObject::load();

	Notify = loadValue<bool>("Notify", false);
}

void ContactNotifyData::store()
{
	if (!isValidStorage())
		return;

	storeValue("Notify", Notify);
}

QString ContactNotifyData::name() const
{
	return QLatin1String("notify");
}
