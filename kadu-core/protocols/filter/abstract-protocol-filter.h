/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
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

#ifndef ABSTRACT_PROTOCOL_FILTER_H
#define ABSTRACT_PROTOCOL_FILTER_H

#include <QtCore/QObject>

class ProtocolFactory;

class AbstractProtocolFilter : public QObject
{
	Q_OBJECT

public:
	explicit AbstractProtocolFilter(QObject *parent = 0)
			: QObject(parent) {}
	virtual ~AbstractProtocolFilter() {}

	virtual bool acceptProtocol(ProtocolFactory *factory) = 0;

signals:
	void filterChanged();

};

#endif // ABSTRACT_PROTOCOL_FILTER_H
