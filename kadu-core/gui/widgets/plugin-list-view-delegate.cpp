/*
 * This file is part of the KDE project
 * Copyright (C) 2007-2008 Rafael Fernández López <ereslibre@kde.org>
 * Copyright (C) 2008 Kevin Ottens <ervin@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "plugin-list-view-delegate.h"
// #include "kwidgetitemdelegate_p.h"

#include <QIcon>
#include <QSize>
#include <QStyle>
#include <QEvent>
#include <QHoverEvent>
#include <QFocusEvent>
#include <QCursor>
#include <QTimer>
#include <QBitmap>
#include <QLayout>
#include <QPainter>
#include <QScrollBar>
#include <QKeyEvent>
#include <QApplication>
#include <QStyleOption>
#include <QPaintEngine>
#include <QCoreApplication>
#include <QAbstractItemView>
#include <QAbstractProxyModel>
#include <QTreeView>

#include "gui/widgets/plugin-list-view-delegate-widgets.h"

Q_DECLARE_METATYPE(QList<QEvent::Type>)

/**
  Private class that helps to provide binary compatibility between releases.
  @internal
*/
//@cond PRIVATE
PluginListViewDelegatePrivate::PluginListViewDelegatePrivate(PluginListViewDelegate *q, QObject *parent)
                : QObject(parent)
                , itemView(0)
                , widgetPool(new PluginListViewDelegateWidgets(q))
                , model(0)
                , viewDestroyed(false)
                , q(q)
{
}

PluginListViewDelegatePrivate::~PluginListViewDelegatePrivate()
{
        if (!viewDestroyed)
        {
                widgetPool->fullClear();
        }

        delete widgetPool;
}

void PluginListViewDelegatePrivate::_k_slotRowsInserted(const QModelIndex &parent, int start, int end)
{
        Q_UNUSED(end);
        // We need to update the rows behind the inserted row as well because the widgets need to be
        // moved to their new position
        updateRowRange(parent, start, model->rowCount(parent), false);
}

void PluginListViewDelegatePrivate::_k_slotRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
        updateRowRange(parent, start, end, true);
}

void PluginListViewDelegatePrivate::_k_slotRowsRemoved(const QModelIndex &parent, int start, int end)
{
        Q_UNUSED(end);
        // We need to update the rows that come behind the deleted rows because the widgets need to be
        // moved to the new position
        updateRowRange(parent, start, model->rowCount(parent), false);
}

void PluginListViewDelegatePrivate::_k_slotDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
        for (int i = topLeft.row(); i <= bottomRight.row(); ++i)
        {
                for (int j = topLeft.column(); j <= bottomRight.column(); ++j)
                {
                        const QModelIndex index = model->index(i, j, topLeft.parent());
                        QStyleOptionViewItemV4 optionView;
                        optionView.initFrom(itemView->viewport());
                        optionView.rect = itemView->visualRect(index);
                        widgetPool->findWidgets(index, optionView);
                }
        }
}

void PluginListViewDelegatePrivate::_k_slotLayoutChanged()
{
        foreach (QWidget *widget, widgetPool->invalidIndexesWidgets())
        {
                widget->setVisible(false);
        }

        QTimer::singleShot(0, this, SLOT(initializeModel()));
}

void PluginListViewDelegatePrivate::_k_slotModelReset()
{
        widgetPool->fullClear();
        QTimer::singleShot(0, this, SLOT(initializeModel()));
}

void PluginListViewDelegatePrivate::updateRowRange(const QModelIndex &parent, int start, int end, bool isRemoving)
{
        int i = start;

        while (i <= end)
        {
                for (int j = 0; j < model->columnCount(parent); ++j)
                {
                        const QModelIndex index = model->index(i, j, parent);
                        QStyleOptionViewItemV4 optionView;
                        optionView.initFrom(itemView->viewport());
                        optionView.rect = itemView->visualRect(index);

                        QList<QWidget*> widgetList = widgetPool->findWidgets(index, optionView, isRemoving ? PluginListViewDelegateWidgets::NotUpdateWidgets
                                                     : PluginListViewDelegateWidgets::UpdateWidgets);

                        if (isRemoving)
                        {
                                widgetPool->d->allocatedWidgets.removeAll(widgetList);
                                foreach (QWidget *widget, widgetList)
                                {
                                        const QModelIndex idx = widgetPool->d->widgetInIndex[widget];
                                        widgetPool->d->usedWidgets.remove(idx);
                                        widgetPool->d->widgetInIndex.remove(widget);
                                        delete widget;
                                }
                        }
                }

                i++;
        }
}

void PluginListViewDelegatePrivate::initializeModel(const QModelIndex &parent)
{
        if (!model)
        {
                return;
        }

        for (int i = 0; i < model->rowCount(parent); ++i)
        {
                for (int j = 0; j < model->columnCount(parent); ++j)
                {
                        const QModelIndex index = model->index(i, j, parent);

                        if (index.isValid())
                        {
                                QStyleOptionViewItemV4 optionView;
                                optionView.initFrom(itemView->viewport());
                                optionView.rect = itemView->visualRect(index);
                                widgetPool->findWidgets(index, optionView);
                        }
                }

                // Check if we need to go recursively through the children of parent (if any) to initialize
                // all possible indexes that are shown.
                const QModelIndex index = model->index(i, 0, parent);

                if (index.isValid() && model->hasChildren(index))
                {
                        initializeModel(index);
                }
        }
}

PluginListViewDelegate::PluginListViewDelegate(QAbstractItemView *itemView, QObject *parent)
                : QAbstractItemDelegate(parent)
                , d(new PluginListViewDelegatePrivate(this))
{
        Q_ASSERT(itemView);

        itemView->setMouseTracking(true);
        itemView->viewport()->setAttribute(Qt::WA_Hover);

        d->itemView = itemView;

        itemView->viewport()->installEventFilter(d); // mouse events
        itemView->installEventFilter(d);             // keyboard events

        if (qobject_cast<QTreeView*>(itemView))
        {
                connect(itemView,  SIGNAL(collapsed(QModelIndex)),
                        d, SLOT(initializeModel()));
                connect(itemView,  SIGNAL(expanded(QModelIndex)),
                        d, SLOT(initializeModel()));
        }
}

PluginListViewDelegate::~PluginListViewDelegate()
{
        delete d;
}

QAbstractItemView *PluginListViewDelegate::itemView() const
{
        return d->itemView;
}

QPersistentModelIndex PluginListViewDelegate::focusedIndex() const
{
        const QPersistentModelIndex idx = d->widgetPool->d->widgetInIndex.value(QApplication::focusWidget());

        if (idx.isValid())
        {
                return idx;
        }

        // Use the mouse position, if the widget refused to take keyboard focus.
        const QPoint pos = d->itemView->viewport()->mapFromGlobal(QCursor::pos());

        return d->itemView->indexAt(pos);
}

bool PluginListViewDelegatePrivate::eventFilter(QObject *watched, QEvent *event)
{
        if (event->type() == QEvent::Destroy)
        {
                // we care for the view since it deletes the widgets (parentage).
                // if the view hasn't been deleted, it might be that just the
                // delegate is removed from it, in which case we need to remove the widgets
                // manually, otherwise they still get drawn.
                if (watched == itemView)
                {
                        viewDestroyed = true;
                }

                return false;
        }

        Q_ASSERT(itemView);

        if (model != itemView->model())
        {
                if (model)
                {
                        disconnect(model, SIGNAL(rowsInserted(QModelIndex, int, int)), q, SLOT(_k_slotRowsInserted(QModelIndex, int, int)));
                        disconnect(model, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), q, SLOT(_k_slotRowsAboutToBeRemoved(QModelIndex, int, int)));
                        disconnect(model, SIGNAL(rowsRemoved(QModelIndex, int, int)), q, SLOT(_k_slotRowsRemoved(QModelIndex, int, int)));
                        disconnect(model, SIGNAL(dataChanged(QModelIndex, QModelIndex)), q, SLOT(_k_slotDataChanged(QModelIndex, QModelIndex)));
                        disconnect(model, SIGNAL(layoutChanged()), q, SLOT(_k_slotLayoutChanged()));
                        disconnect(model, SIGNAL(modelReset()), q, SLOT(_k_slotModelReset()));
                }

                model = itemView->model();

                connect(model, SIGNAL(rowsInserted(QModelIndex, int, int)), q, SLOT(_k_slotRowsInserted(QModelIndex, int, int)));
                connect(model, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), q, SLOT(_k_slotRowsAboutToBeRemoved(QModelIndex, int, int)));
                connect(model, SIGNAL(rowsRemoved(QModelIndex, int, int)), q, SLOT(_k_slotRowsRemoved(QModelIndex, int, int)));
                connect(model, SIGNAL(dataChanged(QModelIndex, QModelIndex)), q, SLOT(_k_slotDataChanged(QModelIndex, QModelIndex)));
                connect(model, SIGNAL(layoutChanged()), q, SLOT(_k_slotLayoutChanged()));
                connect(model, SIGNAL(modelReset()), q, SLOT(_k_slotModelReset()));
                QTimer::singleShot(0, this, SLOT(initializeModel()));
        }

        switch (event->type())
        {

                case QEvent::Polish:

                case QEvent::Resize:

                        if (!qobject_cast<QAbstractItemView*>(watched))
                        {
                                QTimer::singleShot(0, this, SLOT(initializeModel()));
                        }

                        break;

                default:
                        break;
        }

        return QObject::eventFilter(watched, event);
}

void PluginListViewDelegate::setBlockedEventTypes(QWidget *widget, QList<QEvent::Type> types) const
{
        widget->setProperty("goya:blockedEventTypes", qVariantFromValue(types));
}

QList<QEvent::Type> PluginListViewDelegate::blockedEventTypes(QWidget *widget) const
{
        return widget->property("goya:blockedEventTypes").value<QList<QEvent::Type> >();
}
