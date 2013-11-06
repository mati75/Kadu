/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "misc/error.h"
#include "debug.h"

#include "server/gadu-connection.h"
#include "server/gadu-writable-session-token.h"
#include "gadu-account-details.h"

#include "gadu-chat-image-service.h"

GaduChatImageService::GaduChatImageService(Account account, QObject *parent) :
		ChatImageService(account, parent), ReceiveImages(false)
{
}

GaduChatImageService::~GaduChatImageService()
{
}

void GaduChatImageService::setConnection(GaduConnection *connection)
{
	Connection = connection;
}

void GaduChatImageService::setGaduChatService(GaduChatService *gaduChatService)
{
	if (CurrentChatService)
		disconnect(CurrentChatService.data(), 0, this, 0);

	CurrentChatService = gaduChatService;
	if (CurrentChatService)
		connect(CurrentChatService.data(), SIGNAL(chatImageKeyReceived(QString,ChatImageKey)),
		        this, SLOT(chatImageKeyReceivedSlot(QString,ChatImageKey)));
}

void GaduChatImageService::setReceiveImages(bool receiveImages)
{
	ReceiveImages = receiveImages;
}

void GaduChatImageService::handleEventImageRequest(struct gg_event *e)
{
	kdebugm(KDEBUG_INFO, "%s", qPrintable(QString("Received image request. sender: %1, size: %2, crc32: %3\n")
		.arg(e->event.image_request.sender).arg(e->event.image_request.size).arg(e->event.image_request.crc32)));

	if (!Connection || !Connection.data()->hasSession())
		return;

	ChatImageKey key(e->event.image_request.size, e->event.image_request.crc32);
	if (!ChatImages.contains(key))
		return;

	QByteArray content = ChatImages.value(key);
	if (content.isEmpty())
		return;

	auto writableSessionToken = Connection.data()->writableSessionToken();
	gg_image_reply(writableSessionToken.rawSession(), e->event.image_request.sender, key.toString().toUtf8().constData(),
			content.constData(), content.length());
}

void GaduChatImageService::handleEventImageReply(struct gg_event *e)
{
	kdebugm(KDEBUG_INFO, "%s", qPrintable(QString("Received image. sender: %1, size: %2, crc32: %3,filename: %4\n")
			.arg(e->event.image_reply.sender).arg(e->event.image_reply.size)
			.arg(e->event.image_reply.crc32).arg(e->event.image_reply.filename)));

	ChatImageKey key(e->event.image_reply.size, e->event.image_reply.crc32);
	QByteArray imageData(e->event.image_reply.image, e->event.image_reply.size);

	if (key.isNull() || imageData.isEmpty())
		return;

	emit chatImageAvailable(key, imageData);
}

void GaduChatImageService::chatImageKeyReceivedSlot(const QString &id, const ChatImageKey &imageKey)
{
	if (ReceiveImages)
		emit chatImageKeyReceived(id, imageKey);
}

void GaduChatImageService::requestChatImage(const QString &id, const ChatImageKey &imageKey)
{
	if (!Connection || !Connection.data()->hasSession())
		return;

	if (id.isEmpty() || imageKey.isNull())
		return;

	auto writableSessionToken = Connection.data()->writableSessionToken();
	gg_image_request(writableSessionToken.rawSession(), id.toUInt(), imageKey.size(), imageKey.crc32());
}

ChatImageKey GaduChatImageService::prepareImageToBeSent(const QByteArray &imageData)
{
	quint32 crc32 = imageData.isEmpty() ? 0 : gg_crc32(0, (const unsigned char*)imageData.constData(), imageData.length());

	ChatImageKey result(imageData.size(), crc32);
	ChatImages.insert(result, imageData);
	return result;
}

Error GaduChatImageService::checkImageSize(qint64 size) const
{
	GaduAccountDetails *details = dynamic_cast<GaduAccountDetails *>(account().details());
	if (!details || !details->chatImageSizeWarning() || size <= RECOMMENDED_MAXIMUM_SIZE)
		return Error(NoError, QString());

	QString message = tr("This image has %1 KiB and exceeds recommended maximum size of %2 KiB. Some clients may have trouble with too large images.")
		+ '\n' + tr("Do you really want to send this image?");
	message = message.arg((size + 1023) / 1024).arg(RECOMMENDED_MAXIMUM_SIZE / 1024);

	return Error(ErrorLow, message);
}

#include "moc_gadu-chat-image-service.cpp"
