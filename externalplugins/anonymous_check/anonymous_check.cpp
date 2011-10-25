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


#include "anonymous_check.h"

#include "debug.h"
#include "buddies/buddy-search-criteria.h"
#include "chat/message/message-render-info.h"
#include "chat/message/message-common.h"
#include "gui/widgets/chat-widget-manager.h"
#include "contacts/contact-set.h"
#include "protocols/services/contact-personal-info-service.h"

AnonCheck::AnonymousCheck::AnonymousCheck(QObject *parent)
: QObject(parent) {
    connect(ChatWidgetManager::instance(), SIGNAL(chatWidgetCreated(ChatWidget *)),
            this, SLOT(onChatWidgetCreated(ChatWidget *)));
}

AnonCheck::AnonymousCheck::~AnonymousCheck() {
    disconnect(ChatWidgetManager::instance(), SIGNAL(chatWidgetCreated(ChatWidget *)),
            this, SLOT(onChatWidgetCreated(ChatWidget *)));
}

void AnonCheck::AnonymousCheck::configurationUpdated() {
    // TODO: Implement it if configuration reading needed
}

void AnonCheck::AnonymousCheck::onChatWidgetCreated(ChatWidget *chatWidget) {
    if(0 == chatWidget) {
        kdebugm(KDEBUG_INFO, "No chatWidget...\n");
        return;
    }

    Chat chat = chatWidget->chat();

    Protocol *protocolHandler = chat.chatAccount().protocolHandler();
    if(0 == protocolHandler) {
        kdebugm(KDEBUG_INFO, "No protocol handler...\n");
        return;
    }

    ContactPersonalInfoService *infoService = protocolHandler->contactPersonalInfoService();
    if(0 == infoService) {
        kdebugm(KDEBUG_INFO, "No search service...\n");
        return;
    }

    // First check if contact(s) have altready been processed
    ContactSet contacts = chat.contacts();
    ContactSet::iterator it;
    ContactSet::iterator endIt = contacts.end();
    for(it = contacts.begin(); it != endIt; ++it) {
        const Contact c = (*it);
        Account acc = chat.chatAccount();
        if((*it).ownerBuddy().isAnonymous() &&
            !m_checkQueue.contains(AnonymousInfo(c, chatWidget, acc))) {
            scheduleForProcessing(chat.chatAccount(), infoService, (*it), chatWidget);
        }
    }
}

void AnonCheck::AnonymousCheck::scheduleForProcessing(Account account,
                                                      ContactPersonalInfoService *infoService,
                                                      Contact contact,
                                                      ChatWidget *chatWidget) {
    AnonymousInfo info(contact, chatWidget, account);
    m_checkQueue.append(info);
    kdebugm(KDEBUG_INFO, "id: <%s>\n", contact.id().toAscii().constData());
    connect(infoService, SIGNAL(personalInfoAvailable(Buddy)),
            this, SLOT(onNewResults(Buddy)));
    infoService->fetchPersonalInfo(contact);
}

void AnonCheck::AnonymousCheck::onNewResults(Buddy buddy) {
    QList<AnonymousInfo>::const_iterator endCheckQueueIt = m_checkQueue.end();
    QList<AnonymousInfo>::iterator checkQueueIt;
    for(checkQueueIt = m_checkQueue.begin(); checkQueueIt != endCheckQueueIt; ++checkQueueIt) {
        if((*checkQueueIt).contact().id() == buddy.id((*checkQueueIt).account())) {
            kdebugm(KDEBUG_INFO, "id:\t%s\n", (*checkQueueIt).contact().id().toAscii().constData());
            QString gender;
            if(GenderFemale == buddy.gender()) {
                gender = tr("Female");
            } else if(GenderMale == buddy.gender()) {
                gender = tr("Male");
            }
            QString messageStr = QString(
                    "<h1>" + tr("Contact Info") + "</h1>");

            // Add only non-empty details
            addDetail(messageStr, buddy.firstName(), tr("First Name"));
            addDetail(messageStr, buddy.lastName(), tr("Last Name"));
            addDetail(messageStr, buddy.familyName(), tr("Family Name"));
            addDetail(messageStr, buddy.city(), tr("City"));
            addDetail(messageStr, buddy.familyCity(), tr("Family City"));
            addDetail(messageStr, buddy.nickName(), tr("Nick Name"));
            addDetail(messageStr, buddy.homePhone(), tr("Home Phone"));
            addDetail(messageStr, buddy.mobile(), tr("Mobile Phone"));
            addDetail(messageStr, buddy.email(), tr("Email"));
            addDetail(messageStr, buddy.website(), tr("Website"));
            addDetail(messageStr, gender, tr("Gender"), false);
            if(0 != buddy.birthYear()) {
                addDetail(messageStr, QString("%1").arg(buddy.birthYear()),
                          tr("Birth Year"), false);
            }

            Message msg = Message::create();
            if(0 != (*checkQueueIt).chatWidget()) {
                Chat chat = (*checkQueueIt).chatWidget()->chat();
                if(!chat.isNull()) {
                    msg.setMessageChat(chat);
					msg.setType(MessageTypeSystem);
                    msg.setMessageSender(*(chat.contacts().begin()));
                    msg.setContent(messageStr);
                    msg.setReceiveDate(QDateTime::currentDateTime());
                    msg.setSendDate(QDateTime::currentDateTime());
                    MessageRenderInfo *renderInfo = new MessageRenderInfo(msg);
                    (*checkQueueIt).chatWidget()->appendMessage(renderInfo);
                }
            }

            m_checkQueue.removeOne(*checkQueueIt);
        }
    }

    disconnect(sender(), SIGNAL(personalInfoAvailable(Buddy)), this, SLOT(onNewResults(Buddy)));
}

bool AnonCheck::AnonymousCheck::addDetail(QString &msg,
                                          const QString &detail,
                                          const QString &desc,
                                          bool addNewLine) {
    // To maximize readibility, add only non-empty details
    if(!detail.isEmpty()) {
        msg += QString("<b>" + desc + ":</b> %1").arg(detail);
        if(addNewLine)
            msg += "<br>";

        return true;
    }

    return false;
}
