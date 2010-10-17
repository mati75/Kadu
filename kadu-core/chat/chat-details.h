/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef CHAT_DETAILS_H
#define CHAT_DETAILS_H

#include "chat/chat-shared.h"
#include "storage/details.h"

class BuddySet;
class Chat;
class ChatType;
class ContactSet;

/**
 * @addtogroup Chat
 * @{
 */

/**
 * @class ChatDetails
 * @author Rafal 'Vogel' Malinowski
 * @short Chat data specyfic to given chat type.
 *
 * Objects derivered from this class contains chat data that is specyfic to
 * given chat type. This class creates common interface for this custom data
 * - it can return list of contacts in that chat.
 */
class ChatDetails : public QObject, public Details<ChatShared>
{
	Q_OBJECT

public:
	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @param mainData ChatShared object described by this object
	 * @short Default contructor.
	 *
	 * Contructor must be overriden and called by all subclasses. It assigns
	 * new object ot mainData object.
	 */
	explicit ChatDetails(ChatShared *mainData) : Details<ChatShared>(mainData) {}
	virtual ~ChatDetails() {}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @return chat type of this object
	 * @short Returns chat type of this object.
	 *
	 * Returns chat type of this object.
	 */
	virtual ChatType * type() const = 0;

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @return list of contacts with this chat
	 * @short Returns list of contacts with this chat.
	 *
	 * Returns list of contacts with this chat.
	 */
	virtual ContactSet contacts() const = 0;

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @return name of this chat
	 * @short Returns name of this chat.
	 *
	 * Returns name of this chat. This can be anything - like IRC room name
	 * or list of names of contacts.
	 */
	virtual QString name() const = 0;

signals:
	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Emited when chat title changes.
	 * @param chat this chat
	 * @param newTitle new chat title
	 *
	 * Signal is emited when chat title changes. Title can be list of
	 * chat names, topic of IRC room or anything else (depens on chat type).
	 */
	void titleChanged(Chat chat, const QString &newTitle);

};

/**
 * @}
 */

#include "chat/chat.h" // for MOC

#endif // CHAT_DETAILS_H
