#ifndef ANONYMOUS_INFO_H
#define ANONYMOUS_INFO_H

#include "contacts/contact.h"
#include "chat/chat.h"
#include "gui/widgets/chat-widget.h"
class AnonymousInfo {
public:
    AnonymousInfo(const Contact &contact, ChatWidget *chatWidget, const Account &account);
    ~AnonymousInfo();

    const Contact &contact() const { return m_contact; }
    const Account &account() const { return m_account; }
    ChatWidget *chatWidget() { return m_chatWidget; }
    bool operator ==(const AnonymousInfo &rhs) const;

private:
    Contact m_contact;
    ChatWidget *m_chatWidget;
    Account m_account;
};

#endif // ANONYMOUS_INFO_H
