#include "completion_context.h"

#include "kadu-core/debug.h"

#include <QAbstractItemView>
#include <QCompleter>
#include <QKeyEvent>
#include <QScrollBar>

KaduCompletion::CompletionContext::CompletionContext(const KaduCompletion::EmoticonList& emotList,
                                                     CustomInput *edit)
    : QObject(edit), _completer{nullptr}, _emotList(emotList) {
    kdebugf();
    if(0 == edit)
        deleteLater();
    else {
        connect(edit, SIGNAL(keyPressed(QKeyEvent*,CustomInput*,bool&)),
                this, SLOT(keyPressed(QKeyEvent*,CustomInput*,bool&)));
        connect(edit, SIGNAL(destroyed()), this, SLOT(deleteLater()));
    }
}

KaduCompletion::CompletionContext::~CompletionContext() {
    kdebugf();
}

void KaduCompletion::CompletionContext::keyPressed(QKeyEvent *e, CustomInput *input, bool&) {
    kdebugf();
    Qt::KeyboardModifiers m = e->modifiers();
    if(Qt::Key_Space == e->key() && m == Qt::ControlModifier) {
        QString text = input->toPlainText();
        int position = input->textCursor().position();
        bool foundBegin = false;
        int pos;
        QString completionPrefix;
        if(0 < text.length()) {
            for(pos = position - 1; pos >= 0 && !foundBegin; --pos) {
                if('<' == text.at(pos)) {
                    foundBegin = true;
                }
            }
        }

        ++pos;
        if(foundBegin) {
            completionPrefix = text.mid(pos, position - pos);
        }

        QStringList list;

        bool addAll = (0 == completionPrefix.length())? true : false;

        for(unsigned i = 0; i < _emotList.size(); ++i) {
            if(addAll) {
                list << _emotList[i];
            } else if(_emotList[i].left(completionPrefix.length()) == completionPrefix) {
                list << _emotList[i];
            }
        }

        if (_completer)
		{
			disconnect(_completer, SIGNAL(activated(QString)), this, SLOT(insertCompletion(QString)));
			_completer->deleteLater();
		}

		_completer = new QCompleter{list, input};
        _completer->setCaseSensitivity(Qt::CaseInsensitive);
        _completer->setWidget(input);
        _completer->setCompletionMode(QCompleter::PopupCompletion);
        _completer->setCaseSensitivity(Qt::CaseInsensitive);
        connect(_completer, SIGNAL(activated(QString)),
                this, SLOT(insertCompletion(QString)));
        _completer->setCompletionPrefix(completionPrefix);

        QRect cr = input->cursorRect();
        cr.setWidth(_completer->popup()->sizeHintForColumn(0)
                    + _completer->popup()->verticalScrollBar()->sizeHint().width());
        _completer->complete(cr);
    }
    kdebugf2();
}

void KaduCompletion::CompletionContext::insertCompletion(QString completion)
{
    CustomInput* widget = dynamic_cast<CustomInput*>(_completer->widget());
    if(0 == widget)
        return;

    QTextCursor tc = widget->textCursor();
    int extra = completion.length() - _completer->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));
    widget->setTextCursor(tc);
}

#include "moc_completion_context.cpp"
