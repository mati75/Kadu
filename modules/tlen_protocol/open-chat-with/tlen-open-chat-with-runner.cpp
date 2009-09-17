/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account.h"

#include "contacts/contact.h"

#include "debug.h"

#include "tlen-contact-account-data.h"
#include "tlen-open-chat-with-runner.h"

TlenOpenChatWithRunner::TlenOpenChatWithRunner(Account *account) : ParentAccount(account)
{
}

ContactList TlenOpenChatWithRunner::matchingContacts(const QString &query)
{
	kdebugf();

	ContactList matchedContacts;
	if (!validateUserID(query))
		return matchedContacts;

	Contact c;

	TlenContactAccountData *gcad = new TlenContactAccountData(c, ParentAccount, query);
	c.addAccountData(gcad);
	c.setDisplay(ParentAccount->name() + ": " + query);
	matchedContacts.append(c);

	return matchedContacts;
}

bool TlenOpenChatWithRunner::validateUserID(const QString &uid)
{
	// 3-25 chars, small letters, numbers and "-", "." , "_"
	//QString text = uid;
	return true;
}
