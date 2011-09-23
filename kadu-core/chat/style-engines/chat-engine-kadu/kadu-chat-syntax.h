/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
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

#ifndef KADU_CHAT_SYNTAX_H
#define KADU_CHAT_SYNTAX_H

#include <QtCore/QString>

class KaduChatSyntax
{
	QString Syntax;
	QString Top;
	QString WithHeader;
	QString WithoutHeader;

public:
	KaduChatSyntax() {};
	KaduChatSyntax(const QString &syntax);

	void setSyntax(const QString &syntax);

	const QString & syntax() { return Syntax; };
	const QString & top() { return Top; };
	const QString & withHeader() { return WithHeader; };
	const QString & withoutHeader() { return WithoutHeader; };

};

#endif // KADU_CHAT_SYNTAX_H
