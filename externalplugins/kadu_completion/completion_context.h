#ifndef COMPLETION_CONTEXT_H
#define COMPLETION_CONTEXT_H

#include "kadu-core/gui/widgets/custom-input.h"

#include "kadu_completion.h"
#include <QObject>

#include <memory>

class QKeyEvent;
class QCompleter;

namespace KaduCompletion {
    class CompletionContext : public QObject {
        Q_OBJECT
    public:
        explicit CompletionContext(const KaduCompletion::EmoticonList& emotList,
                                   CustomInput* edit = 0);
        virtual ~CompletionContext();

    private slots:
        void keyPressed(QKeyEvent *e, CustomInput *input, bool&);
        void insertCompletion(QString completion);

    private:
        std::auto_ptr<QCompleter> _completer;
        const KaduCompletion::EmoticonList& _emotList;
    };
}
#endif // COMPLETION_CONTEXT_H
