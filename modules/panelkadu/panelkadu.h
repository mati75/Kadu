#ifndef PANELKADU_H
	#define PANELKADU_H


#include <QObject>
#include <QTimer>

#include "configuration_aware_object.h"
#include "main_configuration_window.h"

#include "panelkadu.h"


#define  PANELKADU_MOUSEITMERINTERVAL          50  /* ms */
#define  PANELKADU_SIDE_RIGHT                   0
#define  PANELKADU_SIDE_BOTTOM                  1
#define  PANELKADU_SIDE_LEFT                    2
#define  PANELKADU_SIDE_TOP                     3
#define  PANELKADU_DEFAULTSIZE                200  /* px */
#define  PANELKADU_DEFAULTLENGTH              500  /* px */
#define  PANELKADU_DEFAULTACTIVATIONTIME        0  /* ms */
#define  PANELKADU_DEFAULTHIDETIME            200  /* ms */
#define  PANELKADU_KADUMINIMUMWIDTH           100  /* px */
#define  PANELKADU_KADUMINIMUMHEIGHT          180  /* px */


class EventFilter : public QObject
{
	public:
		EventFilter() {}
	protected:
		bool eventFilter( QObject *o, QEvent *e );
};


class PanelKadu : public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT
	public:
		PanelKadu();
		~PanelKadu();
		void mainConfigurationWindowCreated( MainConfigurationWindow *mainConfigurationWindow );
		EventFilter *eventfilter;
	protected:
		void configurationUpdated();
	private:
		void createDefaultConfiguration();
		bool isInActivationRanges( int number );
		QTimer *mouseTimer;
		QTimer *activationTimer;
		QTimer *hidingTimer;
		QRect oldGeometry;
		int side;                       // configuration value
		int size;                       // configuration value
		bool userDefinedPanelLength;    // configuration value
		int panelPosition;              // configuration value
		int panelLength;                // configuration value
		int activationTime;             // configuration value
		int hideTime;                   // configuration value
		int hidingMargin;               // configuration value
		bool useActivationRanges;       // configuration value
		QString activationRanges;       // configuration value
		bool dontHidePanelWhenActive;   // configuration value
	private slots:
		void checkMouse();
		void showKadu();
		void hideKadu();
};

extern PanelKadu *panelKadu;


#endif
