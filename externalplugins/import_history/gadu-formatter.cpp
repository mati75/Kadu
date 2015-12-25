/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#define GG_IGNORE_DEPRECATED
#include <libgadu.h>

#include <QtGui/QApplication>

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "buddies/buddy-manager.h"
#include "configuration/configuration-file.h"
#include "contacts/contact-manager.h"
#include "gui/windows/message-dialog.h"
#include "message/formatted-message.h"

#include "gadu-account-details.h"
#include "gadu-protocol.h"

#include "gadu-formatter.h"

Q_DECLARE_TYPEINFO(FormatAttribute, Q_PRIMITIVE_TYPE);

namespace GaduFormatter
{

static unsigned int computeFormatsSize(const FormattedMessage &message)
{
	unsigned int size = sizeof(struct gg_msg_richtext);
	bool first = true;

	foreach (const FormattedMessagePart &part, message.parts())
	{
		if (!first || part.isImage() || part.bold() || part.italic() || part.underline() || part.color().isValid())
		{
			first = false;

			size += sizeof(struct gg_msg_richtext_format);

			if (part.isImage())
				size += sizeof(struct gg_msg_richtext_image);
			else if (part.color().isValid())
				size += sizeof(struct gg_msg_richtext_color);
		}
	}

	return first ? 0 : size;
}

unsigned char * createFormats(Account account, const FormattedMessage &message, unsigned int &size)
{
	size = computeFormatsSize(message);
	if (!size)
		return 0;

	unsigned char *result = new unsigned char[size];
	bool first = true;
	unsigned int memoryPosition = sizeof(struct gg_msg_richtext);
	unsigned int textPosition = 0;

	struct gg_msg_richtext header;
	struct gg_msg_richtext_format format;
	struct gg_msg_richtext_color color;
	struct gg_msg_richtext_image image;

	header.flag = 2;
	header.length = gg_fix16(size - sizeof(struct gg_msg_richtext));
	memcpy(result, &header, sizeof(header));

	foreach (const FormattedMessagePart &part, message.parts())
	{
		if (!first || part.isImage() || part.bold() || part.italic() || part.underline() || part.color().isValid())
		{
			first = false;

			format.position = gg_fix16(textPosition);
			format.font = 0;

			if (part.isImage())
				format.font |= GG_FONT_IMAGE;
			else
			{
				if (part.bold())
					format.font |= GG_FONT_BOLD;
				if (part.italic())
					format.font |= GG_FONT_ITALIC;
				if (part.underline())
					format.font |= GG_FONT_UNDERLINE;
				if (part.color().isValid())
					format.font |= GG_FONT_COLOR;
			}

			memcpy(result + memoryPosition, &format, sizeof(format));
			memoryPosition += sizeof(format);

			if (part.isImage())
			{
				quint32 size;
				quint32 crc32;

				GaduChatImageService *gcis = static_cast<GaduChatImageService *>(account.protocolHandler()->chatImageService());
				gcis->prepareImageToSend(part.imagePath(), size, crc32);

				image.unknown1 = 0x0109;
				image.size = gg_fix32(size);
				image.crc32 = gg_fix32(crc32);

				memcpy(result + memoryPosition, &image, sizeof(image));
				memoryPosition += sizeof(image);
			}
			else if (part.color().isValid())
			{
				color.red = part.color().red();
				color.green = part.color().green();
				color.blue = part.color().blue();

				memcpy(result + memoryPosition, &color, sizeof(color));
				memoryPosition += sizeof(color);
			}
		}

		textPosition += part.content().length();
	}

	return result;
}

QString createImageId(unsigned int sender, unsigned int size, unsigned int crc32)
{
	return QString("gadu-%1-%2-%3")
		.arg(sender)
		.arg(size)
		.arg(crc32);
}

static FormattedMessagePart imagePart(Account account, Contact contact, const gg_msg_richtext_image &image, bool receiveImages)
{
	quint32 size = gg_fix32(image.size);
	quint32 crc32 = gg_fix32(image.crc32);

	if (size == 20 && (crc32 == 4567 || crc32 == 99)) // fake spy images
		return FormattedMessagePart();

	if (!receiveImages)
		return FormattedMessagePart(qApp->translate("@default", "IMAGE SENT BY THIS BUDDY HAS BEEN BLOCKED"), false, true, false, QColor());

	GaduAccountDetails *details = dynamic_cast<GaduAccountDetails *>(account.details());
	if (!details)
		return FormattedMessagePart();

	if (details->limitImageSize() && size > (quint32)details->maximumImageSize() * 1024U)
	{
		bool allow = false;
		if (details->imageSizeAsk())
		{
			QString question = qApp->translate("@default",
					"Buddy %1 is attempting to send you an image of %2 KiB in size.\n"
					"This exceeds your configured limits.\n"
					"Do you want to accept this image anyway?")
					.arg(contact.display(true)).arg((size + 1023) / 1024);
			allow = MessageDialog::ask(
					KaduIcon("dialog-question"),
					qApp->translate("@default", "Kadu") + " - " + qApp->translate("@default", "Incoming Image"),
					question);
		}

		if (!allow)
			return FormattedMessagePart(qApp->translate("@default", "THIS BUDDY HAS SENT YOU AN IMAGE THAT IS TOO BIG TO BE RECEIVED"), false, true, false, QColor());
	}

	GaduChatImageService *service = account.protocolHandler()
			? qobject_cast<GaduChatImageService *>(account.protocolHandler()->chatImageService())
			: 0;

	if (!service)
		return FormattedMessagePart();

	service->sendImageRequest(contact, size, crc32);

	return FormattedMessagePart(createImageId(contact.id().toUInt(), size, crc32));
}

#define MAX_NUMBER_OF_IMAGES 5

} // namespace
