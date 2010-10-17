/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtCore/QBuffer>
#include <QtCore/QTimer>
#include <QtGui/QCursor>
#include <QtGui/QGraphicsPixmapItem>
#include <QtGui/QGraphicsProxyWidget>
#include <QtGui/QResizeEvent>

#include "gui/graphics-items/selection-frame-item.h"
#include "gui/widgets/screenshot-tool-box.h"

#include "crop-image-widget.h"

#define HANDLER_SIZE 10
#define HANDLER_HALF_SIZE (HANDLER_SIZE/2)

CropImageWidget::CropImageWidget(QWidget *parent) :
		QGraphicsView(parent), IsMouseButtonPressed(false)
{
	setContentsMargins(0, 0, 0, 0);
	setFrameShape(NoFrame);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setInteractive(true);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	QGraphicsScene *graphicsScene = new QGraphicsScene(this);
	setScene(graphicsScene);

	PixmapItem = new QGraphicsPixmapItem();
	PixmapItem->setCursor(Qt::CrossCursor);
	PixmapItem->setPos(0, 0);

	scene()->addItem(PixmapItem);

	SelectionFrame = new SelectionFrameItem();
	SelectionFrame->setPos(0, 0);
	SelectionFrame->setSize(size());

	scene()->addItem(SelectionFrame);

	ToolBox = new ScreenshotToolBox();
	connect(ToolBox, SIGNAL(crop()), this, SLOT(crop()));

	ToolBoxTimer = new QTimer(this);
	connect(ToolBoxTimer, SIGNAL(timeout()), this, SLOT(updateToolBoxFileSizeHint()));
	ToolBoxTimer->start(1000);

	ToolBoxProxy = new QGraphicsProxyWidget();
	ToolBoxProxy->setWidget(ToolBox);
	scene()->addItem(ToolBoxProxy);

	TopLeftHandler = new HandlerRectItem(HandlerTopLeft, HANDLER_SIZE);
	TopLeftHandler->setCursor(Qt::SizeFDiagCursor);
	connect(TopLeftHandler, SIGNAL(movedTo(HandlerType,int,int)), this, SLOT(handlerMovedTo(HandlerType,int,int)));
	scene()->addItem(TopLeftHandler);

	TopHandler = new HandlerRectItem(HandlerTop, HANDLER_SIZE);
	TopHandler->setCursor(Qt::SizeVerCursor);
	connect(TopHandler, SIGNAL(movedTo(HandlerType,int,int)), this, SLOT(handlerMovedTo(HandlerType,int,int)));
	scene()->addItem(TopHandler);

	TopRightHandler = new HandlerRectItem(HandlerTopRight, HANDLER_SIZE);
	TopRightHandler->setCursor(Qt::SizeBDiagCursor);
	connect(TopRightHandler, SIGNAL(movedTo(HandlerType,int,int)), this, SLOT(handlerMovedTo(HandlerType,int,int)));
	scene()->addItem(TopRightHandler);

	LeftHandler = new HandlerRectItem(HandlerLeft, HANDLER_SIZE);
	LeftHandler->setCursor(Qt::SizeHorCursor);
	connect(LeftHandler, SIGNAL(movedTo(HandlerType,int,int)), this, SLOT(handlerMovedTo(HandlerType,int,int)));
	scene()->addItem(LeftHandler);

	RightHandler = new HandlerRectItem(HandlerRight, HANDLER_SIZE);
	RightHandler->setCursor(Qt::SizeHorCursor);
	connect(RightHandler, SIGNAL(movedTo(HandlerType,int,int)), this, SLOT(handlerMovedTo(HandlerType,int,int)));
	scene()->addItem(RightHandler);

	BottomLeftHandler = new HandlerRectItem(HandlerBottomLeft, HANDLER_SIZE);
	BottomLeftHandler->setCursor(Qt::SizeBDiagCursor);
	connect(BottomLeftHandler, SIGNAL(movedTo(HandlerType,int,int)), this, SLOT(handlerMovedTo(HandlerType,int,int)));
	scene()->addItem(BottomLeftHandler);

	BottomHandler = new HandlerRectItem(HandlerBottom, HANDLER_SIZE);
	BottomHandler->setCursor(Qt::SizeVerCursor);
	connect(BottomHandler, SIGNAL(movedTo(HandlerType,int,int)), this, SLOT(handlerMovedTo(HandlerType,int,int)));
	scene()->addItem(BottomHandler);

	BottomRightHandler = new HandlerRectItem(HandlerBottomRight, HANDLER_SIZE);
	BottomRightHandler->setCursor(Qt::SizeFDiagCursor);
	connect(BottomRightHandler, SIGNAL(movedTo(HandlerType,int,int)), this, SLOT(handlerMovedTo(HandlerType,int,int)));
	scene()->addItem(BottomRightHandler);

	CropRect.setTopLeft(QPoint(0, 0));
	CropRect.setSize(size());
	updateCropRectDisplay();
}

CropImageWidget::~CropImageWidget()
{
}

void CropImageWidget::normalizeCropRect()
{
	CropRect = CropRect.normalized();
	updateCropRectDisplay();
}

void CropImageWidget::updateCropRectDisplay()
{
	QRect normalized = CropRect.normalized();
	SelectionFrame->setSelection(normalized);

	int xMiddle = (normalized.left() + normalized.right()) / 2;
	int yMiddle = (normalized.top() + normalized.bottom()) / 2;

	// workaround for wrong cursor on negative coordinates
	TopLeftHandler->setPos(CropRect.left() - HANDLER_HALF_SIZE, CropRect.top() - HANDLER_HALF_SIZE);
	TopHandler->setPos(xMiddle - HANDLER_HALF_SIZE, CropRect.top() - HANDLER_HALF_SIZE);
	TopRightHandler->setPos(CropRect.right() - HANDLER_HALF_SIZE, CropRect.top() - HANDLER_HALF_SIZE);
	LeftHandler->setPos(CropRect.left() - HANDLER_HALF_SIZE, yMiddle - HANDLER_HALF_SIZE);
	RightHandler->setPos(CropRect.right() - HANDLER_HALF_SIZE, yMiddle - HANDLER_HALF_SIZE);
	BottomLeftHandler->setPos(CropRect.left() - HANDLER_HALF_SIZE, CropRect.bottom() - HANDLER_HALF_SIZE);
	BottomHandler->setPos(xMiddle - HANDLER_HALF_SIZE, CropRect.bottom() - HANDLER_HALF_SIZE);
	BottomRightHandler->setPos(CropRect.right() - HANDLER_HALF_SIZE, CropRect.bottom() - HANDLER_HALF_SIZE);

	ToolBox->setGeometry(QString("%1x%2").arg(CropRect.width()).arg(CropRect.height()));
	ToolBoxProxy->setPos(xMiddle - ToolBox->width() / 2, yMiddle - ToolBox->height() / 2);

	scene()->update(scene()->sceneRect());
}

QPixmap CropImageWidget::croppedPixmap()
{
	return PixmapItem->pixmap().copy(CropRect.normalized());
}

void CropImageWidget::handlerMovedTo(HandlerType type, int x, int y)
{
	if (type == HandlerTopLeft || type == HandlerTop || type == HandlerTopRight)
		CropRect.setTop(y);
	else if (type == HandlerBottomLeft || type == HandlerBottom || type == HandlerBottomRight)
		CropRect.setBottom(y);

	if (type == HandlerTopLeft || type == HandlerLeft || type == HandlerBottomLeft)
		CropRect.setLeft(x);
	else if (type == HandlerTopRight || type == HandlerRight || type == HandlerBottomRight)
		CropRect.setRight(x);

	updateCropRectDisplay();
}

void CropImageWidget::crop()
{
	emit pixmapCropped(croppedPixmap());
}

void CropImageWidget::updateToolBoxFileSizeHint()
{
	QBuffer buffer;
	QPixmap pixmap = croppedPixmap();

	bool ret = pixmap.save(&buffer, "png");

	if (ret)
		ToolBox->setFileSize(QString::number(buffer.size()/1024) + " KB");
}

void CropImageWidget::mousePressEvent(QMouseEvent *event)
{
	QGraphicsView::mousePressEvent(event);
	if (event->isAccepted())
		return;

	if (event->button() != Qt::LeftButton)
		return;

	IsMouseButtonPressed = true;
	WasDoubleClick = false;

	NewTopLeft = event->pos();

	updateCropRectDisplay();
}

void CropImageWidget::mouseReleaseEvent(QMouseEvent *event)
{
	QGraphicsView::mouseReleaseEvent(event);
	normalizeCropRect();

	if (!IsMouseButtonPressed)
		return;

	if (event->button() != Qt::LeftButton)
		return;

	IsMouseButtonPressed = false;

	CropRect.setBottomRight(event->pos());
	normalizeCropRect();
	updateCropRectDisplay();
	updateToolBoxFileSizeHint();
}

void CropImageWidget::mouseMoveEvent(QMouseEvent *event)
{
	QGraphicsView::mouseMoveEvent(event);

	if (!IsMouseButtonPressed)
		return;

	CropRect.setTopLeft(NewTopLeft);
	CropRect.setBottomRight(event->pos());
	updateCropRectDisplay();
}

void CropImageWidget::resizeEvent(QResizeEvent *event)
{
	SelectionFrame->setSize(event->size());
    QGraphicsView::resizeEvent(event);

	scene()->setSceneRect(QRectF(0, 0, event->size().width(), event->size().height()));
}

void CropImageWidget::showEvent(QShowEvent *event)
{
    QGraphicsView::showEvent(event);

	setCropRect(frameGeometry());
}

void CropImageWidget::setPixmap(QPixmap pixmap)
{
	PixmapItem->setPixmap(pixmap);
}

void CropImageWidget::setCropRect(QRect cropRect)
{
	CropRect = cropRect.normalized();
	updateCropRectDisplay();
}
