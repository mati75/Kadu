/*
 * Copyright © 2008 Patryk Cisek, PK Team
 * Copyright © 2010, Patryk Cisek
 *
 * This file is part of anonymous_check.
 *
 *    anonymous_check is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    anonymous_check is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with anonymous_check.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "anonymous_info.h"

AnonymousInfo::AnonymousInfo(const Contact &contact, ChatWidget *chatWidget, const Account &account)
: m_contact(contact), m_chatWidget(chatWidget), m_account(account) {

}

AnonymousInfo::~AnonymousInfo() {

}

bool AnonymousInfo::operator ==(const AnonymousInfo &rhs) const {
    return (m_contact.id() == rhs.m_contact.id());
}
