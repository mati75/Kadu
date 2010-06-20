#ifndef NEXTINFO_H
	#define NEXTINFO_H


#include <QObject>
#include <QPointer>
#include <QTimer>
#include <QList>

#include "action.h"
#include "configuration_aware_object.h"
#include "main_configuration_window.h"

#include "defines.h"
#include "extendedinformationwindow.h"


class NExtInfo : public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT
	public:
		NExtInfo( bool firstLoad );
		~NExtInfo();
		void mainConfigurationWindowCreated( MainConfigurationWindow *mainConfigurationWindow );
		QPair< bool, QPair<int,int> > checkBirthday( UserListElement user );
		QPair< bool, QPair<int,int> > checkNameday( UserListElement user );
	public slots:
		void showExtendedInformationWindow( QAction *sender, bool toggled );
		void notifyBirthdayNameday();
		void dontInformAboutTheBirthday( QAction *sender, bool toggled ); void informAboutTheBirthday( QAction *sender, bool toggled );
		void dontInformAboutTheNameday( QAction *sender, bool toggled ); void informAboutTheNameday( QAction *sender, bool toggled );
		void showHelp();
		void importDataFromExtInfo();
	protected:
		void configurationUpdated();
	private:
		void createDefaultConfiguration();
		int closestYear( int month, int day );
		QList< QPointer<ExtendedInformationWindow> > extendedinformationwindows;
		ActionDescription *popupmenu_item_separator, *popupmenu_item_extinfo,
		                  *popupmenu_item_birthdaydontinform, *popupmenu_item_birthdayinform,
		                  *popupmenu_item_namedaydontinform,  *popupmenu_item_namedayinform;
		QTimer *birthdaynamedaytimer;
		ActionDescription *nextinfoaction;
		bool notify;                // configuration value
		bool notifyAboutBirthdays;  // configuration value
		bool notifyAboutNamedays;   // configuration value
		int  notifyAdvance;         // configuration value
		int  notifyInterval;        // configuration value
};


extern NExtInfo *nextinfo;


#endif
