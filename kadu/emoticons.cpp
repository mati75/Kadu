/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "emoticons.h"

#include <qfile.h>
#include <qtextstream.h>
#include <qregexp.h>
#include <qdir.h>

// dla config
#include "kadu.h"

EmoticonsManager::EmoticonsManager()
{
	ThemesList=QDir(QString(DATADIR)+"/apps/kadu/themes/emoticons").entryList();
	ThemesList.remove(".");
	ThemesList.remove("..");
};

const QStringList& EmoticonsManager::themes()
{
	return ThemesList;
};

void EmoticonsManager::setEmoticonsTheme(const QString& theme)
{
	config.emoticons_theme=theme;
	loadEmoticonsRegexpList();
	loadEmoticonsSelectorList();
};

void EmoticonsManager::loadEmoticonsRegexpList()
{
	QFile emoticons_file(themePath()+"/emoticons_regexp");
	emoticons_file.open(IO_ReadOnly);
	QTextStream emoticons_stream(&emoticons_file);
	EmoticonsRegexpList.clear();
	QString regexp;
	while(!emoticons_stream.atEnd())
	{
		regexp=emoticons_stream.readLine();
		if(regexp=="") continue;
		QString picname=emoticons_stream.readLine();
		EmoticonsRegexpListItem item;
		item.regexp=regexp;
		item.picname=picname;
		EmoticonsRegexpList.append(item);
		fprintf(stderr,"EMOTICON REGEXP: %s=%s\n",(const char*)regexp.local8Bit(),(const char*)picname.local8Bit());
	};
};

void EmoticonsManager::loadEmoticonsSelectorList()
{
	QFile emoticons_file(themePath()+"/emoticons_selector");
	emoticons_file.open(IO_ReadOnly);
	QTextStream emoticons_stream(&emoticons_file);
	EmoticonsSelectorList.clear();
	QString string;
	while(!emoticons_stream.atEnd())
	{
		string=emoticons_stream.readLine();
		if(string=="") continue;	
		QString picname=emoticons_stream.readLine();
		EmoticonsSelectorListItem item;
		item.string=string;
		item.picname=picname;
		EmoticonsSelectorList.append(item);
	};
};

QString EmoticonsManager::themePath()
{
	return QString(DATADIR)+"/apps/kadu/themes/emoticons/"+config.emoticons_theme;
};

void EmoticonsManager::expandEmoticons(QString& text)
{
	fprintf(stderr,"Expanding emoticons...\n");
	for(QValueList<EmoticonsRegexpListItem>::iterator i=EmoticonsRegexpList.begin(); i!=EmoticonsRegexpList.end(); i++)
		text.replace(QRegExp((*i).regexp),QString("__escaped_lt__IMG SRC=")+(*i).picname+"__escaped_gt__");
	fprintf(stderr,"Emoticons expanded...\n");
};

int EmoticonsManager::emoticonsCount()
{
	return EmoticonsSelectorList.count();
};

QString EmoticonsManager::emoticonString(int emot_num)
{
	return EmoticonsSelectorList[emot_num].string;	
};

QString EmoticonsManager::emoticonPicPath(int emot_num)
{
	return QString(DATADIR)+"/apps/kadu/themes/emoticons/"+config.emoticons_theme+"/"+EmoticonsSelectorList[emot_num].picname;
};
				
EmoticonsManager emoticons;
