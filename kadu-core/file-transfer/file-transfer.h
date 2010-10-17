/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef FILE_TRANSFER_H
#define FILE_TRANSFER_H

#include <QtCore/QObject>

#include "file-transfer/file-transfer-shared.h"
#include "storage/shared-base.h"

#include "exports.h"

class QFile;

class Buddy;

class KADUAPI FileTransfer : public SharedBase<FileTransferShared>
{
	KaduSharedBaseClass(FileTransfer)

public:
	static FileTransfer create();
	static FileTransfer loadStubFromStorage(const QSharedPointer<StoragePoint> &fileTransferStoragePoint);
	static FileTransfer loadFromStorage(const QSharedPointer<StoragePoint> &fileTransferStoragePoint);
	static FileTransfer null;

	FileTransfer();
	FileTransfer(FileTransferShared *data);
	FileTransfer(QObject *data);
	FileTransfer(const FileTransfer &copy);
	virtual ~FileTransfer();

	KaduSharedBase_Property(Contact, peer, Peer)
	KaduSharedBase_Property(QString, localFileName, LocalFileName)
	KaduSharedBase_Property(QString, remoteFileName, RemoteFileName)
	KaduSharedBase_Property(unsigned long, fileSize, FileSize)
	KaduSharedBase_Property(unsigned long, transferredSize, TransferredSize)
	KaduSharedBase_Property(FileTransferType, transferType, TransferType)
	KaduSharedBase_Property(FileTransferStatus, transferStatus, TransferStatus)
	KaduSharedBase_Property(FileTransferError, transferError, TransferError)
	KaduSharedBase_Property(FileTransferHandler *, handler, Handler)

	virtual bool accept(const QFile &file);

	void createHandler();
	unsigned int percent();

};

Q_DECLARE_METATYPE(FileTransfer)

#endif // FILE_TRANSFER_H
