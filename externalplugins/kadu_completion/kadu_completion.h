#ifndef KADU_COMPLETION_H
#define KADU_COMPLETION_H

#include <QMutex>
#include <QMutexLocker>
#include <QObject>
#include <QString>

#include <vector>

class ChatWidget;

namespace KaduCompletion {
    class KaduCompletion : public QObject {
        Q_OBJECT
        public:
        typedef std::vector<QString> EmoticonList;
        virtual ~KaduCompletion();
        static void createInstance();
        static void destroyInstance();
        static KaduCompletion *instance();

    private slots:
        void chatWidgetCreated(ChatWidget *);

    private:
        KaduCompletion();

        static KaduCompletion *_instance;
        static QMutex _mutex;

        EmoticonList _list;
    };
}

#endif // KADU_COMPLETION_H
