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

#ifndef CHAT_DETAILS_SIMPLE_H
#define CHAT_DETAILS_SIMPLE_H

#include "contacts/contact.h"

#include "chat/chat-details.h"

/**
 * @addtogroup Chat
 * @{
 */

/**
 * @class ChatDetailsSimple
 * @author Rafal 'Vogel' Malinowski
 * @short Chat data specyfic to 'simple' chat type.
 *
 * Class contains one Contact object. Chat name is set to this Contact's
 * Buddy's display name, chat title is that name with current description.
 */
class ChatDetailsSimple : public ChatDetails
{
	Q_OBJECT

	Contact CurrentContact;

protected:
	virtual void load();

public:
	explicit ChatDetailsSimple(ChatShared *chatData);
	virtual ~ChatDetailsSimple();

	virtual void store();
	virtual bool shouldStore();

	virtual ChatType * type() const;
	virtual ContactSet contacts() const;
	virtual QString name() const;

	void setContact(Contact contact);
	Contact contact();

};

/**
 * @}
 */

#endif // CHAT_DETAILS_SIMPLE_H
