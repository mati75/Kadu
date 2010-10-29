/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "formula_view.h"
#include "formula_widget.h"
#include "debug.h"

MimeTeX::FormulaView::FormulaView(QString tmpFileName, QWidget *parent)
: QScrollArea(parent)
{
	kdebugf();
	formulaWidget = new FormulaWidget(tmpFileName, viewport());
	setWidget(formulaWidget);
	kdebugf2();
}

MimeTeX::FormulaView::~FormulaView()
{
	kdebugf();
	kdebugf2();
}

void MimeTeX::FormulaView::fillView()
{
	kdebugf();
	if(NULL != formulaWidget)
		formulaWidget->update();
	kdebugf2();
}

