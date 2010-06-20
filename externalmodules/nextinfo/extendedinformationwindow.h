#ifndef EXTENDEDINFORMATIONWINDOW_H
	#define EXTENDEDINFORMATIONWINDOW_H


#include <QComboBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QObject>
#include <QPushButton>
#include <QScrollArea>
#include <QTabWidget>
#include <QTextEdit>
#include <QVariant>
#include <QWidget>

#include "configuration_aware_object.h"
#include "main_configuration_window.h"
#include "kadu.h"

#include "defines.h"


class ExtendedInformationWindow : public QWidget
{
	Q_OBJECT
	public:
		ExtendedInformationWindow( QWidget* parent = 0, const char* name = 0, Qt::WindowFlags f = 0 );
		~ExtendedInformationWindow();
		void setUser( UserListElement usertoset );
	protected:
		// window layout
		QVBoxLayout* ExtendedInformationWindowLayout;
		// tabs
		QTabWidget* tabs;
		// tab: extinfo
		QWidget* extinfotab;
		QVBoxLayout* layout_extinfotab;
		QWidget* extinfotab_separator1;
		QLabel* label_info;
		QWidget* extinfotab_separator2;
		QHBoxLayout* layout_firstname; QLabel* label_firstname; QLineEdit* field_firstname;
		QHBoxLayout* layout_lastname;  QLabel* label_lastname;  QLineEdit* field_lastname;
		QHBoxLayout* layout_nickname;  QLabel* label_nickname;  QLineEdit* field_nickname;
		QHBoxLayout* layout_address;   QLabel* label_address;   QLineEdit* field_address;
		QHBoxLayout* layout_city;      QLabel* label_city;      QLineEdit* field_city;
		QWidget* extinfotab_separator3;
		QHBoxLayout* layout_sex;       QLabel* label_sex;       QComboBox* field_sex;
		QWidget* extinfotab_separator4;
		QHBoxLayout* layout_mobile;    QLabel* label_mobile;    QLineEdit* field_mobile;
		QHBoxLayout* layout_phone2;    QLabel* label_phone2;    QLineEdit* field_phone2;
		QWidget* extinfotab_separator5;
		QHBoxLayout* layout_email;     QLabel* label_email;     QLineEdit* field_email;
		QHBoxLayout* layout_email2;    QLabel* label_email2;    QLineEdit* field_email2;
		QHBoxLayout* layout_www;       QLabel* label_www;       QLineEdit* field_www;
		QWidget* extinfotab_separator6;
		QHBoxLayout* layout_gg;        QLabel* label_gg;        QLineEdit* field_gg;
		QHBoxLayout* layout_gg2;       QLabel* label_gg2;       QLineEdit* field_gg2;
		QHBoxLayout* layout_irc;       QLabel* label_irc;       QLineEdit* field_irc;
		QHBoxLayout* layout_tlen;      QLabel* label_tlen;      QLineEdit* field_tlen;
		QHBoxLayout* layout_wp;        QLabel* label_wp;        QLineEdit* field_wp;
		QHBoxLayout* layout_icq;       QLabel* label_icq;       QLineEdit* field_icq;
		QWidget* extinfotab_separator7;
		QHBoxLayout* layout_birthday;  QLabel* label_birthday;  QLineEdit* field_birthday;
		QHBoxLayout* layout_nameday;   QLabel* label_nameday;   QLineEdit* field_nameday;
		QWidget* extinfotab_separator8;
		QSpacerItem* extinfotab_spacer;
		// tab: notes
		QWidget* notestab;
		QVBoxLayout* layout_notestab;
		QWidget* notestab_separator1;
		QLabel* label_interests; QTextEdit* field_interests;
		QWidget* notestab_separator2;
		QLabel* label_notes;     QTextEdit* field_notes;
		QWidget* notestab_separator3;
		// tab: photo
		QWidget* phototab;
		QVBoxLayout* layout_phototab;
		QScrollArea* photoscrollarea;
		QLabel* photo;
		QHBoxLayout* layout_photobuttons; QSpacerItem* photobuttons_spacer; QPushButton* photobuttons_select; QPushButton* photobuttons_clear;
		// bottom buttons
		QHBoxLayout* bottombuttons;
		QSpacerItem* bottombuttons_spacer;
		QPushButton* button_save;
		QPushButton* button_close;
	protected slots:
		virtual void languageChange();
	private:
		UserListElement user;
		bool userset;
		QString photopath;
	private slots:
		void saveUser();
		void photoSelect();
		void photoClear();
};


#endif
