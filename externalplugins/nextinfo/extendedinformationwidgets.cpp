/****************************************************************************
*                                                                           *
*   NExtInfo module for Kadu                                                *
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




#include <QApplication>
#include <QComboBox>
#include <QDir>
#include <QFileDialog>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QList>
#include <QMessageBox>
#include <QPair>
#include <QPicture>
#include <QPushButton>
#include <QRegExp>
#include <QScrollArea>
#include <QTabWidget>
#include <QTextEdit>
#include <QToolTip>
#include <QVariant>
#include <QWhatsThis>

#include "buddies/buddy.h"
#include "buddies/buddy-shared.h"
#include "gui/widgets/buddy-avatar-widget.h"
#include "gui/widgets/buddy-general-configuration-widget.h"
#include "icons-manager.h"

#include "extendedinformationwidgets.h"
#include "photowidget.h"

#include "buddynextinfodata.h"




ExtendedInformationWidgets::ExtendedInformationWidgets( BuddyDataWindow* owner ) : QObject( owner )
{
	if( owner == NULL )
	{
		deleteLater();
		return;
	}
	INSTANCES.append( this );
	// data
	buddydatawindow = owner;
	buddy = buddydatawindow->buddy();
	// tabwidget
	QTabWidget *tabwidget = buddydatawindow->tabWidget();

	// tab: extended information

	extinfotab = new QWidget( tabwidget );
	layout_extinfotab = new QVBoxLayout( extinfotab );
	layout_extinfotab->setMargin( 5 );
	layout_extinfotab->setSpacing( 3 );

	extinfotab_separator1 = new QWidget( extinfotab );
		extinfotab_separator1->setMinimumHeight( NEXTINFO_FIELDSEPARATORSHEIGHT );
		extinfotab_separator1->resize( 1, NEXTINFO_FIELDSEPARATORSHEIGHT );
		layout_extinfotab->addWidget( extinfotab_separator1 );

	layout_firstname = new QHBoxLayout( 0 );
	layout_firstname->setMargin( 0 );
	layout_firstname->setSpacing( 6 );
		label_firstname = new QLabel( extinfotab );
		label_firstname->setAlignment( Qt::AlignRight | Qt::AlignVCenter | Qt::AlignVCenter | Qt::AlignVCenter );
		layout_firstname->addWidget( label_firstname );
		field_firstname = new QLineEdit( extinfotab );
		layout_firstname->addWidget( field_firstname );
		layout_extinfotab->addLayout( layout_firstname );
	layout_lastname = new QHBoxLayout( 0 );
	layout_lastname->setMargin( 0 );
	layout_lastname->setSpacing( 6 );
		label_lastname = new QLabel( extinfotab );
		label_lastname->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
		layout_lastname->addWidget( label_lastname );
		field_lastname = new QLineEdit( extinfotab );
		layout_lastname->addWidget( field_lastname );
		layout_extinfotab->addLayout( layout_lastname );
	layout_nickname = new QHBoxLayout( 0 );
	layout_nickname->setMargin( 0 );
	layout_nickname->setSpacing( 6 );
		label_nickname = new QLabel( extinfotab );
		label_nickname->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
		layout_nickname->addWidget( label_nickname );
		field_nickname = new QLineEdit( extinfotab );
		layout_nickname->addWidget( field_nickname );
		layout_extinfotab->addLayout( layout_nickname );

	extinfotab_separator2 = new QWidget( extinfotab );
		extinfotab_separator2->setMinimumHeight( NEXTINFO_FIELDSEPARATORSHEIGHT );
		extinfotab_separator2->resize( 1, NEXTINFO_FIELDSEPARATORSHEIGHT );
		layout_extinfotab->addWidget( extinfotab_separator2 );

	layout_gender = new QHBoxLayout( 0 );
	layout_gender->setMargin( 0 );
	layout_gender->setSpacing( 6 );
		label_gender = new QLabel( extinfotab );
		label_gender->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
		layout_gender->addWidget( label_gender );
		field_gender = new QComboBox( extinfotab );
		field_gender->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
		layout_gender->addWidget( field_gender );
		layout_extinfotab->addLayout( layout_gender );

	extinfotab_separator3 = new QWidget( extinfotab );
		extinfotab_separator3->setMinimumHeight( NEXTINFO_FIELDSEPARATORSHEIGHT );
		extinfotab_separator3->resize( 1, NEXTINFO_FIELDSEPARATORSHEIGHT );
		layout_extinfotab->addWidget( extinfotab_separator3 );

	layout_address = new QHBoxLayout( 0 );
	layout_address->setMargin( 0 );
	layout_address->setSpacing( 6 );
		label_address = new QLabel( extinfotab );
		label_address->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
		layout_address->addWidget( label_address );
		field_address = new QLineEdit( extinfotab );
		layout_address->addWidget( field_address );
		layout_extinfotab->addLayout( layout_address );
	layout_city = new QHBoxLayout( 0 );
	layout_city->setMargin( 0 );
	layout_city->setSpacing( 6 );
		label_city = new QLabel( extinfotab );
		label_city->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
		layout_city->addWidget( label_city );
		field_city = new QLineEdit( extinfotab );
		layout_city->addWidget( field_city );
		layout_extinfotab->addLayout( layout_city );

	extinfotab_separator4 = new QWidget( extinfotab );
		extinfotab_separator4->setMinimumHeight( NEXTINFO_FIELDSEPARATORSHEIGHT );
		extinfotab_separator4->resize( 1, NEXTINFO_FIELDSEPARATORSHEIGHT );
		layout_extinfotab->addWidget( extinfotab_separator4 );

	layout_phone = new QHBoxLayout( 0 );
	layout_phone->setMargin( 0 );
	layout_phone->setSpacing( 6 );
		label_phone = new QLabel( extinfotab );
		label_phone->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
		layout_phone->addWidget( label_phone );
		field_phone = new QLineEdit( extinfotab );
		layout_phone->addWidget( field_phone );
		layout_extinfotab->addLayout( layout_phone );
	layout_mobile = new QHBoxLayout( 0 );
	layout_mobile->setMargin( 0 );
	layout_mobile->setSpacing( 6 );
		label_mobile = new QLabel( extinfotab );
		label_mobile->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
		layout_mobile->addWidget( label_mobile );
		field_mobile = new QLineEdit( extinfotab );
		layout_mobile->addWidget( field_mobile );
		layout_extinfotab->addLayout( layout_mobile );

	extinfotab_separator5 = new QWidget( extinfotab );
		extinfotab_separator5->setMinimumHeight( NEXTINFO_FIELDSEPARATORSHEIGHT );
		extinfotab_separator5->resize( 1, NEXTINFO_FIELDSEPARATORSHEIGHT );
		layout_extinfotab->addWidget( extinfotab_separator5 );

	layout_email = new QHBoxLayout( 0 );
	layout_email->setMargin( 0 );
	layout_email->setSpacing( 6 );
		label_email = new QLabel( extinfotab );
		label_email->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
		layout_email->addWidget( label_email );
		field_email = new QLineEdit( extinfotab );
		layout_email->addWidget( field_email );
		layout_extinfotab->addLayout( layout_email );
	layout_email2 = new QHBoxLayout( 0 );
	layout_email2->setMargin( 0 );
	layout_email2->setSpacing( 6 );
		label_email2 = new QLabel( extinfotab );
		label_email2->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
		layout_email2->addWidget( label_email2 );
		field_email2 = new QLineEdit( extinfotab );
		layout_email2->addWidget( field_email2 );
		layout_extinfotab->addLayout( layout_email2 );
	layout_www = new QHBoxLayout( 0 );
	layout_www->setMargin( 0 );
	layout_www->setSpacing( 6 );
		label_www = new QLabel( extinfotab );
		label_www->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
		layout_www->addWidget( label_www );
		field_www = new QLineEdit( extinfotab );
		layout_www->addWidget( field_www );
		layout_extinfotab->addLayout( layout_www );

	extinfotab_separator6 = new QWidget( extinfotab );
		extinfotab_separator6->setMinimumHeight( NEXTINFO_FIELDSEPARATORSHEIGHT );
		extinfotab_separator6->resize( 1, NEXTINFO_FIELDSEPARATORSHEIGHT );
		layout_extinfotab->addWidget( extinfotab_separator6 );

	layout_birthday = new QHBoxLayout( 0 );
	layout_birthday->setMargin( 0 );
	layout_birthday->setSpacing( 6 );
		label_birthday = new QLabel( extinfotab );
		label_birthday->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
		layout_birthday->addWidget( label_birthday );
		field_birthday = new QLineEdit( extinfotab );
		layout_birthday->addWidget( field_birthday );
		layout_extinfotab->addLayout( layout_birthday );
	layout_nameday = new QHBoxLayout( 0 );
	layout_nameday->setMargin( 0 );
	layout_nameday->setSpacing( 6 );
		label_nameday = new QLabel( extinfotab );
		label_nameday->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
		layout_nameday->addWidget( label_nameday );
		field_nameday = new QLineEdit( extinfotab );
		layout_nameday->addWidget( field_nameday );
		layout_extinfotab->addLayout( layout_nameday );

	extinfotab_separator7 = new QWidget( extinfotab );
		extinfotab_separator7->setMinimumHeight( NEXTINFO_FIELDSEPARATORSHEIGHT );
		extinfotab_separator7->resize( 1, NEXTINFO_FIELDSEPARATORSHEIGHT );
		layout_extinfotab->addWidget( extinfotab_separator7 );

	extinfotab_spacer = new QSpacerItem( 20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding );
		layout_extinfotab->addItem( extinfotab_spacer );
	tabwidget->insertTab( NEXTINFO_TABPOS_EXTINFO, extinfotab, QString("") );

	// tab: notes

	notestab = new QWidget( tabwidget );
	layout_notestab = new QVBoxLayout( notestab );
	layout_notestab->setMargin( 5 );
	layout_notestab->setSpacing( 3 );

	notestab_separator1 = new QWidget( notestab );
		notestab_separator1->setMinimumHeight( NEXTINFO_FIELDSEPARATORSHEIGHT );
		notestab_separator1->resize( 1, NEXTINFO_FIELDSEPARATORSHEIGHT );
		layout_notestab->addWidget( notestab_separator1 );

	label_interests = new QLabel( notestab );
		layout_notestab->addWidget( label_interests );
		field_interests = new QTextEdit( notestab );
		field_interests->setMaximumSize( QSize( 32767, NEXTINFO_FIELDINTERESTSHEIGHT ) );
		field_interests->setTabChangesFocus( true );
		layout_notestab->addWidget( field_interests );

	notestab_separator2 = new QWidget( notestab );
		notestab_separator2->setMinimumHeight( NEXTINFO_FIELDSEPARATORSHEIGHT );
		notestab_separator2->resize( 1, NEXTINFO_FIELDSEPARATORSHEIGHT );
		layout_notestab->addWidget( notestab_separator2 );

	label_notes = new QLabel( notestab );
		layout_notestab->addWidget( label_notes );
		field_notes = new QTextEdit( notestab );
		field_notes->setTabChangesFocus( true );
		layout_notestab->addWidget( field_notes );

	notestab_separator3 = new QWidget( notestab );
		notestab_separator3->setMinimumHeight( NEXTINFO_FIELDSEPARATORSHEIGHT );
		notestab_separator3->resize( 1, NEXTINFO_FIELDSEPARATORSHEIGHT );
		layout_notestab->addWidget( notestab_separator3 );

	tabwidget->insertTab( NEXTINFO_TABPOS_NOTES, notestab, QString("") );

	// change captions
	languageChange();
	// load buddy
	loadBuddy();
	// connect buddy updating
	connect( buddydatawindow, SIGNAL(updatingBuddy()), this, SLOT(saveBuddy()) );
	connect( buddydatawindow, SIGNAL(updatingBuddy()), this, SLOT(loadBuddy()) );
}


ExtendedInformationWidgets::~ExtendedInformationWidgets()
{
	BuddyDataWindow *buddydatawindow = dynamic_cast<BuddyDataWindow*>( parent() );
	if( buddydatawindow != NULL )
	{
		disconnect( buddydatawindow, SIGNAL(updatingBuddy()), this, SLOT(saveBuddy()) );
		disconnect( buddydatawindow, SIGNAL(updatingBuddy()), this, SLOT(loadBuddy()) );
	}
	INSTANCES.removeOne( this );
	if( ! extinfotab.isNull() )
		extinfotab->deleteLater();
	if( ! notestab.isNull() )
		notestab->deleteLater();
}


QList<ExtendedInformationWidgets*> ExtendedInformationWidgets::INSTANCES;


QList<ExtendedInformationWidgets*> ExtendedInformationWidgets::instances()
{
	return INSTANCES;
}


void ExtendedInformationWidgets::languageChange()
{
	QTabWidget *tabwidget = buddydatawindow->tabWidget();
	// tab: extinfo
	tabwidget->setTabText( tabwidget->indexOf( extinfotab ), qApp->translate( "@nextinfo", "Extended information" ) );
	// tab: extinfo: labels: set texts
	label_firstname->setText( qApp->translate( "@nextinfo", "First name"            ) + ":" );
	label_lastname->setText(  qApp->translate( "@nextinfo", "Last name"             ) + ":" );
	label_nickname->setText(  qApp->translate( "@nextinfo", "Nickname"              ) + ":" );
	label_gender->setText(    qApp->translate( "@nextinfo", "Gender"                ) + ":" );
	label_address->setText(   qApp->translate( "@nextinfo", "Address"               ) + ":" );
	label_city->setText(      qApp->translate( "@nextinfo", "City"                  ) + ":" );
	label_phone->setText(     qApp->translate( "@nextinfo", "Phone"                 ) + ":" );
	label_mobile->setText(    qApp->translate( "@nextinfo", "Mobile"                ) + ":" );
	label_email->setText(     qApp->translate( "@nextinfo", "e-mail"                ) + ":" );
	label_email2->setText(    qApp->translate( "@nextinfo", "Alternative e-mail"    ) + ":" );
	label_www->setText(       qApp->translate( "@nextinfo", "Website"               ) + ":" );
	label_birthday->setText(  qApp->translate( "@nextinfo", "Birthday (DD.MM.YYYY)" ) + ":" );
	label_nameday->setText(   qApp->translate( "@nextinfo", "Name-day (DD.MM)"      ) + ":" );
	// set gender combobox items
	field_gender->addItem( ""                                      , 0 );
	field_gender->addItem( qApp->translate( "@nextinfo", "female" ), 1 );
	field_gender->addItem( qApp->translate( "@nextinfo", "male"   ), 2 );
	// tab: extinfo: labels: set new sizes
	label_firstname->adjustSize();
	label_lastname->adjustSize();
	label_nickname->adjustSize();
	label_gender->adjustSize();
	label_address->adjustSize();
	label_city->adjustSize();
	label_phone->adjustSize();
	label_mobile->adjustSize();
	label_email->adjustSize();
	label_email2->adjustSize();
	label_www->adjustSize();
	label_birthday->adjustSize();
	label_nameday->adjustSize();
	// tab: extinfo: labels: get maximum width
	int labels_width = 1;  // px
	if( label_firstname->width() > labels_width ) { labels_width = label_firstname->width(); }
	if( label_lastname->width()  > labels_width ) { labels_width = label_lastname->width();  }
	if( label_nickname->width()  > labels_width ) { labels_width = label_nickname->width();  }
	if( label_gender->width()    > labels_width ) { labels_width = label_gender->width();    }
	if( label_address->width()   > labels_width ) { labels_width = label_address->width();   }
	if( label_city->width()      > labels_width ) { labels_width = label_city->width();      }
	if( label_phone->width()     > labels_width ) { labels_width = label_phone->width();     }
	if( label_mobile->width()    > labels_width ) { labels_width = label_mobile->width();    }
	if( label_email->width()     > labels_width ) { labels_width = label_email->width();     }
	if( label_email2->width()    > labels_width ) { labels_width = label_email2->width();    }
	if( label_www->width()       > labels_width ) { labels_width = label_www->width();       }
	if( label_birthday->width()  > labels_width ) { labels_width = label_birthday->width();  }
	if( label_nameday->width()   > labels_width ) { labels_width = label_nameday->width();   }
	// tab: extinfo: labels: set width of each label to the maximum width
	label_firstname->setMinimumWidth( labels_width );
	label_lastname->setMinimumWidth(  labels_width );
	label_nickname->setMinimumWidth(  labels_width );
	label_gender->setMinimumWidth(    labels_width );
	label_address->setMinimumWidth(   labels_width );
	label_city->setMinimumWidth(      labels_width );
	label_phone->setMinimumWidth(     labels_width );
	label_mobile->setMinimumWidth(    labels_width );
	label_email->setMinimumWidth(     labels_width );
	label_email2->setMinimumWidth(    labels_width );
	label_www->setMinimumWidth(       labels_width );
	label_birthday->setMinimumWidth(  labels_width );
	label_nameday->setMinimumWidth(   labels_width );
	// tab: extinfo: labels: update labels' geometry
	label_firstname->updateGeometry();
	label_lastname->updateGeometry();
	label_nickname->updateGeometry();
	label_gender->updateGeometry();
	label_address->updateGeometry();
	label_city->updateGeometry();
	label_phone->updateGeometry();
	label_mobile->updateGeometry();
	label_email->updateGeometry();
	label_email2->updateGeometry();
	label_www->updateGeometry();
	label_birthday->updateGeometry();
	label_nameday->updateGeometry();
	// tab: notes
	tabwidget->setTabText( tabwidget->indexOf( notestab ), qApp->translate( "@nextinfo", "Notes" ) );
	label_interests->setText( qApp->translate( "@nextinfo", "Interests" ) + ":" );
	label_notes->setText(     qApp->translate( "@nextinfo", "Notes"     ) + ":" );
	// read only
	QFont labelfont = label_phone->font();
	labelfont.setItalic( true );
	QFont fieldfont = field_phone->font();
	fieldfont.setItalic( true );
	field_phone->setReadOnly( true );
	field_phone->setFont( fieldfont );
	label_phone->setFont( labelfont );
	field_mobile->setReadOnly( true );
	field_mobile->setFont( fieldfont );
	label_mobile->setFont( labelfont );
	field_email->setReadOnly( true );
	field_email->setFont( fieldfont );
	label_email->setFont( labelfont );
	field_www->setReadOnly( true );
	field_www->setFont( fieldfont );
	label_www->setFont( labelfont );
	// set minumum size
	tabwidget->setUsesScrollButtons( false );
	buddydatawindow->setMinimumSize( buddydatawindow->minimumSizeHint() );
}


void ExtendedInformationWidgets::loadBuddy()
{
	// module data
	BuddyNExtInfoData *bdata = 0;
	if( buddy.data() )
		bdata = buddy.data()->moduleStorableData<BuddyNExtInfoData>( "nextinfo", 0, true );
	if( ! bdata )
		return;
	// load standard information
	field_firstname->setText( buddy.firstName() );
	field_lastname->setText(  buddy.lastName()  );
	field_nickname->setText(  buddy.nickName()  );
	// load standard read-only information
	field_phone->setText(     buddy.homePhone() );
	field_mobile->setText(    buddy.mobile()    );
	field_email->setText(     buddy.email()     );
	field_www->setText(       buddy.website()   );
	// load gender
	field_gender->setCurrentIndex( buddy.gender() );
	// load extended information
	field_address->setText(   bdata->address()   );
	field_city->setText(      bdata->city()      );
	field_email2->setText(    bdata->email2()    );
	field_birthday->setText(  bdata->birthday()  );
	field_nameday->setText(   bdata->nameday()   );
	field_interests->setText( bdata->interests() );
	field_notes->setText(     bdata->notes()     );
}


void ExtendedInformationWidgets::saveBuddy()
{
	// module data
	BuddyNExtInfoData *bdata = 0;
	if( buddy.data() )
		bdata = buddy.data()->moduleStorableData<BuddyNExtInfoData>( "nextinfo", 0, true );
	if( ! bdata )
		return;
	// check birthday format
	if( ! field_birthday->text().contains( QRegExp( NEXTINFO_REGEXPBIRTHDAY ) ) )  // if bad format
		field_birthday->setText( "" );
	// check name-day format
	if( ! field_nameday->text().contains( QRegExp( NEXTINFO_REGEXPNAMEDAY ) ) )  // if bad format
		field_nameday->setText( "" );
	// save standard information
	buddy.setFirstName( field_firstname->text() );
	buddy.setLastName(  field_lastname->text()  );
	buddy.setNickName(  field_nickname->text()  );
	// save gender
	int gender = field_gender->currentIndex();
	if( ( gender < 0 ) || ( gender > 2 ) )
		gender = 0;
	buddy.setGender( (BuddyGender)gender );
	// save extended information
	bdata->setAddress(   field_address->text()          );
	bdata->setCity(      field_city->text()             );
	bdata->setEmail2(    field_email2->text()           );
	bdata->setBirthday(  field_birthday->text()         );
	bdata->setNameday(   field_nameday->text()          );
	bdata->setInterests( field_interests->toPlainText() );
	bdata->setNotes(     field_notes->toPlainText()     );
	bdata->store();
}


void ExtendedInformationWidgets::avatarClicked()
{
	PhotoWidget *pw = new PhotoWidget();
	pw->setBuddy( buddy );
	pw->show();
}
