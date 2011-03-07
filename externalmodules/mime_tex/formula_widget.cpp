/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QPainter>

#include "formula_widget.h"

#include "debug.h"

const int MimeTeX::FormulaWidget::margin = 5;
const int formulaViewMinWidth = 300;
const int formulaViewMinHeight = 250;

MimeTeX::FormulaWidget::FormulaWidget(QString fileName, QWidget *parent)
: QWidget(parent)
{
	kdebugf();
	QByteArray aa = fileName.toAscii();
	kdebugm(KDEBUG_INFO, "filename=%s\n", aa.constData());
	tmpFileName = fileName;
	formulaPixmap.load(tmpFileName);
	resize(width(), height());
	setMinimumSize(formulaViewMinWidth, formulaViewMinHeight);
	setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
	kdebugf2();
}

MimeTeX::FormulaWidget::~FormulaWidget()
{
	kdebugf();
	kdebugf();
}

void MimeTeX::FormulaWidget::cleanView(QPainter &p)
{
	kdebugf();
	cleanPixmap = QPixmap(width(), height());
	cleanPixmap.fill(Qt::white);
	p.drawPixmap(0, 0, cleanPixmap);
	kdebugf2();
}

void MimeTeX::FormulaWidget::fillView(QPainter &p)
{
	kdebugf();
	cleanView(p);
	const int formulaViewMinWidth = 300;
	const int formulaViewMinHeight = 250;
	formulaPixmap.fill(Qt::white);
	formulaPixmap.load(tmpFileName);
	kdebugm(KDEBUG_INFO, "width()=%d, height=%d\n", formulaPixmap.width(), formulaPixmap.height());
	if(formulaPixmap.width() > formulaViewMinWidth)
	{
		kdebugm(KDEBUG_INFO, "Zmieniam szerokosc formulaView na %d\n", formulaPixmap.width());
		setMinimumWidth(formulaPixmap.width() + 2 * margin);
	}
	if(formulaPixmap.height() > formulaViewMinHeight)
	{
		kdebugm(KDEBUG_INFO, "Zmieniam wysokosc formulaView na %d\n", formulaPixmap.height());
		setMinimumHeight(formulaPixmap.height() + 2 * margin);
	}
	p.drawPixmap(0, 0, formulaPixmap);
	kdebugf2();
}

void MimeTeX::FormulaWidget::paintEvent (QPaintEvent *)
{
	kdebugf();
	QPainter p(this);
	fillView(p);
	kdebugf2();
}
