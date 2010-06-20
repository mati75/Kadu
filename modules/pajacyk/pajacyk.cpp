#include "kadu.h"
#include "debug.h"
#include "modules.h"
#include "config_file.h"
#include "message_box.h"
#include "pajacyk.h"

#include <QtCore/QUrl>

Pajacyk *pajacyk = NULL;

extern "C" KADU_EXPORT int pajacyk_init(bool firstLoad)
{
	kdebugf();

	pajacyk = new Pajacyk();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/pajacyk.ui"), pajacyk);

	return 0;

	kdebugf2();
}

extern "C" KADU_EXPORT void pajacyk_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/pajacyk.ui"), pajacyk);
	delete pajacyk;
	pajacyk = NULL;

	kdebugf2();
}

Pajacyk::Pajacyk()
{
	kdebugf();

	pajacykActionDescription = new ActionDescription(
		ActionDescription::TypeGlobal, "pajacykAction",
		this, SLOT(pajacykActionActivated(QAction *, bool)),
		"PajacykMenu", tr("Click Pajacyk - feed the kids"), false
	);
	kadu->insertMenuActionDescription(0, pajacykActionDescription);

	http.setHost("www.pajacyk.pl");
	connect(&http, SIGNAL(finished()), this, SLOT(finishedSlot()));
	connect(&http, SIGNAL(error()), this, SLOT(errorSlot()));
	connect(&http, SIGNAL(redirected(QString)), this, SLOT(redirectedSlot(QString)));

	if (config_file.readBoolEntry("Pajacyk", "AutoClick", false))
	{
		click();
	}

	kdebugf2();
}

Pajacyk::~Pajacyk()
{
	kdebugf();

	kadu->removeMenuActionDescription(pajacykActionDescription);
	delete pajacykActionDescription;
	disconnect(&http, SIGNAL(finished()), this, SLOT(finishedSlot()));
	disconnect(&http, SIGNAL(error()), this, SLOT(errorSlot()));
	disconnect(&http, SIGNAL(redirected(QString)), this, SLOT(redirectedSlot(QString)));

	kdebugf2();
}

void Pajacyk::configurationUpdated()
{
}

void Pajacyk::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
}

void Pajacyk::pajacykActionActivated(QAction *, bool)
{
	click();
}

void Pajacyk::click()
{
	http.get("/zlicz.php");
}

void Pajacyk::finishedSlot()
{
	kdebugf();
	QString page = http.data();
	kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "finishedSlot: '%s'\n", qPrintable(page));
	checkResponse(page);
}

void Pajacyk::errorSlot()
{
	kdebugf();
	QString page = http.data();
	kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "errorSlot: '%s'\n", qPrintable(page));
	checkResponse(page);
}

void Pajacyk::redirectedSlot(QString link)
{
	kdebugf();
	QString page = http.data();
	kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "redirectedSlot: '%s'\n", qPrintable(link));
	checkResponse(page);
	checkResponse(link);
}

void Pajacyk::checkResponse(QString &result)
{
	if (result.contains("nie_dziekujemy.php"))
		MessageBox::msg(tr("You already have fed the kids today. Please do it again tomorrow"));
	else if (result.contains("dziekujemy.php"))
		MessageBox::msg(tr("You have just fed the kids. Thank you"));
}

