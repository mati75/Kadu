/*
 * %kadu copyright begin%
 * Copyright 2008, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef GADU_URL_HANDLER_H
#define GADU_URL_HANDLER_H

#include <QtCore/QObject>
#include <QtCore/QRegExp>

#include "url-handlers/url-handler.h"

class QAction;

class IgnoreLinksDomVisitor;
class SimpleUrlExpander;

class GaduUrlHandler : public QObject, public UrlHandler
{
	Q_OBJECT

	SimpleUrlExpander *UrlExpander;
	IgnoreLinksDomVisitor *IgnoreLinksVisitor;

	QRegExp GaduRegExp;

private slots:
	void accountSelected(QAction *action);

public:
	GaduUrlHandler();
	virtual ~GaduUrlHandler();

	virtual bool isUrlValid(const QByteArray &url);
	virtual void expandUrls(QDomDocument domDocument, bool generateOnlyHrefAttr);
	virtual void openUrl(const QByteArray &url, bool disableMenu = false);

};

#endif // GADU_URL_HANDLER_H
