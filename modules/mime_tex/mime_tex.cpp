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

#include "action.h"
#include "chat_manager.h"
#include "config_file.h"
#include "icons_manager.h"
#include "kadu_main_window.h"
#include "misc.h"
#include "modules.h"
#include "debug.h"

MimeTeX::MimeTeX *mimeTeX = 0;
static MimeTeX::UIHandler uiHandler;

extern "C" int mime_tex_init()
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

MimeTeX::MimeTeX::MimeTeX(QObject *parent, const char *name)
: QObject(parent, name)
{
	kdebugf();
	
	config_file.addVariable("MimeTeX", "mimetex_font_size", MimeTeX::MimeTeX::defaultFontSize());
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/mime_tex.ui"), &uiHandler);
	
	TeXActionDescription = new ActionDescription(
			ActionDescription::TypeChat,
			"TeXformulaAction",
			this,
			SLOT(TeXActionActivated(QAction *, bool)),
			dataPath("kadu/modules/data/mime_tex/mime_tex_icons/tex_icon.png"), tr("Insert TeX formula"),
			false);
	/*
	TeXAction = new KaduAction(
			dataPath("kadu/modules/data/mime_tex/mime_tex_icons/tex_icon.png"),
				tr("TeX formula"), "tex_action", KaduAction::TypeChat);
	connect(TeXAction, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(TeXActionActivated(const UserGroup*, const QWidget*, bool)));
	ToolBar::addDefaultAction("Chat toolbar 1", "tex_action", true);
	 */
	
	kdebugf2();
}

MimeTeX::MimeTeX::~MimeTeX()
{
	kdebugf();
	emit deleting();
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/mime_tex.ui"), &uiHandler);

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

void MimeTeX::MimeTeX::TeXActionActivated(QAction *action, bool toggled)
{
	kdebugf();
	// TODO: Implement it
	/*
	ChatWidget *chat = chat_manager->findChatWidget(group);
	TeXFormulaDialog *formulaDialog = new TeXFormulaDialog(chat, "tex_formula_dialog");
	tmpFiles.append(formulaDialog->getTmpFileName());
	formulaDialog->show();
	 */
	KaduMainWindow *kaduMainWindow = dynamic_cast<KaduMainWindow *>(action->parent());
	if (!kaduMainWindow)
		return;

	ChatWidget *chatWidget = chat_manager->findChatWidget(kaduMainWindow->userListElements());
	if (!chatWidget)
		return;
	TeXFormulaDialog *formulaDialog = new TeXFormulaDialog(chatWidget, "tex_formula_dialog");
	tmpFiles.append(formulaDialog->getTmpFileName());
	formulaDialog->show();
	kdebugf2();
}

int MimeTeX::MimeTeX::defaultFontSize()
{
	kdebugf();
	return 4; // \Large
}
