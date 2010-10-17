/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#define GG_IGNORE_DEPRECATED
#include <libgadu.h>

#include <QtCore/QFile>
#include <QtGui/QApplication>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "contacts/contact-manager.h"
#include "chat/message/formatted-message.h"
#include "configuration/configuration-file.h"
#include "buddies/buddy-manager.h"

#include "gadu-account-details.h"
#include "gadu-protocol.h"

#include "gadu-formatter.h"

unsigned int GaduFormater::computeFormatsSize(const FormattedMessage &message)
{
	unsigned int size = sizeof(struct gg_msg_richtext);
	bool first = true;

	foreach (const FormattedMessagePart part, message.parts())
	{
		if (!first || part.isImage() || part.bold() || part.italic() || part.underline() || part.color().isValid())
		{
			size += sizeof(struct gg_msg_richtext_format);
			first = false;
		}

		if (part.isImage())
		{
			size += sizeof(struct gg_msg_richtext_image);
			first = false;
			continue;
		}

		if (part.color().isValid())
		{
			size += sizeof(struct gg_msg_richtext_color);
			first = false;
		}
	}

	return first ? 0 : size;
}

unsigned char * GaduFormater::createFormats(Account account, const FormattedMessage &message, unsigned int &size)
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

	foreach (FormattedMessagePart part, message.parts())
	{
		if (first && !part.isImage() && !part.bold() && !part.italic() && !part.underline() && !part.color().isValid())
		{
			first = false;
			textPosition += part.content().length();
			continue;
		}

		format.position = gg_fix16(textPosition);
		format.font = 0;

		if (part.isImage())
		{
			format.font |= GG_FONT_IMAGE;
		}
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
			uint32_t size;
			uint32_t crc32;

			GaduChatImageService *gcis = dynamic_cast<GaduChatImageService *>(account.protocolHandler()->chatImageService());
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

		textPosition += part.content().length();
	}

	return result;
}

void GaduFormater::appendToMessage(Account account, FormattedMessage &result, UinType sender, const QString &content,
		struct gg_msg_richtext_format &format,
		struct gg_msg_richtext_color &color, struct gg_msg_richtext_image &image, bool receiveImages)
{
	QColor textColor;

	if (format.font & GG_FONT_IMAGE)
	{
		int32_t size = gg_fix32(image.size);
		int32_t crc32 = gg_fix32(image.crc32);

		if (size == 20 && (crc32 == 4567 || crc32 == 99)) // fake spy images
			return;

		QString file_name = GaduChatImageService::imageFileName(sender, size, crc32);
		QFile file(file_name);

		if (!receiveImages)
		{
			result << FormattedMessagePart(qApp->translate("@default", QT_TR_NOOP("###IMAGE BLOCKED###")), false, false, false, textColor);
			return;
		}

		GaduAccountDetails *details = dynamic_cast<GaduAccountDetails *>(account.details());
		if (!details)
			return;

		if (size > details->maximumImageSize() * 1024)
		{
			result << FormattedMessagePart(qApp->translate("@default", QT_TR_NOOP("###IMAGE TOO BIG###")), false, false, false, textColor);
			return;
		}

		// TODO: fix
		GaduProtocol *gadu = dynamic_cast<GaduProtocol *>(account.protocolHandler());
		if (gadu)
		{
			dynamic_cast<GaduChatImageService *>(gadu->chatImageService())->
					sendImageRequest(ContactManager::instance()->byId(account, QString::number(sender)), size, crc32);
			result << FormattedMessagePart(file_name, !file.exists());
		}
	}
	else
	{
		if (format.font & GG_FONT_COLOR)
		{
			textColor.setRed(color.red);
			textColor.setGreen(color.green);
			textColor.setBlue(color.blue);
		}

		result << FormattedMessagePart(content, format.font & GG_FONT_BOLD, format.font & GG_FONT_ITALIC, format.font & GG_FONT_UNDERLINE, textColor);
	}
}

#define MAX_NUMBER_OF_IMAGES 5

QString GaduFormater::createImageId(unsigned int sender, unsigned int size, unsigned int crc32)
{
	return QString("gadu-%1-%2-%3")
		.arg(sender)
		.arg(size)
		.arg(crc32);
}

FormattedMessage GaduFormater::createMessage(Account account, UinType sender, const QString &content,
		unsigned char *formats, unsigned int size, bool receiveImages)
{
	FormattedMessage result;

	if (size == 0 || !formats)
	{
		result << FormattedMessagePart(content, false, false, false, QColor());
		return result;
	}

	bool first = true;
	unsigned int memoryPosition = 0;
	unsigned int prevTextPosition = 0;
	unsigned int textPosition = 0;
	unsigned int images = 0;

	struct gg_msg_richtext_format prevFormat;
	struct gg_msg_richtext_format format;
	struct gg_msg_richtext_color prevColor;
	struct gg_msg_richtext_color color;
	struct gg_msg_richtext_image image;

	while (memoryPosition + sizeof(format) <= size)
	{
		memcpy(&format, formats + memoryPosition, sizeof(format));
		memoryPosition += sizeof(format);
		textPosition = gg_fix16(format.position);

		if (first && format.position > 0)
			result << FormattedMessagePart(content.mid(0, textPosition), false, false, false, QColor());

		if (format.font & GG_FONT_IMAGE)
		{
			images++;

			if (memoryPosition + sizeof(image) <= size)
			{
				memcpy(&image, formats + memoryPosition, sizeof(image));
				memoryPosition += sizeof(image);
			}
		}
		else
		{
			if (memoryPosition + sizeof(color) <= size)
				if (format.font & GG_FONT_COLOR)
				{
					memcpy(&color, formats + memoryPosition, sizeof(color));
					memoryPosition += sizeof(color);
				}
		}

		if (!first && textPosition > prevTextPosition)
			appendToMessage(account, result, sender, content.mid(prevTextPosition, textPosition - prevTextPosition),
					prevFormat, prevColor, image, receiveImages && images <= MAX_NUMBER_OF_IMAGES);
		else
			first = false;

		if (textPosition > prevTextPosition)
			prevTextPosition = textPosition;

		prevFormat = format;
		prevColor = color;
	}

	appendToMessage(account, result, sender, content.mid(prevTextPosition, content.length() - prevTextPosition),
			prevFormat, prevColor, image, receiveImages && images <= MAX_NUMBER_OF_IMAGES);

	return result;
}

