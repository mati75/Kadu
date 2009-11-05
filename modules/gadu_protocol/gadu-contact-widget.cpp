/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

#include "gadu-contact.h"
#include "gadu-contact-data-manager.h"
#include "misc/misc.h"

#include "gadu-contact-widget.h"

GaduContactWidget::GaduContactWidget(GaduContact *contactAccountData, QWidget *parent) :
		ContactAccountDataWidget(new GaduContactDataManager(contactAccountData, parent),
			contactAccountData, parent),
		Data(contactAccountData)
{
	setAttribute(Qt::WA_DeleteOnClose);

	createGui();
}

GaduContactWidget::~GaduContactWidget()
{
}

void GaduContactWidget::createGui()
{
	appendUiFile(dataPath("kadu/modules/configuration/gadu_contact.ui"));
}
