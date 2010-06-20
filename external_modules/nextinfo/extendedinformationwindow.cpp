/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/




#include <QApplication>
#include <QComboBox>
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

#include "icons_manager.h"

#include "extendedinformationwindow.h"

#include "defines_stringencoder.h"
NEXTINFO_ENCODESTRING
NEXTINFO_DECODESTRING




ExtendedInformationWindow::ExtendedInformationWindow( QWidget* parent, const char* name, Qt::WindowFlags f ) : QWidget( parent, name, f )
{
	// user is not set yet
	userset = false;
	// window layout
	ExtendedInformationWindowLayout = new QVBoxLayout( this, 5, 4, "ExtendedInformationWindowLayout" );
	// tabs
	tabs = new QTabWidget( this, "tabs" );
	// tab: extinfo
	extinfotab = new QWidget( tabs, "extinfotab" );
	layout_extinfotab = new QVBoxLayout( extinfotab, 5, 3, "layout_extinfotab" );
	extinfotab_separator1 = new QWidget( extinfotab, "extinfotab_separator1" );
		extinfotab_separator1->setMinimumHeight( NEXTINFO_FIELDSEPARATORSHEIGHT );
		extinfotab_separator1->resize( 1, NEXTINFO_FIELDSEPARATORSHEIGHT );
		layout_extinfotab->addWidget( extinfotab_separator1 );
	label_info = new QLabel( extinfotab, "label_info" );
		label_info->setMargin( 0 );
		label_info->setMinimumWidth( NEXTINFO_LABELINFOMINIMUMWIDTH );
		layout_extinfotab->addWidget( label_info );
	extinfotab_separator2 = new QWidget( extinfotab, "extinfotab_separator2" );
		extinfotab_separator2->setMinimumHeight( NEXTINFO_FIELDSEPARATORSHEIGHT );
		extinfotab_separator2->resize( 1, NEXTINFO_FIELDSEPARATORSHEIGHT );
		layout_extinfotab->addWidget( extinfotab_separator2 );
	layout_firstname = new QHBoxLayout( 0, 0, 6, "layout_firstname" );
		label_firstname = new QLabel( extinfotab, "label_firstname" );
		label_firstname->setAlignment( Qt::AlignRight | Qt::AlignVCenter | Qt::AlignVCenter | Qt::AlignVCenter );
		layout_firstname->addWidget( label_firstname );
		field_firstname = new QLineEdit( extinfotab, "field_firstname" );
		layout_firstname->addWidget( field_firstname );
		layout_extinfotab->addLayout( layout_firstname );
	layout_lastname = new QHBoxLayout( 0, 0, 6, "layout_lastname" );
		label_lastname = new QLabel( extinfotab, "label_lastname" );
		label_lastname->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
		layout_lastname->addWidget( label_lastname );
		field_lastname = new QLineEdit( extinfotab, "field_lastname" );
		layout_lastname->addWidget( field_lastname );
		layout_extinfotab->addLayout( layout_lastname );
	layout_nickname = new QHBoxLayout( 0, 0, 6, "layout_nickname" );
		label_nickname = new QLabel( extinfotab, "label_nickname" );
		label_nickname->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
		layout_nickname->addWidget( label_nickname );
		field_nickname = new QLineEdit( extinfotab, "field_nickname" );
		layout_nickname->addWidget( field_nickname );
		layout_extinfotab->addLayout( layout_nickname );
	layout_address = new QHBoxLayout( 0, 0, 6, "layout_address" );
		label_address = new QLabel( extinfotab, "label_address" );
		label_address->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
		layout_address->addWidget( label_address );
		field_address = new QLineEdit( extinfotab, "field_address" );
		layout_address->addWidget( field_address );
		layout_extinfotab->addLayout( layout_address );
	layout_city = new QHBoxLayout( 0, 0, 6, "layout_city" );
		label_city = new QLabel( extinfotab, "label_city" );
		label_city->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
		layout_city->addWidget( label_city );
		field_city = new QLineEdit( extinfotab, "field_city" );
		layout_city->addWidget( field_city );
		layout_extinfotab->addLayout( layout_city );
	extinfotab_separator3 = new QWidget( extinfotab, "extinfotab_separator3" );
		extinfotab_separator3->setMinimumHeight( NEXTINFO_FIELDSEPARATORSHEIGHT );
		extinfotab_separator3->resize( 1, NEXTINFO_FIELDSEPARATORSHEIGHT );
		layout_extinfotab->addWidget( extinfotab_separator3 );
	layout_sex = new QHBoxLayout( 0, 0, 6, "layout_sex" );
		label_sex = new QLabel( extinfotab, "label_sex" );
		label_sex->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
		layout_sex->addWidget( label_sex );
		field_sex = new QComboBox( false, extinfotab, "field_sex" );
		field_sex->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
		layout_sex->addWidget( field_sex );
		layout_extinfotab->addLayout( layout_sex );
	extinfotab_separator4 = new QWidget( extinfotab, "extinfotab_separator4" );
		extinfotab_separator4->setMinimumHeight( NEXTINFO_FIELDSEPARATORSHEIGHT );
		extinfotab_separator4->resize( 1, NEXTINFO_FIELDSEPARATORSHEIGHT );
		layout_extinfotab->addWidget( extinfotab_separator4 );
	layout_mobile = new QHBoxLayout( 0, 0, 6, "layout_mobile" );
		label_mobile = new QLabel( extinfotab, "label_mobile" );
		label_mobile->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
		layout_mobile->addWidget( label_mobile );
		field_mobile = new QLineEdit( extinfotab, "field_mobile" );
		layout_mobile->addWidget( field_mobile );
		layout_extinfotab->addLayout( layout_mobile );
	layout_phone2 = new QHBoxLayout( 0, 0, 6, "layout_phone2" );
		label_phone2 = new QLabel( extinfotab, "label_phone2" );
		label_phone2->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
		layout_phone2->addWidget( label_phone2 );
		field_phone2 = new QLineEdit( extinfotab, "field_phone2" );
		layout_phone2->addWidget( field_phone2 );
		layout_extinfotab->addLayout( layout_phone2 );
	extinfotab_separator5 = new QWidget( extinfotab, "extinfotab_separator5" );
		extinfotab_separator5->setMinimumHeight( NEXTINFO_FIELDSEPARATORSHEIGHT );
		extinfotab_separator5->resize( 1, NEXTINFO_FIELDSEPARATORSHEIGHT );
		layout_extinfotab->addWidget( extinfotab_separator5 );
	layout_email = new QHBoxLayout( 0, 0, 6, "layout_email" );
		label_email = new QLabel( extinfotab, "label_email" );
		label_email->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
		layout_email->addWidget( label_email );
		field_email = new QLineEdit( extinfotab, "field_email" );
		layout_email->addWidget( field_email );
		layout_extinfotab->addLayout( layout_email );
	layout_email2 = new QHBoxLayout( 0, 0, 6, "layout_email2" );
		label_email2 = new QLabel( extinfotab, "label_email2" );
		label_email2->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
		layout_email2->addWidget( label_email2 );
		field_email2 = new QLineEdit( extinfotab, "field_email2" );
		layout_email2->addWidget( field_email2 );
		layout_extinfotab->addLayout( layout_email2 );
	layout_www = new QHBoxLayout( 0, 0, 6, "layout_www" );
		label_www = new QLabel( extinfotab, "label_www" );
		label_www->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
		layout_www->addWidget( label_www );
		field_www = new QLineEdit( extinfotab, "field_www" );
		layout_www->addWidget( field_www );
		layout_extinfotab->addLayout( layout_www );
	extinfotab_separator6 = new QWidget( extinfotab, "extinfotab_separator6" );
		extinfotab_separator6->setMinimumHeight( NEXTINFO_FIELDSEPARATORSHEIGHT );
		extinfotab_separator6->resize( 1, NEXTINFO_FIELDSEPARATORSHEIGHT );
		layout_extinfotab->addWidget( extinfotab_separator6 );
	layout_gg = new QHBoxLayout( 0, 0, 6, "layout_gg" );
		label_gg = new QLabel( extinfotab, "label_gg" );
		label_gg->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
		layout_gg->addWidget( label_gg );
		field_gg = new QLineEdit( extinfotab, "field_gg" );
		layout_gg->addWidget( field_gg );
		layout_extinfotab->addLayout( layout_gg );
	layout_gg2 = new QHBoxLayout( 0, 0, 6, "layout_gg2" );
		label_gg2 = new QLabel( extinfotab, "label_gg2" );
		label_gg2->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
		layout_gg2->addWidget( label_gg2 );
		field_gg2 = new QLineEdit( extinfotab, "field_gg2" );
		layout_gg2->addWidget( field_gg2 );
		layout_extinfotab->addLayout( layout_gg2 );
	layout_irc = new QHBoxLayout( 0, 0, 6, "layout_irc" );
		label_irc = new QLabel( extinfotab, "label_irc" );
		label_irc->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
		layout_irc->addWidget( label_irc );
		field_irc = new QLineEdit( extinfotab, "field_irc" );
		layout_irc->addWidget( field_irc );
		layout_extinfotab->addLayout( layout_irc );
	layout_tlen = new QHBoxLayout( 0, 0, 6, "layout_tlen" );
		label_tlen = new QLabel( extinfotab, "label_tlen" );
		label_tlen->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
		layout_tlen->addWidget( label_tlen );
		field_tlen = new QLineEdit( extinfotab, "field_tlen" );
		layout_tlen->addWidget( field_tlen );
		layout_extinfotab->addLayout( layout_tlen );
	layout_wp = new QHBoxLayout( 0, 0, 6, "layout_wp" );
		label_wp = new QLabel( extinfotab, "label_wp" );
		label_wp->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
		layout_wp->addWidget( label_wp );
		field_wp = new QLineEdit( extinfotab, "field_wp" );
		layout_wp->addWidget( field_wp );
		layout_extinfotab->addLayout( layout_wp );
	layout_icq = new QHBoxLayout( 0, 0, 6, "layout_icq" );
		label_icq = new QLabel( extinfotab, "label_icq" );
		label_icq->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
		layout_icq->addWidget( label_icq );
		field_icq = new QLineEdit( extinfotab, "field_icq" );
		layout_icq->addWidget( field_icq );
		layout_extinfotab->addLayout( layout_icq );
	extinfotab_separator7 = new QWidget( extinfotab, "extinfotab_separator7" );
		extinfotab_separator7->setMinimumHeight( NEXTINFO_FIELDSEPARATORSHEIGHT );
		extinfotab_separator7->resize( 1, NEXTINFO_FIELDSEPARATORSHEIGHT );
		layout_extinfotab->addWidget( extinfotab_separator7 );
	layout_birthday = new QHBoxLayout( 0, 0, 6, "layout_birthday" );
		label_birthday = new QLabel( extinfotab, "label_birthday" );
		label_birthday->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
		layout_birthday->addWidget( label_birthday );
		field_birthday = new QLineEdit( extinfotab, "field_birthday" );
		layout_birthday->addWidget( field_birthday );
		layout_extinfotab->addLayout( layout_birthday );
	layout_nameday = new QHBoxLayout( 0, 0, 6, "layout_nameday" );
		label_nameday = new QLabel( extinfotab, "label_nameday" );
		label_nameday->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
		layout_nameday->addWidget( label_nameday );
		field_nameday = new QLineEdit( extinfotab, "field_nameday" );
		layout_nameday->addWidget( field_nameday );
		layout_extinfotab->addLayout( layout_nameday );
	extinfotab_separator8 = new QWidget( extinfotab, "extinfotab_separator8" );
		extinfotab_separator8->setMinimumHeight( NEXTINFO_FIELDSEPARATORSHEIGHT );
		extinfotab_separator8->resize( 1, NEXTINFO_FIELDSEPARATORSHEIGHT );
		layout_extinfotab->addWidget( extinfotab_separator8 );
	extinfotab_spacer = new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding );
		layout_extinfotab->addItem( extinfotab_spacer );
	tabs->insertTab( extinfotab, QString("") );
	// tab: notes
	notestab = new QWidget( tabs, "notestab" );
	layout_notestab = new QVBoxLayout( notestab, 5, 3, "layout_notestab" );
	notestab_separator1 = new QWidget( notestab, "notestab_separator1" );
		notestab_separator1->setMinimumHeight( NEXTINFO_FIELDSEPARATORSHEIGHT );
		notestab_separator1->resize( 1, NEXTINFO_FIELDSEPARATORSHEIGHT );
		layout_notestab->addWidget( notestab_separator1 );
	label_interests = new QLabel( notestab, "label_interests" );
		layout_notestab->addWidget( label_interests );
		field_interests = new QTextEdit( notestab, "interests" );
		field_interests->setMaximumSize( QSize( 32767, NEXTINFO_FIELDINTERESTSHEIGHT ) );
		field_interests->setTabChangesFocus( true );
		layout_notestab->addWidget( field_interests );
	notestab_separator2 = new QWidget( notestab, "notestab_separator2" );
		notestab_separator2->setMinimumHeight( NEXTINFO_FIELDSEPARATORSHEIGHT );
		notestab_separator2->resize( 1, NEXTINFO_FIELDSEPARATORSHEIGHT );
		layout_notestab->addWidget( notestab_separator2 );
	label_notes = new QLabel( notestab, "label_notes" );
		layout_notestab->addWidget( label_notes );
		field_notes = new QTextEdit( notestab, "notes" );
		field_notes->setTabChangesFocus( true );
		layout_notestab->addWidget( field_notes );
	notestab_separator3 = new QWidget( notestab, "notestab_separator3" );
		notestab_separator3->setMinimumHeight( NEXTINFO_FIELDSEPARATORSHEIGHT );
		notestab_separator3->resize( 1, NEXTINFO_FIELDSEPARATORSHEIGHT );
		layout_notestab->addWidget( notestab_separator3 );
	tabs->insertTab( notestab, QString("") );
	// tab: photo
	phototab = new QWidget( tabs, "phototab" );
	layout_phototab = new QVBoxLayout( phototab, 5, 6, "layout_phototab" );
		photoscrollarea = new QScrollArea( phototab );
		photoscrollarea->setFrameStyle( QFrame::NoFrame );
		photoscrollarea->setLineWidth( 0 );
		photoscrollarea->setAlignment( Qt::AlignCenter );
		photoscrollarea->setAttribute( Qt::WA_NoSystemBackground, true );
		QWidget *photoscrollareaviewport = new QWidget( photoscrollarea );
		photoscrollareaviewport->setAttribute( Qt::WA_NoSystemBackground, true );
		photoscrollarea->setViewport( photoscrollareaviewport );
		photo = new QLabel( phototab, "photo" );
		photo->setAlignment( Qt::AlignCenter );
		photo->setAttribute( Qt::WA_NoSystemBackground, true );
		photoscrollarea->setWidget( photo );
		photo->setAutoFillBackground( false );  // must be after photoscrollarea->setWidget( photo );
		layout_phototab->addWidget( photoscrollarea );
	layout_photobuttons = new QHBoxLayout( 0, 0, 6, "layout_photobuttons" );
		photobuttons_spacer = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
		layout_photobuttons->addItem( photobuttons_spacer );
		photobuttons_select = new QPushButton( QIconSet( icons_manager->loadIcon( "ChooseSelectPathDialogButton" ) ), "photobuttons_select", phototab, "photobuttons_select" );
		connect( photobuttons_select, SIGNAL(clicked()), this, SLOT(photoSelect()) );
		layout_photobuttons->addWidget( photobuttons_select );
		photobuttons_clear = new QPushButton( QIconSet( icons_manager->loadIcon( "RemoveSelectPathDialogButton" ) ), "photobuttons_clear", phototab, "photobuttons_clear" );
		connect( photobuttons_clear, SIGNAL(clicked()), this, SLOT(photoClear()) );
		layout_photobuttons->addWidget( photobuttons_clear );
		layout_phototab->addLayout( layout_photobuttons );
	tabs->insertTab( phototab, QString("") );
	// insert tabs widget into the window's layout
	ExtendedInformationWindowLayout->addWidget( tabs );
	// bottom buttons
	bottombuttons = new QHBoxLayout( 0, 0, 6, "bottombuttons" );
	bottombuttons_spacer = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
		bottombuttons->addItem( bottombuttons_spacer );
	button_save = new QPushButton( QIconSet( icons_manager->loadIcon( "SavePersonalInfoButton" ) ), "button_save", this, "button_save" );
		connect( button_save, SIGNAL(clicked()), this, SLOT(saveUser()) );
		bottombuttons->addWidget( button_save );
	button_close = new QPushButton( QIconSet( icons_manager->loadIcon( "OkWindowButton" ) ), "button_close", this, "button_close" );
		connect( button_close, SIGNAL(clicked()), this, SLOT(close()) );
		bottombuttons->addWidget( button_close );
	// insert bottom buttons into the window's layout
	ExtendedInformationWindowLayout->addLayout( bottombuttons );
	// change captions
	languageChange();
	// resize window to the minimum size
	resize( minimumSizeHint() );
}


ExtendedInformationWindow::~ExtendedInformationWindow()
{
}


void ExtendedInformationWindow::languageChange()
{
	// tab: extinfo
	tabs->changeTab( extinfotab, qApp->translate( "@nextinfo", "Extended information" ) );
	// tab: extinfo: labels: set texts
	label_firstname->setText( qApp->translate( "@nextinfo", "First name"            ) + ":" );
	label_lastname->setText(  qApp->translate( "@nextinfo", "Last name"             ) + ":" );
	label_nickname->setText(  qApp->translate( "@nextinfo", "Nickname"              ) + ":" );
	label_address->setText(   qApp->translate( "@nextinfo", "Address"               ) + ":" );
	label_city->setText(      qApp->translate( "@nextinfo", "City"                  ) + ":" );
	label_sex->setText(       qApp->translate( "@nextinfo", "Sex"                   ) + ":" );
	label_mobile->setText(    qApp->translate( "@nextinfo", "Mobile phone"          ) + ":" );
	label_phone2->setText(    qApp->translate( "@nextinfo", "Alternative phone"     ) + ":" );
	label_email->setText(     qApp->translate( "@nextinfo", "e-mail"                ) + ":" );
	label_email2->setText(    qApp->translate( "@nextinfo", "Alternative e-mail"    ) + ":" );
	label_www->setText(       qApp->translate( "@nextinfo", "WWW"                   ) + ":" );
	label_gg->setText(        qApp->translate( "@nextinfo", "GG"                    ) + ":" );
	label_gg2->setText(       qApp->translate( "@nextinfo", "Alternative GG"        ) + ":" );
	label_irc->setText(       qApp->translate( "@nextinfo", "IRC"                   ) + ":" );
	label_tlen->setText(      qApp->translate( "@nextinfo", "Tlen"                  ) + ":" );
	label_wp->setText(        qApp->translate( "@nextinfo", "WP"                    ) + ":" );
	label_icq->setText(       qApp->translate( "@nextinfo", "ICQ"                   ) + ":" );
	label_birthday->setText(  qApp->translate( "@nextinfo", "Birthday (DD.MM.YYYY)" ) + ":" );
	label_nameday->setText(   qApp->translate( "@nextinfo", "Name-day (DD.MM)"      ) + ":" );
	// set sex combobox items
	field_sex->insertItem( ""                                      , 0 );
	field_sex->insertItem( qApp->translate( "@nextinfo", "female" ), 1 );
	field_sex->insertItem( qApp->translate( "@nextinfo", "male"   ), 2 );
	// tab: extinfo: labels: set new sizes
	label_firstname->adjustSize();
	label_lastname->adjustSize();
	label_nickname->adjustSize();
	label_address->adjustSize();
	label_city->adjustSize();
	label_sex->adjustSize();
	label_mobile->adjustSize();
	label_phone2->adjustSize();
	label_email->adjustSize();
	label_email2->adjustSize();
	label_www->adjustSize();
	label_gg->adjustSize();
	label_gg2->adjustSize();
	label_irc->adjustSize();
	label_tlen->adjustSize();
	label_wp->adjustSize();
	label_icq->adjustSize();
	label_birthday->adjustSize();
	label_nameday->adjustSize();
	// tab: extinfo: labels: get maximum width
	int labels_width = 1;  // px
	if( label_firstname->width() > labels_width ) { labels_width = label_firstname->width(); }
	if( label_lastname->width()  > labels_width ) { labels_width = label_lastname->width();  }
	if( label_nickname->width()  > labels_width ) { labels_width = label_nickname->width();  }
	if( label_address->width()   > labels_width ) { labels_width = label_address->width();   }
	if( label_city->width()      > labels_width ) { labels_width = label_city->width();      }
	if( label_sex->width()       > labels_width ) { labels_width = label_sex->width();       }
	if( label_mobile->width()    > labels_width ) { labels_width = label_mobile->width();    }
	if( label_phone2->width()    > labels_width ) { labels_width = label_phone2->width();    }
	if( label_email->width()     > labels_width ) { labels_width = label_email->width();     }
	if( label_email2->width()    > labels_width ) { labels_width = label_email2->width();    }
	if( label_www->width()       > labels_width ) { labels_width = label_www->width();       }
	if( label_gg->width()        > labels_width ) { labels_width = label_gg->width();        }
	if( label_gg2->width()       > labels_width ) { labels_width = label_gg2->width();       }
	if( label_irc->width()       > labels_width ) { labels_width = label_irc->width();       }
	if( label_tlen->width()      > labels_width ) { labels_width = label_tlen->width();      }
	if( label_wp->width()        > labels_width ) { labels_width = label_wp->width();        }
	if( label_icq->width()       > labels_width ) { labels_width = label_icq->width();       }
	if( label_birthday->width()  > labels_width ) { labels_width = label_birthday->width();  }
	if( label_nameday->width()   > labels_width ) { labels_width = label_nameday->width();   }
	// tab: extinfo: labels: set width of each label to the maximum width
	label_firstname->setMinimumWidth( labels_width );
	label_lastname->setMinimumWidth(  labels_width );
	label_nickname->setMinimumWidth(  labels_width );
	label_address->setMinimumWidth(   labels_width );
	label_city->setMinimumWidth(      labels_width );
	label_sex->setMinimumWidth(       labels_width );
	label_mobile->setMinimumWidth(    labels_width );
	label_phone2->setMinimumWidth(    labels_width );
	label_email->setMinimumWidth(     labels_width );
	label_email2->setMinimumWidth(    labels_width );
	label_www->setMinimumWidth(       labels_width );
	label_gg->setMinimumWidth(        labels_width );
	label_gg2->setMinimumWidth(       labels_width );
	label_irc->setMinimumWidth(       labels_width );
	label_tlen->setMinimumWidth(      labels_width );
	label_wp->setMinimumWidth(        labels_width );
	label_icq->setMinimumWidth(       labels_width );
	label_birthday->setMinimumWidth(  labels_width );
	label_nameday->setMinimumWidth(   labels_width );
	// tab: extinfo: labels: update labels' geometry
	label_firstname->updateGeometry();
	label_lastname->updateGeometry();
	label_nickname->updateGeometry();
	label_address->updateGeometry();
	label_city->updateGeometry();
	label_sex->updateGeometry();
	label_mobile->updateGeometry();
	label_phone2->updateGeometry();
	label_email->updateGeometry();
	label_email2->updateGeometry();
	label_www->updateGeometry();
	label_gg->updateGeometry();
	label_gg2->updateGeometry();
	label_irc->updateGeometry();
	label_tlen->updateGeometry();
	label_wp->updateGeometry();
	label_icq->updateGeometry();
	label_birthday->updateGeometry();
	label_nameday->updateGeometry();
	// tab: notes
	tabs->changeTab( notestab, qApp->translate( "@nextinfo", "Notes" ) );
	label_interests->setText( qApp->translate( "@nextinfo", "Interests" ) + ":" );
	label_notes->setText(     qApp->translate( "@nextinfo", "Notes"     ) + ":" );
	// tab: photo
	tabs->changeTab( phototab, qApp->translate( "@nextinfo", "Photo" ) );
	photobuttons_select->setText( qApp->translate( "@nextinfo", "Select ..." ) );
	photobuttons_clear->setText(  qApp->translate( "@nextinfo", "Clear"      ) );
	// bottom buttons
	button_save->setText(  qApp->translate( "@nextinfo", "Save"  ) );
	button_close->setText( qApp->translate( "@nextinfo", "Close" ) );
	// reload user information
	if( userset )
		setUser( user );
}


void ExtendedInformationWindow::setUser( UserListElement usertoset )
{
	// set the user
	user = usertoset;
	userset = true;
	// update window caption
	this->setCaption( qApp->translate( "@nextinfo", "Kadu - extended information for %%" ).replace( "%%", user.altNick() ) );
	// update info label
	label_info->setText( qApp->translate( "@nextinfo", "Extended information for %%" ).replace( "%%", QString("<b>")+user.altNick()+QString("</b>") ) );
	// load simple information
	field_firstname->setText( user.firstName() );
	field_lastname->setText(  user.lastName()  );
	field_nickname->setText(  user.nickName()  );
	field_mobile->setText(    user.mobile()    );
	field_email->setText(     user.email()     );
	// load "Gadu" protocol's UIN
	field_gg->setText(        user.ID( "Gadu" ) );
	// load extended information
	field_address->setText(                 user.data( "nextinfo_address"   ).toString()   );
	field_city->setText(                    user.data( "nextinfo_city"      ).toString()   );
	field_sex->setCurrentItem(              user.data( "nextinfo_sex"       ).toInt()      );
	field_phone2->setText(                  user.data( "nextinfo_phone2"    ).toString()   );
	field_email2->setText(                  user.data( "nextinfo_email2"    ).toString()   );
	field_www->setText(                     user.data( "nextinfo_www"       ).toString()   );
	field_gg2->setText(                     user.data( "nextinfo_gg2"       ).toString()   );
	field_irc->setText(                     user.data( "nextinfo_irc"       ).toString()   );
	field_tlen->setText(                    user.data( "nextinfo_tlen"      ).toString()   );
	field_wp->setText(                      user.data( "nextinfo_wp"        ).toString()   );
	field_icq->setText(                     user.data( "nextinfo_icq"       ).toString()   );
	field_birthday->setText(                user.data( "nextinfo_birthday"  ).toString()   );
	field_nameday->setText(                 user.data( "nextinfo_nameday"   ).toString()   );
	field_interests->setText( decodeString( user.data( "nextinfo_interests" ).toString() ) );
	field_notes->setText(     decodeString( user.data( "nextinfo_notes"     ).toString() ) );
	// load photo
	photopath = user.data( "nextinfo_photo" ).toString().stripWhiteSpace();
	if( photopath.isNull() )
		photopath = "";
	QPixmap photopixmap;
	if( ( ! photopath.isEmpty() ) && photopixmap.load( photopath ) )
	{
		photo->clear();
		photo->setPixmap( photopixmap );
	}
	else
	{
		photopath = "";
		photo->setText( qApp->translate( "@nextinfo", "No photo" ) );
	}
	photo->adjustSize();
}


void ExtendedInformationWindow::saveUser()
{
	// check birthday format
	if( field_birthday->text().find( QRegExp( NEXTINFO_REGEXPBIRTHDAY ) ) == -1 )  // if bad format
		field_birthday->setText( "" );
	// check name-day format
	if( field_nameday->text().find( QRegExp( NEXTINFO_REGEXPNAMEDAY ) ) == -1 )  // if bad format
		field_nameday->setText( "" );
	// save simple information
	user.setData( "FirstName" , QVariant( field_firstname->text() ), true, false );
	user.setData( "LastName"  , QVariant( field_lastname->text()  ), true, false );
	user.setData( "NickName"  , QVariant( field_nickname->text()  ), true, false );
	user.setData( "Mobile"    , QVariant( field_mobile->text()    ), true, false );
	user.setData( "Email"     , QVariant( field_email->text()     ), true, false );
	// save "Gadu" protocol's UIN
	field_gg->setText( field_gg->text().stripWhiteSpace() );
	QString id = QString::number( 0 );
	if( ! field_gg->text().isEmpty() )
		id = field_gg->text();
	if( user.usesProtocol( "Gadu" ) )  // there was an UIN so far?
	{
		if( user.ID( "Gadu" ) != id )  // uin was changed
		{
			user.deleteProtocol( "Gadu" );
			if( id.toUInt() != 0 )  // but it might be deleted
				user.addProtocol( "Gadu" , id );
		}
	}
	else  // there was no UIN so far
		if( id.toUInt() != 0 )  // if it was filled, then we add new protocol
			user.addProtocol( "Gadu", id );
	// save extended information
	user.setData( "nextinfo_address"  , QVariant(               field_address->text()     ), true, false );
	user.setData( "nextinfo_city"     , QVariant(               field_city->text()        ), true, false );
	user.setData( "nextinfo_sex"      , QVariant(               field_sex->currentItem()  ), true, false );
	user.setData( "nextinfo_phone2"   , QVariant(               field_phone2->text()      ), true, false );
	user.setData( "nextinfo_email2"   , QVariant(               field_email2->text()      ), true, false );
	user.setData( "nextinfo_www"      , QVariant(               field_www->text()         ), true, false );
	user.setData( "nextinfo_gg2"      , QVariant(               field_gg2->text()         ), true, false );
	user.setData( "nextinfo_irc"      , QVariant(               field_irc->text()         ), true, false );
	user.setData( "nextinfo_tlen"     , QVariant(               field_tlen->text()        ), true, false );
	user.setData( "nextinfo_wp"       , QVariant(               field_wp->text()          ), true, false );
	user.setData( "nextinfo_icq"      , QVariant(               field_icq->text()         ), true, false );
	user.setData( "nextinfo_birthday" , QVariant(               field_birthday->text()    ), true, false );
	user.setData( "nextinfo_nameday"  , QVariant(               field_nameday->text()     ), true, false );
	user.setData( "nextinfo_interests", QVariant( encodeString( field_interests->text() ) ), true, false );
	user.setData( "nextinfo_notes"    , QVariant( encodeString( field_notes->text()     ) ), true, false );
	// save photo
	user.setData( "nextinfo_photo"    , QVariant(               photopath                 ), true, true  );  // last change (!)
}


void ExtendedInformationWindow::photoSelect()
{
	// start path for QFileDialog
	QString startpath = photopath;
	int lastslashposition = startpath.findRev( "/" );
	if( lastslashposition != -1 )
	{
		startpath = startpath.left( lastslashposition + 1 );  // only the directory path
	}
	else
	{
		startpath = "/home/";
	}
	// QFileDialog
	QString newphotopath = QFileDialog::getOpenFileName( startpath, QString::null, this, 0, qApp->translate( "@nextinfo", "Select a photo" ) );
	if( newphotopath.isNull() )
		newphotopath = "";
	QPixmap photopixmap;
	if( ( ! newphotopath.isEmpty() ) && photopixmap.load( newphotopath ) )
	{
		photopath = newphotopath;
		photo->clear();
		photo->setPixmap( photopixmap );
		photo->adjustSize();
	}
	else
	{
		if( ! newphotopath.isEmpty() )  // a file was selected, but it is not an displayable image
		{
			// display invalid image warning
			QMessageBox *messagebox = new QMessageBox(
					qApp->translate( "@nextinfo", "Kadu - extended information" ),
					qApp->translate( "@nextinfo", "Selected file is not an image." ),
					QMessageBox::Warning,
					QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton,
					this
				);
			messagebox->show();
		}
	}
}


void ExtendedInformationWindow::photoClear()
{
	photopath = "";
	photo->clear();
	photo->setText( qApp->translate( "@nextinfo", "No photo" ) );
	photo->adjustSize();
}
