/********************************************************************************
** Form generated from reading UI file 'import_history.ui'
**
** Created by: Qt User Interface Compiler version 5.3.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_IMPORT_HISTORY_H
#define UI_IMPORT_HISTORY_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ImportHistory
{
public:
    QVBoxLayout *verticalLayout_2;
    QTabWidget *tabWidget;
    QWidget *oldGG;
    QVBoxLayout *verticalLayout_4;
    QLabel *label_2;
    QLabel *hint_old;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QLineEdit *ggPath;
    QPushButton *ggBrowseButton;
    QSpacerItem *verticalSpacer;
    QWidget *newGG;
    QVBoxLayout *verticalLayout;
    QLabel *label_3;
    QLabel *hint_new;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_4;
    QLineEdit *gg8Path;
    QPushButton *gg8BrowseButton;
    QSpacerItem *verticalSpacer_2;
    QGridLayout *gridLayout;
    QProgressBar *progressBar;
    QLabel *label_5;
    QPushButton *proceedButton;
    QPushButton *cancelButton;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *horizontalSpacer_2;
    QComboBox *accountCBox;

    void setupUi(QWidget *ImportHistory)
    {
        if (ImportHistory->objectName().isEmpty())
            ImportHistory->setObjectName(QStringLiteral("ImportHistory"));
        ImportHistory->resize(635, 201);
        verticalLayout_2 = new QVBoxLayout(ImportHistory);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        tabWidget = new QTabWidget(ImportHistory);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        oldGG = new QWidget();
        oldGG->setObjectName(QStringLiteral("oldGG"));
        verticalLayout_4 = new QVBoxLayout(oldGG);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        label_2 = new QLabel(oldGG);
        label_2->setObjectName(QStringLiteral("label_2"));

        verticalLayout_4->addWidget(label_2);

        hint_old = new QLabel(oldGG);
        hint_old->setObjectName(QStringLiteral("hint_old"));

        verticalLayout_4->addWidget(hint_old);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label = new QLabel(oldGG);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout_2->addWidget(label);

        ggPath = new QLineEdit(oldGG);
        ggPath->setObjectName(QStringLiteral("ggPath"));

        horizontalLayout_2->addWidget(ggPath);

        ggBrowseButton = new QPushButton(oldGG);
        ggBrowseButton->setObjectName(QStringLiteral("ggBrowseButton"));

        horizontalLayout_2->addWidget(ggBrowseButton);


        verticalLayout_4->addLayout(horizontalLayout_2);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_4->addItem(verticalSpacer);

        tabWidget->addTab(oldGG, QString());
        newGG = new QWidget();
        newGG->setObjectName(QStringLiteral("newGG"));
        verticalLayout = new QVBoxLayout(newGG);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        label_3 = new QLabel(newGG);
        label_3->setObjectName(QStringLiteral("label_3"));

        verticalLayout->addWidget(label_3);

        hint_new = new QLabel(newGG);
        hint_new->setObjectName(QStringLiteral("hint_new"));

        verticalLayout->addWidget(hint_new);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_4 = new QLabel(newGG);
        label_4->setObjectName(QStringLiteral("label_4"));

        horizontalLayout_3->addWidget(label_4);

        gg8Path = new QLineEdit(newGG);
        gg8Path->setObjectName(QStringLiteral("gg8Path"));

        horizontalLayout_3->addWidget(gg8Path);

        gg8BrowseButton = new QPushButton(newGG);
        gg8BrowseButton->setObjectName(QStringLiteral("gg8BrowseButton"));

        horizontalLayout_3->addWidget(gg8BrowseButton);


        verticalLayout->addLayout(horizontalLayout_3);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);

        tabWidget->addTab(newGG, QString());

        verticalLayout_2->addWidget(tabWidget);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        progressBar = new QProgressBar(ImportHistory);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        progressBar->setValue(0);

        gridLayout->addWidget(progressBar, 0, 0, 1, 5);

        label_5 = new QLabel(ImportHistory);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout->addWidget(label_5, 1, 0, 1, 2);

        proceedButton = new QPushButton(ImportHistory);
        proceedButton->setObjectName(QStringLiteral("proceedButton"));

        gridLayout->addWidget(proceedButton, 2, 0, 1, 1);

        cancelButton = new QPushButton(ImportHistory);
        cancelButton->setObjectName(QStringLiteral("cancelButton"));

        gridLayout->addWidget(cancelButton, 2, 4, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 1, 3, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 2, 1, 1, 2);

        accountCBox = new QComboBox(ImportHistory);
        accountCBox->setObjectName(QStringLiteral("accountCBox"));

        gridLayout->addWidget(accountCBox, 1, 2, 1, 1);


        verticalLayout_2->addLayout(gridLayout);


        retranslateUi(ImportHistory);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(ImportHistory);
    } // setupUi

    void retranslateUi(QWidget *ImportHistory)
    {
        ImportHistory->setWindowTitle(QApplication::translate("ImportHistory", "Form", 0));
        label_2->setText(QApplication::translate("ImportHistory", "Gadu Gadu 7.0 and olders:", 0));
        hint_old->setText(QString());
        label->setText(QApplication::translate("ImportHistory", "Path to gadu-gadu archives file", 0));
#ifndef QT_NO_TOOLTIP
        ggPath->setToolTip(QApplication::translate("ImportHistory", "Exact path to archives.dat file.\n"
"File can be found in Gadu-Gadu's profile directory.", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        ggPath->setWhatsThis(QApplication::translate("ImportHistory", "Exact path to archives.dat file.\n"
"File can be found in Gadu-Gadu's profile directory.", 0));
#endif // QT_NO_WHATSTHIS
        ggBrowseButton->setText(QApplication::translate("ImportHistory", "Browse", 0));
        tabWidget->setTabText(tabWidget->indexOf(oldGG), QApplication::translate("ImportHistory", "Old GG", 0));
        label_3->setText(QApplication::translate("ImportHistory", "Gadu Gadu version 8, 9 and 10:", 0));
        hint_new->setText(QString());
        label_4->setText(QApplication::translate("ImportHistory", "Path to gadu-gadu profile dir:", 0));
#ifndef QT_NO_TOOLTIP
        gg8Path->setToolTip(QApplication::translate("ImportHistory", "Path to Gadu-Gadu user's profile directory.\n"
"Given directory should contain Archive.db file and ImgCache subdirectory.", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        gg8Path->setWhatsThis(QApplication::translate("ImportHistory", "Path to Gadu-Gadu user's profile directory.\n"
"Given directory should contain Archive.db file and ImgCache subdirectory.", 0));
#endif // QT_NO_WHATSTHIS
        gg8BrowseButton->setText(QApplication::translate("ImportHistory", "Browse", 0));
        tabWidget->setTabText(tabWidget->indexOf(newGG), QApplication::translate("ImportHistory", "New GG", 0));
        label_5->setText(QApplication::translate("ImportHistory", "Target account:", 0));
        proceedButton->setText(QApplication::translate("ImportHistory", "Import", 0));
        cancelButton->setText(QApplication::translate("ImportHistory", "Cancel", 0));
    } // retranslateUi

};

namespace Ui {
    class ImportHistory: public Ui_ImportHistory {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_IMPORT_HISTORY_H
