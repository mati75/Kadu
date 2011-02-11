#ifndef UI_HANDLER_H
#define UI_HANDLER_H

#include "gui/windows/main-configuration-window.h"

namespace MimeTeX {
	/*!
	 * \brief Handler of configuration dialogs GUI
	 */
	class UIHandler : public ConfigurationUiHandler
	{
		Q_OBJECT

		public:
			//! Default constructor
			UIHandler();

			//! Default destructor
			~UIHandler();

			public slots:
				/*!
				 * This slot is called when the configuration
				 * dialog is created. It handles setting
				 * appropriate value of the font size combo
				 * box
				 *
				 * \param mainConfigurationWindow Pointer to
				 *        the configuration window object
				 */
				virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

	};
}

#endif /* UI_HANDLER_H */
