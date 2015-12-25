/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QFile>
#include <QSpinBox>
#include <QComboBox>

#include "mime_tex.h"
#include "tex_formula_dialog.h"

#include "gui/actions/action.h"
#include "gui/actions/action-context.h"
#include "kadu-core/chat/chat-manager.h"
#include "kadu-core/contacts/contact-set.h"
#include "kadu-core/configuration/configuration-api.h"
#include "kadu-core/configuration/deprecated-configuration-api.h"
#include "kadu-core/icons/icons-manager.h"
#include "kadu-core/gui/widgets/chat-widget/chat-widget.h"
#include "kadu-core/gui/widgets/chat-widget/chat-widget-repository.h"
#include "kadu-core/gui/windows/main-window.h"
#include "kadu-core/misc/paths-provider.h"
#include "kadu-core/debug.h"
#include "kadu-core/core/core.h"
#include "kadu-core/core/application.h"

MimeTeX::MimeTeX * MimeTeX::MimeTeX::Instance = 0;

void MimeTeX::MimeTeX::createInstance()
{
	if (!Instance)
		Instance = new MimeTeX();
}

void MimeTeX::MimeTeX::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

MimeTeX::MimeTeX * MimeTeX::MimeTeX::instance()
{
	return Instance;
}

MimeTeX::MimeTeX::MimeTeX(QObject *parent)
: QObject(parent)
{
	kdebugf();

	Application::instance()->configuration()->deprecatedApi()->addVariable("MimeTeX", "mimetex_font_size", MimeTeX::defaultFontSize());

	TeXActionDescription = new ActionDescription(
			this,
			ActionDescription::TypeChat,
			"TeXformulaAction",
			this,
			SLOT(TeXActionActivated(QAction *, bool)),
			KaduIcon(Application::instance()->pathsProvider()->dataPath() + "plugins/data/mime_tex/mime_tex_icons/tex_icon.png"),
			tr("Insert TeX formula"));

	kdebugf2();
}

MimeTeX::MimeTeX::~MimeTeX()
{
	kdebugf();
	emit deleting();

	if(Application::instance()->configuration()->deprecatedApi()->readBoolEntry("MimeTeX", "mimetex_remove_tmp_files", false))
	{
		kdebugm(KDEBUG_INFO, "Removing tmp GIFs\n");
		QStringList::ConstIterator it;
		for(it = tmpFiles.begin(); it != tmpFiles.end(); ++it)
		{
			QFile file(*it);
			file.remove();
		}
	}
        delete TeXActionDescription;

	kdebugf2();
}

void MimeTeX::MimeTeX::TeXActionActivated(QAction *action, bool)
{
	kdebugf();

	Chat chat = qobject_cast<Action *>(action)->context()->chat();

	ChatWidget *chatWidget = Core::instance()->chatWidgetRepository()->widgetForChat(chat);
	if (!chatWidget)
		return;
	TeXFormulaDialog *formulaDialog = new TeXFormulaDialog(chatWidget);
	tmpFiles.append(formulaDialog->getTmpFileName());
	formulaDialog->show();
	kdebugf2();
}

int MimeTeX::MimeTeX::defaultFontSize()
{
	kdebugf();
	return 4; // \Large
}

#include "moc_mime_tex.cpp"
