/*
 * %kadu copyright begin%
 * Copyright 2006, 2007, 2008, 2009 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004 Tomasz Jarzynka (tomee@cpi.pl)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2002, 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002, 2003 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2002 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2002, 2003 Dariusz Jagodzik (mast3r@kadu.net)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QTextStream>
#include <QtGui/QHBoxLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QPushButton>
#include <QtGui/QStyle>
#include <QtGui/QTabWidget>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>

#include "debug.h"
#include "kadu-config.h"
#include "icons-manager.h"
#include "html_document.h"
#include "misc/misc.h"
#include "os/generic/url-opener.h"
#include "url-handlers/mail-url-handler.h"

#include "about.h"

About::About(QWidget *parent) :
		QWidget(parent, Qt::Window)
{
	kdebugf();

	// set window properties and flags
	setWindowRole("kadu-about");
	setWindowTitle(tr("About"));
	setAttribute(Qt::WA_DeleteOnClose);
	// end set window properties and flags

	QWidget *center = new QWidget(this);
	QWidget *texts = new QWidget(center);

	QLabel *l_icon = new QLabel(texts);
	l_icon->setPixmap(IconsManager::instance()->iconByPath("kadu_icons/kadu").pixmap(64, 64));

	QLabel *l_info = new QLabel(texts);
	l_info->setBackgroundRole(texts->backgroundRole());

	l_info->setText("<font size=\"5\">Kadu</font><br /><b>"
		+ tr("Version %1").arg(VERSION)
		+ "</b><br />"
		+ QString("Qt %2 (compiled with Qt %3)").arg(qVersion()).arg(QT_VERSION_STR));

	l_info->setWordWrap(true);
	l_info->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));

	QHBoxLayout *texts_layout = new QHBoxLayout(texts);
	texts_layout->addWidget(l_icon);
	texts_layout->addWidget(l_info);
	// end create main QLabel widgets (icon and app info)

	// our TabWidget
	QTabWidget *tw_about = new QTabWidget(center);
	tw_about->setUsesScrollButtons(false);
	// end our TabWidget

	QWidget *wb_about = new QWidget(tw_about);
	QVBoxLayout *about_layout = new QVBoxLayout(wb_about);
	about_layout->addWidget(new QLabel(tr("Instant Messenger"), wb_about));
	about_layout->addWidget(new QLabel(tr("Support:"), wb_about));
	about_layout->addWidget(new KaduLink("http://www.kadu.net/forum/", wb_about));
	about_layout->addSpacing(20);
	about_layout->addWidget(new QLabel("(C) 2001-2010 Kadu Team", wb_about));
	about_layout->addWidget(new KaduLink("http://www.kadu.net/", wb_about));
	about_layout->addStretch(100);

	// create our info widgets
	// authors
	QTextEdit *tb_authors = new QTextEdit(tw_about);
	tb_authors->setReadOnly(true);
	tb_authors->setFrameStyle(QFrame::NoFrame);
	tb_authors->setWordWrapMode(QTextOption::NoWrap);
	tb_authors->viewport()->setAutoFillBackground(false);
	tb_authors->setTextInteractionFlags(Qt::TextBrowserInteraction);
	QString authors = loadFile("AUTHORS");
	// convert the email addresses
	authors.replace(" (at) ", "@");
	authors.replace(" (dot) ", ".");
	authors.remove(QRegExp("[<>]"));
	authors.replace("\n   ", "</b><br/>&nbsp;&nbsp;&nbsp;");
	authors.replace('\n', QLatin1String("</b><br/><b>"));
	HtmlDocument doc;
	doc.parseHtml(authors);
	MailUrlHandler *handler = new MailUrlHandler;
	handler->convertUrlsToHtml(doc);
	delete handler;
	tb_authors->setHtml(doc.generateHtml());

	// people to thank
	QTextEdit *tb_thanks = new QTextEdit(tw_about);
	tb_thanks->setReadOnly(true);
	tb_thanks->setFrameStyle(QFrame::NoFrame);
	tb_thanks->setWordWrapMode(QTextOption::NoWrap);
	tb_thanks->viewport()->setAutoFillBackground(false);
	tb_thanks->setText(loadFile("THANKS"));

	// license
	QTextEdit *tb_license = new QTextEdit(tw_about);
	tb_license->setReadOnly(true);
	tb_license->setFrameStyle(QFrame::NoFrame);
	tb_license->setWordWrapMode(QTextOption::WordWrap);
	tb_license->viewport()->setAutoFillBackground(false);
	tb_license->setText(loadFile("COPYING"));

	// changelog
	QTextEdit *tb_changelog = new QTextEdit(tw_about);
	tb_changelog->setReadOnly(true);
	tb_changelog->setFrameStyle(QFrame::NoFrame);
	tb_changelog->setWordWrapMode(QTextOption::NoWrap);
	tb_changelog->viewport()->setAutoFillBackground(false);
	tb_changelog->setText(loadFile("ChangeLog"));

	// add tabs
	tw_about->addTab(wb_about, tr("&About"));
	tw_about->addTab(tb_authors, tr("A&uthors"));
	tw_about->addTab(tb_thanks, tr("&Thanks"));
	tw_about->addTab(tb_license, tr("&License"));
	tw_about->addTab(tb_changelog, tr("&ChangeLog"));
	// end create our info widgets

	QVBoxLayout *center_layout = new QVBoxLayout(center);
	center_layout->addWidget(texts);
	center_layout->addWidget(tw_about);
	// close button
	QWidget *bottom = new QWidget(center);

	QWidget *blank2 = new QWidget(bottom);
	blank2->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));

	QPushButton *pb_close =
			new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton),
				tr("&Close"), bottom);
	connect(pb_close, SIGNAL(clicked()), this, SLOT(close()));

	QHBoxLayout *bottom_layout = new QHBoxLayout(bottom);
	bottom_layout->addWidget(blank2);
	bottom_layout->addWidget(pb_close);

	// end close button
	center_layout->addWidget(bottom);

	QHBoxLayout *layout = new QHBoxLayout(this);
	//slayout->addWidget(left);
	layout->addWidget(center);

	loadWindowGeometry(this, "General", "AboutGeometry", 0, 50, 480, 380);

	kdebugf2();
}

About::~About()
{
	kdebugf();

 	saveWindowGeometry(this, "General", "AboutGeometry");

	kdebugf2();
}

void About::keyPressEvent(QKeyEvent *ke_event)
{
	if (ke_event->key() == Qt::Key_Escape)
		close();
}

QString About::loadFile(const QString &name)
{
	kdebugf();

	QFile file(dataPath("kadu/" + name));
	if (!file.open(QIODevice::ReadOnly))
	{
		kdebugm(KDEBUG_ERROR, "About::loadFile(%s) cannot open file\n", qPrintable(name));
		return QString::null;
	}

	QTextStream str(&file);
	QString data = str.readAll();
	file.close();

	kdebugf2();
	return data;
}

KaduLink::KaduLink(const QString &link, QWidget *parent) :
		QLabel(parent), Link(link)
{
	setText(QString("<a href=\"%1\">%1</a>").arg(Link));
	setCursor(QCursor(Qt::PointingHandCursor));
	setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed));
}

KaduLink::~KaduLink()
{
}

void KaduLink::mousePressEvent(QMouseEvent *)
{
	UrlOpener::openUrl(Link);
}
