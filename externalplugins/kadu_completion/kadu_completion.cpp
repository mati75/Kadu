#include "completion_context.h"
#include "kadu_completion.h"

#include "kadu-core/core/application.h"
#include "kadu-core/core/core.h"
#include "kadu-core/debug.h"
#include "kadu-core/gui/widgets/chat-widget/chat-widget.h"
#include "kadu-core/gui/widgets/chat-widget/chat-widget-repository.h"
#include "kadu-core/misc/paths-provider.h"

#include <QFile>
#include <QString>
#include <QStringList>
#include <QTextCodec>

KaduCompletion::KaduCompletion *KaduCompletion::KaduCompletion::_instance = 0;
QMutex KaduCompletion::KaduCompletion::_mutex;

KaduCompletion::KaduCompletion::~KaduCompletion() {
    kdebugf();
}

void KaduCompletion::KaduCompletion::createInstance() {
    kdebugf();
    if(_instance == 0) {
        QMutexLocker locker(&_mutex);
        if(_instance == 0)
            _instance = new KaduCompletion;
    }
}

void KaduCompletion::KaduCompletion::destroyInstance() {
    QMutexLocker locker(&_mutex);
    if(_instance != 0) {
        delete _instance;
        _instance = 0;
    }
}

void KaduCompletion::KaduCompletion::chatWidgetCreated(ChatWidget *chatWidget) {
    CompletionContext *newContext = new CompletionContext(_list, chatWidget->edit());
    connect(this, SIGNAL(destroyed()), newContext, SLOT(deleteLater()));
}

KaduCompletion::KaduCompletion::KaduCompletion() {
    kdebugf();
    QString listFileName(Application::instance()->pathsProvider()->dataPath() + "plugins/data/kadu_completion/list.txt");
    QFile file(listFileName);
    QTextCodec* codec = QTextCodec::codecForName("Windows-1250");
    if(file.open(QIODevice::ReadOnly)) {
        while(!file.atEnd()) {
            QString line = codec->toUnicode(file.readLine());
            QStringList lineElems = line.split(QChar(','));
            foreach(QString elem, lineElems) {
                if(elem.length() >= 0) {
                    elem = elem.replace(QRegExp("^\\("), "");
                    elem = elem.replace(QRegExp("\\)$"), "");
                    elem = elem.replace(QRegExp("\""), "");
                    elem = elem.replace(QRegExp("\\n"), "");

                    if(elem[0] == '<' && elem[elem.length() - 1] == '>')
                        _list.push_back(elem);
                }
            }
        }

        // Handle already opened chats
        for (auto *chatWidget : Core::instance()->chatWidgetRepository()) {
            chatWidgetCreated(chatWidget);
        }

        // Handle any chat created in the future
        connect(Core::instance()->chatWidgetRepository(), SIGNAL(chatWidgetAdded(ChatWidget*)), this, SLOT(chatWidgetCreated(ChatWidget*)));
    }
}

#include "moc_kadu_completion.cpp"
