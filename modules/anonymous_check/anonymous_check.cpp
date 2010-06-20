/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "anonymous_check.h"

#include "chat_manager.h"
#include "chat_widget.h"
#include "config_file.h"
#include "kadu.h"
#include "userlist.h"
#include "debug.h"

AnonCheck::AnonymousCheck *anonCheck = 0;

extern "C" int anonymous_check_init()
{
	kdebugf();
	anonCheck = new AnonCheck::AnonymousCheck();
	kdebugf2();
	return 0;
}

extern "C" void anonymous_check_close()
{
	kdebugf();
	delete anonCheck;
	kdebugf2();
}

AnonCheck::AnonymousCheck::AnonymousCheck(QObject *parent, const char *name)
: QObject(parent, name)
{
	kdebugf();
	QColor color;
	config_file.addVariable("AnonymousCheck", "anonymous_check_enabled", true);
	color.setRgb(255, 255, 255);
	config_file.addVariable("AnonymousCheck", "anonymous_check_msg_bg_color", color);
	color.setRgb(0, 0, 0);
	config_file.addVariable("AnonymousCheck", "anonymous_check_msg_fg_color", color);
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/anonymous_check.ui"), this);
	connect(chat_manager, SIGNAL( chatWidgetCreated(ChatWidget *) ), this, SLOT( onNewChatSlot(ChatWidget*) ));
	connect(gadu, SIGNAL( newSearchResults(SearchResults&, int, int) ), 
		this, SLOT( onSearchFinishedSlot(SearchResults&, int, int) ));
	kdebugf2();
}

AnonCheck::AnonymousCheck::~AnonymousCheck()
{
	kdebugf();
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/anonymous_check.ui"), this);
	kdebugf2();
}

void AnonCheck::AnonymousCheck::configurationUpdated()
{
}

void AnonCheck::AnonymousCheck::onNewChatSlot(ChatWidget *chat)
{
	kdebugf();
	const UserGroup *group = chat->users();
	if(config_file.readBoolEntry("AnonymousCheck", "anonymous_check_enabled"))
	{
		UserGroup::const_iterator itEnd = group->constEnd();
		for(UserGroup::const_iterator it = group->constBegin(); it != itEnd; ++it)
		{
			if(!userlist->contains("Gadu", (*it).ID("Gadu"), FalseForAnonymous))
			{
				kdebugm(KDEBUG_INFO, "Adding: %s\n", (*it).ID("Gadu").latin1());
				groups.insert((*it).altNick(), group);
				SearchRecord record;
				record.Uin = (*it).ID("Gadu");
				gadu->searchInPubdir(record);
			}
		}
	}
	kdebugf2();
}

void AnonCheck::AnonymousCheck::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
}

QString AnonCheck::AnonymousCheck::HTMLColorStr(const QColor &color)
{
	QString ret;
	ret.sprintf("#%02X%02X%02X", color.red(), color.green(), color.blue());
	kdebugm(KDEBUG_INFO, "ret=<%s>\n", ret.toLocal8Bit().constData());

	return ret;
}

void AnonCheck::AnonymousCheck::showMsg(ChatWidget* chat, const QString &msg)
{
	kdebugf();
	QColor tmpColor;
	tmpColor.setRgb(255, 255, 255);
	QColor bgColor = config_file.readColorEntry("AnonymousCheck", "anonymous_check_msg_bg_color", &tmpColor);
	QColor myChatBgColor = config_file.readColorEntry("Look", "ChatMyBgColor");
	tmpColor.setRgb(0, 0, 0);
	QColor color = config_file.readColorEntry("AnonymousCheck", "anonymous_check_msg_fg_color", &tmpColor);

	QString msgText = "<table bgcolor=\"" + HTMLColorStr(bgColor) + "\" border=1 width=100%><tr><td><font color=\"" +
		HTMLColorStr(color) + "\">" + msg + "</font></td></tr></table>";

	kdebugm(KDEBUG_INFO, "msg: %s\n", msgText.toLocal8Bit().constData());
	chat->appendSystemMessage(msgText, HTMLColorStr(myChatBgColor), HTMLColorStr(color));
	kdebugf2();
}

void AnonCheck::AnonymousCheck::onSearchFinishedSlot(SearchResults& searchResults, int seq, int lastUin)
{
	kdebugf();
	if(config_file.readBoolEntry("PowerKadu", "request_infos_about_anonymous", true))
		for(SearchResults::iterator it = searchResults.begin(); it != searchResults.end(); ++it)
		{
			ChatWidget *chat = chat_manager->findChatWidget(groups.value((*it).Uin));
			groups.remove((*it).Uin);
			if(chat)
			{
				QString infoMsgData = tr("Public directory search result:<br>") +
						      tr("Uin: <it>") + (*it).Uin + "</it><br>" +
						      tr("First name: ") + (*it).First + "<br>" +
						      tr("Nick: ") + (*it).Nick + "<br>" +
						      tr("Birth year: ") + (*it).Born + "<br>" +
						      tr("City: ") + (*it).City;
				kdebugm(KDEBUG_INFO, "infoMsgData=%s\n", infoMsgData.toLocal8Bit().constData());
				showMsg(chat, infoMsgData);
			}
		}
	kdebugf2();
}
