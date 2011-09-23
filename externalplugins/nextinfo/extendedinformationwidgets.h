/****************************************************************************
*                                                                           *
*   NExtInfo plugin for Kadu                                                *
*   Copyright (C) 2008-2011  Piotr Dąbrowski ultr@ultr.pl                   *
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


#ifndef EXTENDEDINFORMATIONWIDGETS_H
	#define EXTENDEDINFORMATIONWIDGETS_H

#undef None

#include <QComboBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QObject>
#include <QPointer>
#include <QPushButton>
#include <QScrollArea>
#include <QTabWidget>
#include <QTextEdit>
#include <QVariant>
#include <QWidget>

#include "buddies/buddy.h"
#include "gui/windows/buddy-data-window.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/main-configuration-window.h"

#include "defines.h"

class ExtendedInformationWidgets : public QObject
{
	Q_OBJECT
	public:
		static QList<ExtendedInformationWidgets*> instances();
		ExtendedInformationWidgets( BuddyDataWindow* owner );
		~ExtendedInformationWidgets();
	protected:
		// tab: extinfo
		QPointer<QWidget> extinfotab;
		QVBoxLayout* layout_extinfotab;
		QWidget* extinfotab_separator1;
		QHBoxLayout* layout_firstname; QLabel* label_firstname; QLineEdit* field_firstname;
		QHBoxLayout* layout_lastname;  QLabel* label_lastname;  QLineEdit* field_lastname;
		QHBoxLayout* layout_nickname;  QLabel* label_nickname;  QLineEdit* field_nickname;
		QWidget* extinfotab_separator2;
		QHBoxLayout* layout_gender;    QLabel* label_gender;    QComboBox* field_gender;
		QWidget* extinfotab_separator3;
		QHBoxLayout* layout_address;   QLabel* label_address;   QLineEdit* field_address;
		QHBoxLayout* layout_city;      QLabel* label_city;      QLineEdit* field_city;
		QWidget* extinfotab_separator4;
		QHBoxLayout* layout_phone;     QLabel* label_phone;     QLineEdit* field_phone;
		QHBoxLayout* layout_mobile;    QLabel* label_mobile;    QLineEdit* field_mobile;
		QWidget* extinfotab_separator5;
		QHBoxLayout* layout_email;     QLabel* label_email;     QLineEdit* field_email;
		QHBoxLayout* layout_email2;    QLabel* label_email2;    QLineEdit* field_email2;
		QHBoxLayout* layout_www;       QLabel* label_www;       QLineEdit* field_www;
		QWidget* extinfotab_separator6;
		QHBoxLayout* layout_birthday;  QLabel* label_birthday;  QLineEdit* field_birthday;
		QHBoxLayout* layout_nameday;   QLabel* label_nameday;   QLineEdit* field_nameday;
		QWidget* extinfotab_separator7;
		QSpacerItem* extinfotab_spacer;
		// tab: notes
		QPointer<QWidget> notestab;
		QVBoxLayout* layout_notestab;
		QWidget* notestab_separator1;
		QLabel* label_interests; QTextEdit* field_interests;
		QWidget* notestab_separator2;
		QLabel* label_notes;     QTextEdit* field_notes;
		QWidget* notestab_separator3;
	private:
		static QList<ExtendedInformationWidgets*> INSTANCES;
		void languageChange();
		Buddy buddy;
		BuddyDataWindow* buddydatawindow;
	private slots:
		void loadBuddy();
		void saveBuddy();
		void avatarClicked();
};


#endif
