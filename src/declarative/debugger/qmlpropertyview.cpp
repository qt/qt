/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "qmlpropertyview_p.h"
#include <QtGui/qboxlayout.h>
#include <QtGui/qtreewidget.h>
#include <QtCore/qmetaobject.h>

QmlPropertyView::QmlPropertyView(QWidget *parent)
: QWidget(parent), m_tree(0)
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    setLayout(layout);

    m_tree = new QTreeWidget(this);
    m_tree->setHeaderLabels(QStringList() << "Property" << "Value");

    m_tree->setColumnCount(2);

    layout->addWidget(m_tree);
}

class QmlPropertyViewItem : public QObject, public QTreeWidgetItem
{
Q_OBJECT
public:
    QmlPropertyViewItem(QTreeWidget *widget);

    QObject *object;
    QMetaProperty property;

public slots:
    void refresh();
};

QmlPropertyViewItem::QmlPropertyViewItem(QTreeWidget *widget)
: QTreeWidgetItem(widget)
{
}

void QmlPropertyViewItem::refresh()
{
    setText(1, property.read(object).toString());
}

void QmlPropertyView::setObject(QObject *object)
{
    m_object = object;

    m_tree->clear();
    if(!m_object)
        return;

    const QMetaObject *mo = object->metaObject();
    for(int ii = 0; ii < mo->propertyCount(); ++ii) {
        QmlPropertyViewItem *item = new QmlPropertyViewItem(m_tree);

        QMetaProperty p = mo->property(ii);
        item->object = object;
        item->property = p;

        item->setText(0, QLatin1String(p.name()));

        static int refreshIdx = -1;
        if(refreshIdx == -1) 
            refreshIdx = QmlPropertyViewItem::staticMetaObject.indexOfMethod("refresh()");

        if(p.hasNotifySignal()) 
            QMetaObject::connect(object, p.notifySignalIndex(), 
                                 item, refreshIdx);

        item->refresh();
    }
}

void QmlPropertyView::refresh()
{
    setObject(m_object);
}

#include "qmlpropertyview.moc"
