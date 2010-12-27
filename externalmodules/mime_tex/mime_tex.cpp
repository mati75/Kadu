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
#include "mime_tex_ui_handler.h"

#include "gui/actions/action.h"
#include "kadu-core/chat/chat-manager.h"
#include "kadu-core/contacts/contact-set.h"
#include "kadu-core/configuration/configuration-file.h"
#include "kadu-core/icons-manager.cpp"
#include "kadu-core/gui/widgets/chat-widget-manager.h"
#include "kadu-core/gui/windows/main-window.h"
#include "kadu-core/misc/misc.h"
#include "kadu-core/modules.h"
#include "kadu-core/debug.h"

MimeTeX::MimeTeX *mimeTeX = 0;
//static MimeTeX::UIHandler uiHandler;

extern "C" int mime_tex_init(bool)
{
	kdebugf();
	mimeTeX = new MimeTeX::MimeTeX();
	kdebugf2();
	if(NULL == mimeTeX)
		return 1;
	return 0;
}

extern "C" void mime_tex_close()
{
	kdebugf();
	delete mimeTeX;
	kdebugf2();
}

MimeTeX::MimeTeX::MimeTeX(QObject *parent)
: QObject(parent)
{
	kdebugf();
	
	config_file.addVariable("MimeTeX", "mimetex_font_size", MimeTeX::MimeTeX::defaultFontSize());
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/mime_tex.ui"));
	
	TeXActionDescription = new ActionDescription(
			this,
			ActionDescription::TypeChat,
			"TeXformulaAction",
			this,
			SLOT(TeXActionActivated(QAction *, bool)),
			dataPath("kadu/modules/data/mime_tex/mime_tex_icons/tex_icon.png"),
			dataPath("kadu/modules/data/mime_tex/mime_tex_icons/tex_icon.png"),
			tr("Insert TeX formula"));
	
	kdebugf2();
}

MimeTeX::MimeTeX::~MimeTeX()
{
	kdebugf();
	emit deleting();
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/mime_tex.ui"));

	if(config_file.readBoolEntry("MimeTeX", "mimetex_remove_tmp_files", false))
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

	ContactSet contacts = dynamic_cast<Action *>(action)->contacts();

	ChatWidget *chatWidget = ChatWidgetManager::instance()->byChat(ChatManager::instance()->findChat(contacts));
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
