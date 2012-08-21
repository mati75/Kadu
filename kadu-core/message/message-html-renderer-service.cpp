/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration/chat-configuration-holder.h"
#include "emoticons/emoticons-manager.h"
#include "message/message.h"
#include "url-handlers/url-handler-manager.h"

#include "message-html-renderer-service.h"

MessageHtmlRendererService::MessageHtmlRendererService(QObject *parent) :
		QObject(parent)
{
}

MessageHtmlRendererService::~MessageHtmlRendererService()
{
}

QString MessageHtmlRendererService::renderMessage(const Message &message)
{
	QDomDocument domDocument;
	// force content to be valid HTML with only one root
	domDocument.setContent(QString("<div>%1</div>").arg(message.htmlContent()));

	UrlHandlerManager::instance()->expandUrls(domDocument, false);
	EmoticonsManager::instance()->expandEmoticons(domDocument, (EmoticonsStyle)ChatConfigurationHolder::instance()->emoticonsStyle());

	QString result = domDocument.toString(0);
	// remove <div></div>
	return result.mid(5, result.length() - 12);
}
