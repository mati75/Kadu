#ifndef PAJACYK_H
#define PAJACYK_H

#include <QtCore/QObject>
#include <QtCore/QString>

#include "action.h"
#include "http_client.h"
#include "configuration_aware_object.h"
#include "main_configuration_window.h"

class Pajacyk : public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT

	HttpClient http;
	ActionDescription *pajacykActionDescription;
	void checkResponse(QString &result);

public:
	Pajacyk();
	~Pajacyk();
	void click();
	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

protected:
	virtual void configurationUpdated();

public slots:
	void finishedSlot();
	void errorSlot();
	void redirectedSlot(QString link);
	void pajacykActionActivated(QAction *, bool);
};

extern Pajacyk *pajacyk;

#endif // PAJACYK_H
