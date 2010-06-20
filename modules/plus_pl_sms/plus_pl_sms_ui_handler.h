#ifndef UI_HANDLER_H
#define UI_HANDLER_H

#include "main_configuration_window.h"

namespace plus_pl_sms
{
	class UIHandler : public ConfigurationUiHandler
	{
		Q_OBJECT

		public:
			UIHandler(QObject *parent = NULL, char *name = NULL);
			~UIHandler();

		public slots:
			virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

	};
};

#endif // UI_HANDLER_H

