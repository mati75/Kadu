#ifndef CHECK_ANONYMOUS_H
#define CHECK_ANONYMOUS_H

#include <QList>
#include <QObject>

#include "anonymous_info.h"

#include "contacts/contact-shared.h"
#include "buddies/buddy-list.h"
#include "configuration/configuration-aware-object.h"
#include "contacts/contact.h"
#include "chat/chat.h"

class Account;
class ChatWidget;
class ContactPersonalInfoService;
class Message;

namespace AnonCheck {
    class AnonymousCheck : public QObject, ConfigurationAwareObject {
        Q_OBJECT
    public:
        AnonymousCheck(QObject *parent = 0);
        virtual ~AnonymousCheck();

        virtual void configurationUpdated();

    private slots:
        void onChatWidgetCreated(ChatWidget *chatWidget);
        void scheduleForProcessing(Account account, ContactPersonalInfoService *infoService,
                                   Contact contact, ChatWidget *chatWidget);
        void onNewResults(Buddy buddy);

    private:
        bool addDetail(QString &msg, const QString &detail, const QString &desc,
                       bool addNewLine = true);
        QList<AnonymousInfo> m_checkQueue;
    };
}

#endif
