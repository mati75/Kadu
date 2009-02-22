/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "dcc/dcc-manager.h"
#include "dcc/dcc-socket-notifiers.h"
#include "gadu-contact-account-data.h"
#include "gadu-protocol.h"

#include "gadu-file-transfer.h"

GaduFileTransfer::GaduFileTransfer(GaduProtocol *protocol, Contact peer, FileTransfer::FileTransferType transferType) :
		FileTransfer(peer, transferType), Protocol(protocol), SocketNotifiers(0), WaitingForSocketNotifiers(false)
{
}

void GaduFileTransfer::updateFileInfo()
{
	if (SocketNotifiers)
	{
		setFileSize(SocketNotifiers->fileSize());
		setFileSize(SocketNotifiers->transferredFileSize());
	}
	else
	{
		setFileSize(0);
		setTransferredSize(0);
	}
}

void GaduFileTransfer::setFileTransferNotifiers(DccSocketNotifiers *socketNotifiers)
{
	if (!socketNotifiers)
	{
		socketNotAvailable();
		return;
	}

	SocketNotifiers = socketNotifiers;
	SocketNotifiers->setGaduFileTransfer(this);
	WaitingForSocketNotifiers = false;
	GaduFileName = socketNotifiers->gaduFileName();

	changeFileTransferStatus(FileTransfer::StatusTransfer);
}

void GaduFileTransfer::socketNotAvailable()
{
	WaitingForSocketNotifiers = false;

	changeFileTransferStatus(FileTransfer::StatusFinished);
}

void GaduFileTransfer::send()
{
	if (SocketNotifiers || WaitingForSocketNotifiers) // already sending/receiving
		return;

	GaduFileName = QString::null;

	if (!Protocol || localFileName().isEmpty())
	{
		changeFileTransferStatus(FileTransfer::StatusNotConnected);
		return; // TODO: notify
	}

	GaduContactAccountData *gcad = Protocol->gaduContactAccountData(contact());
	if (!gcad)
	{
		changeFileTransferStatus(FileTransfer::StatusNotConnected);
		return;
	}

	// async call, will return in setFileTransferNotifiers
	changeFileTransferStatus(FileTransfer::StatusWaitingForConnection);
	WaitingForSocketNotifiers = true;
	Protocol->dccManager()->attachSendFileTransferSocket(this);
}
