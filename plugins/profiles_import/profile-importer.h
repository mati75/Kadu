/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef PROFILE_IMPORTER_H
#define PROFILE_IMPORTER_H

#include <QtCore/QFile>

#include "accounts/account.h"

class ProfileImporter : public QObject
{
	Q_OBJECT

	QString ProfileFileName;
	QString ErrorMessage;
	Account ResultAccount;

public:
	ProfileImporter(const QString &profileFileName);

	bool import(const Identity &identity);
	Account resultAccount() { return ResultAccount; }

	QString errorMessage();
	
};

#endif // PROFILE_IMPORTER_H
