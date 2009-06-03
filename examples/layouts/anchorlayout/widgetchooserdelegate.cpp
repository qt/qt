/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>

#include "widgetchooserdelegate.h"


WidgetChooserDelegate::WidgetChooserDelegate(QVector<QGraphicsLayoutItem*> *layoutItems, 
                                             QGraphicsLayout *layout, 
                                             QWidget *parent /*= 0*/)
: QStyledItemDelegate(parent), m_layoutItems(layoutItems), m_layout(layout)
{
}

static const char *edges[] = {
            "Left",
            "HCenter",
            "Right",
            "Top",
            "VCenter",
            "Bottom"
            };

QWidget *WidgetChooserDelegate::createEditor(QWidget *parent,
                                    const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const

{
    int column = index.column();
    if (column == 0 || column == 2) {
        // first and second anchor items
        QComboBox *chooser = new QComboBox(parent);
        chooser->addItem(QLatin1String("layout"), 
                qVariantFromValue(reinterpret_cast<void*>(m_layout)));
        for (int i = 0; i < m_layoutItems->count(); ++i) {
            QGraphicsLayoutItem *item = m_layoutItems->at(i);
            chooser->addItem(item->graphicsItem()->data(0).toString(), 
                qVariantFromValue(reinterpret_cast<void*>(item)));
        }
        connect(chooser, SIGNAL(currentIndexChanged(int)),
                this, SLOT(commitAndCloseEditor()));
        return chooser;
    } else if (column == 1 || column == 3) {
        // first and second anchor edges
        QComboBox *chooser = new QComboBox(parent);
        for (int i = 0; i < sizeof(edges)/sizeof(char*); ++i) {
            chooser->addItem(QLatin1String(edges[i]), i);
        }
        connect(chooser, SIGNAL(currentIndexChanged(int)),
                this, SLOT(commitAndCloseEditor()));
        return chooser;
    } else {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

void WidgetChooserDelegate::setEditorData(QWidget *editor,
                                 const QModelIndex &index) const
{
    if (QComboBox *chooser = qobject_cast<QComboBox*>(editor)) {
        int column = index.column();
        if (column >= 0 && column <= 3) {
            QVariant data = index.data(Qt::UserRole);
            int index = chooser->findData(data);
            if (index >= 0) {
                chooser->setCurrentIndex(index);
            }
        }
    } else {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void WidgetChooserDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                const QModelIndex &index) const
{
    if (QComboBox *chooser = qobject_cast<QComboBox*>(editor)) {
        int column = index.column();
        if (column >= 0 && column <= 3) {
            int currentIndex = chooser->currentIndex();
            model->setData(index, chooser->itemText(currentIndex));
            model->setData(index, chooser->itemData(currentIndex), Qt::UserRole);
        }
    } else {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

void WidgetChooserDelegate::commitAndCloseEditor()
{
    QComboBox *editor = qobject_cast<QComboBox *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}

