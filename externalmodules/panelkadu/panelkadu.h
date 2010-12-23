/****************************************************************************
*                                                                           *
*   PanelKadu module for Kadu                                               *
*   Copyright (C) 2008-2010  Piotr Dąbrowski ultr@ultr.pl                   *
*                                                                           *
*   This program is free software: you can redistribute it and/or modify    *
*   it under the terms of the GNU General Public License as published by    *
*   the Free Software Foundation, either version 3 of the License, or       *
*   (at your option) any later version.                                     *
*                                                                           *
*   This program is distributed in the hope that it will be useful,         *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
*   GNU General Public License for more details.                            *
*                                                                           *
*   You should have received a copy of the GNU General Public License       *
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
*                                                                           *
****************************************************************************/


#ifndef PANELKADU_H
	#define PANELKADU_H


#include <QObject>
#include <QTimer>

#include "configuration/configuration-aware-object.h"
#include "gui/windows/main-configuration-window.h"

#include "panelkadu.h"


#define  PANELKADU_MOUSEITMERINTERVAL      50  /* ms */
#define  PANELKADU_SIDE_RIGHT               0
#define  PANELKADU_SIDE_BOTTOM              1
#define  PANELKADU_SIDE_LEFT                2
#define  PANELKADU_SIDE_TOP                 3
#define  PANELKADU_DEFAULTSIZE            200  /* px */
#define  PANELKADU_DEFAULTLENGTH          500  /* px */
#define  PANELKADU_DEFAULTACTIVATIONTIME    0  /* ms */
#define  PANELKADU_DEFAULTHIDETIME        200  /* ms */
#define  PANELKADU_KADUMINIMUMWIDTH       100  /* px */
#define  PANELKADU_KADUMINIMUMHEIGHT      180  /* px */


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
		static QWidget *topLevel();
	protected:
		void configurationUpdated();
	private:
		void createDefaultConfiguration();
		bool isInActivationRanges( int number );
		void animate();
		bool isCompositingManagerRunning();
		EventFilter *eventfilter;
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
		void panelize( QWidget *window );
		void depanelize( QWidget *window );
		void showKadu();
		void hideKadu();
		void kaduParentChanged( QWidget *oldParent );
};

extern PanelKadu *panelkadu;


#endif
