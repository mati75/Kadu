/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat/chat-details-contact-set.h"
#include "icons/kadu-icon.h"

#include "chat-type-contact-set.h"

ChatTypeContactSet::ChatTypeContactSet(QObject *parent) :
		ChatType(parent)
{
	Aliases.append("ContactSet");
	Aliases.append("Conference");
}

ChatTypeContactSet::~ChatTypeContactSet()
{
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Sort index for this object.
 * @return sort index for this object
 *
 * Sort index is used for sorting chat types in history window.
 * Chats with smaller sort indexes are displayed first.
 *
 * Sort index for ChatTypeContactSet is 1.
 */
int ChatTypeContactSet::sortIndex() const
{
	return 1;
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Internal name of chat type.
 * @return internal name of chat type
 *
 * Chat type internal name. Internal name is used in @link ChatTypeManager @endlink
 * and also it is stored with @link Chat @endlink data.
 *
 * Internal name for ChatTypeContactSet is 'Conference'.
 */
QString ChatTypeContactSet::name() const
{
	return "ContactSet";
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Internal aliases of chat type.
 * @return internal aliases of chat type
 *
 * Chat type internal aliases, used to imporitng old configurations.
 *
 * Internal aliases for ChatTypeContactSet are 'ContactSet' and 'Conference'.
 */
QStringList ChatTypeContactSet::aliases() const
{
	return Aliases;
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Icon of chat type.
 * @return icon of chat type
 *
 * Chat type icon. Icon is used in history window and as icon of chat windows.
 *
 * Icon for ChatTypeContactSet is 'kadu_icons/conference'.
 */
KaduIcon ChatTypeContactSet::icon() const
{
	return KaduIcon("kadu_icons/conference");
}

/**
 * @author Piotr 'ultr' Dąbrowski
 * @short Window role for this chat type.
 * @return window role for this chat type.
 *
 * Kadu window role for this chat type.
 * For ContactSet the role is "kadu-chat-contact-set".
 */
QString ChatTypeContactSet::windowRole() const
{
	return "kadu-chat-contact-set";
}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Creates new ChatDetailsContactSet object for given chat type.
 * @return new ChatDetailsContactSet object for given chat type
 *
 * Creates new @link ChatDetailsContactSet @endlink object for
 * given @link Chat @endlink (@link ChatShared @endlink).
 */
ChatDetails * ChatTypeContactSet::createChatDetails(ChatShared *chatData) const
{
	return new ChatDetailsContactSet(chatData);
}
