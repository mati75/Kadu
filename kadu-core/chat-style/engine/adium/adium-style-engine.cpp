/*
 * %kadu copyright begin%
 * Copyright 2012 Marcel Zięba (marseel@gmail.com)
 * Copyright 2009, 2010, 2010, 2011, 2012 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
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

#include "adium-style-engine.h"

#include "chat-style/chat-style.h"
#include "chat-style/engine/adium/adium-style.h"
#include "chat-style/engine/adium/adium-style-renderer-factory.h"
#include "message/message-html-renderer-service.h"
#include "misc/kadu-paths.h"
#include "misc/memory.h"

#include <QtCore/QDir>

AdiumStyleEngine::AdiumStyleEngine()
{
}

AdiumStyleEngine::~AdiumStyleEngine()
{
}

void AdiumStyleEngine::setMessageHtmlRendererService(MessageHtmlRendererService *messageHtmlRendererService)
{
	CurrentMessageHtmlRendererService = messageHtmlRendererService;
}

QString AdiumStyleEngine::isStyleValid(QString stylePath)
{
	return AdiumStyle::isStyleValid(stylePath) ? QDir(stylePath).dirName() : QString();
}

bool AdiumStyleEngine::styleUsesTransparencyByDefault(QString styleName)
{
	auto style = AdiumStyle{styleName};
	return style.defaultBackgroundIsTransparent();
}

QString AdiumStyleEngine::defaultVariant(const QString &styleName)
{
	auto style = AdiumStyle{styleName};
	return style.defaultVariant();
}

QStringList AdiumStyleEngine::styleVariants(QString styleName)
{
	QDir dir;
	QString styleBaseHref = KaduPaths::instance()->profilePath() + QLatin1String("syntax/chat/") + styleName + QLatin1String("/Contents/Resources/Variants/");
	if (!dir.exists(styleBaseHref))
		styleBaseHref = KaduPaths::instance()->dataPath() + QLatin1String("syntax/chat/") + styleName + QLatin1String("/Contents/Resources/Variants/");
	dir.setPath(styleBaseHref);
	dir.setNameFilters(QStringList("*.css"));
	return dir.entryList();
}

std::unique_ptr<ChatStyleRendererFactory> AdiumStyleEngine::createRendererFactory(const ChatStyle &chatStyle)
{
	auto style = std::make_shared<AdiumStyle>(chatStyle.name());
	style->setCurrentVariant(chatStyle.variant());

	auto result = make_unique<AdiumStyleRendererFactory>(style);
	result.get()->setMessageHtmlRendererService(CurrentMessageHtmlRendererService);
	return std::move(result);
}

#include "moc_adium-style-engine.cpp"
