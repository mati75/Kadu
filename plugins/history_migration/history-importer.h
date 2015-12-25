/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef HISTORY_IMPORTER_H
#define HISTORY_IMPORTER_H

#include <QtCore/QObject>

#include "accounts/account.h"

#include "history_migration_exports.h"

class HistoryImportThread;
class HistoryImportWindow;

class HISTORY_MIGRATION_API HistoryImporter : public QObject
{
	Q_OBJECT

	Account DestinationAccount;
	QString SourceDirectory;

	QThread *Thread;
	HistoryImportThread *HistoryImport;
	HistoryImportWindow *ProgressWindow;

private slots:
	void updateProgressWindow();
	void threadFinished();

public:
	explicit HistoryImporter(const Account &account, const QString &path, QObject *parent = 0);
	virtual ~HistoryImporter();

	const QString & sourceDirectory() const { return SourceDirectory; }

public slots:
	void run();

};

#endif // HISTORY_IMPORTER_H
