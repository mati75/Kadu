/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtXml/QDomElement>

#include "config_file.h"
#include "chat_widget.h"
#include "custom_input.h"
#include "toolbar.h"

#include "chat_edit_box.h"

ChatEditBox::ChatEditBox(QWidget *parent)
	: KaduMainWindow(parent)
{
	InputBox = new CustomInput(this);
	InputBox->setWordWrapMode(QTextOption::WordWrap);
	InputBox->setTextFormat(Qt::RichText);

	setCentralWidget(InputBox);

	bool old_top = loadToolBarsFromConfig("chatTopDockArea", Qt::TopToolBarArea, true);
	bool old_middle = loadToolBarsFromConfig("chatMiddleDockArea", Qt::TopToolBarArea, true);
	bool old_bottom = loadToolBarsFromConfig("chatBottomDockArea", Qt::BottomToolBarArea, true);
	bool old_left = loadToolBarsFromConfig("chatLeftDockArea", Qt::LeftToolBarArea, true);
	bool old_right = loadToolBarsFromConfig("chatRightDockArea", Qt::RightToolBarArea, true);

	if (old_top || old_middle || old_bottom || old_left || old_right)
		writeToolBarsToConfig("chat"); // port old config
	else
		loadToolBarsFromConfig("chat"); // load new config
}

ChatEditBox::~ChatEditBox()
{
	writeToolBarsToConfig("chat");
}

CustomInput * ChatEditBox::inputBox()
{
	return InputBox;
}

bool ChatEditBox::supportsActionType(ActionDescription::ActionType type)
{
	return (type == ActionDescription::TypeGlobal || type == ActionDescription::TypeChat || type == ActionDescription::TypeUser);
}

UserBox * ChatEditBox::userBox()
{
	ChatWidget *cw = chatWidget();
	if (cw && cw->users()->count() > 1)
			return cw->getUserbox();

	return 0;
}

UserListElements ChatEditBox::userListElements()
{
	ChatWidget *cw = chatWidget();
	if (cw)
		return cw->users()->toUserListElements();

	return UserListElements();
}

ChatWidget * ChatEditBox::chatWidget()
{
	ChatWidget *result = dynamic_cast<ChatWidget *>(parent());
	if (result)
		return result;

	result = dynamic_cast<ChatWidget *>(parent()->parent());
	if (result)
		return result;

	return 0;
}
