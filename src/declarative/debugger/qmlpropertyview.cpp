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
#include <QtCore/qdebug.h>
#include <QtDeclarative/qmlbindablevalue.h>
#include <private/qmlboundsignal_p.h>

QmlPropertyView::QmlPropertyView(QmlWatches *watches, QWidget *parent)
: QWidget(parent), m_tree(0), m_watches(watches)
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    setLayout(layout);

    m_tree = new QTreeWidget(this);
    m_tree->setHeaderLabels(QStringList() << tr("Property") << tr("Value"));
    QObject::connect(m_tree, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
                     this, SLOT(itemDoubleClicked(QTreeWidgetItem *)));
    QObject::connect(m_tree, SIGNAL(itemClicked(QTreeWidgetItem *, int)),
                     this, SLOT(itemClicked(QTreeWidgetItem *)));

    m_tree->setColumnCount(2);

    layout->addWidget(m_tree);
}

class QmlPropertyViewItem : public QObject, public QTreeWidgetItem
{
Q_OBJECT
public:
    QmlPropertyViewItem(QTreeWidget *widget);
    QmlPropertyViewItem(QTreeWidgetItem *parent);

    QObject *object;
    QMetaProperty property;

    quint32 exprId;

public slots:
    void refresh();
};

QmlPropertyViewItem::QmlPropertyViewItem(QTreeWidget *widget)
: QTreeWidgetItem(widget), object(0), exprId(0)
{
}

QmlPropertyViewItem::QmlPropertyViewItem(QTreeWidgetItem *parent)
: QTreeWidgetItem(parent), object(0), exprId(0)
{
}

void QmlPropertyViewItem::refresh()
{
    QVariant v = property.read(object);
    QString str = v.toString();

    if (QmlMetaType::isObject(v.userType())) 
        str = QmlWatches::objectToString(QmlMetaType::toQObject(v));

    setText(1, str);
}

void QmlPropertyView::itemClicked(QTreeWidgetItem *i)
{
    QmlPropertyViewItem *item = static_cast<QmlPropertyViewItem *>(i);

    if(item->object) {
        QVariant v = item->property.read(item->object);
        if (QmlMetaType::isObject(v.userType())) {
            QObject *obj = QmlMetaType::toQObject(v);

            if(obj) {
                quint32 id = m_watches->objectId(obj);
                emit objectClicked(id);
            }
        }
    }

}

void QmlPropertyView::itemDoubleClicked(QTreeWidgetItem *i)
{
    QmlPropertyViewItem *item = static_cast<QmlPropertyViewItem *>(i);

    if(item->object) {
        quint32 objectId = m_watches->objectId(item->object);

        if(m_watches->hasWatch(objectId, item->property.name())) {
            m_watches->remWatch(objectId, item->property.name());
            item->setForeground(0, Qt::black);
        } else {
            m_watches->addWatch(objectId, item->property.name());
            item->setForeground(0, Qt::red);
        }
    }

}

void QmlPropertyView::setObject(QObject *object)
{
    m_object = object;

    m_tree->clear();
    if(!m_object)
        return;

    QMultiHash<QByteArray, QPair<QString, quint32> > bindings;
    QHash<QByteArray, QString> sigs;
    QObjectList children = object->children();

    foreach(QObject *child, children) {
        if(QmlBindableValue *value = qobject_cast<QmlBindableValue *>(child)) {
            bindings.insert(value->property().name().toUtf8(), qMakePair(value->expression(), value->id()));
        } else if(QmlBoundSignal *signal = qobject_cast<QmlBoundSignal *>(child)) {
            QMetaMethod method = object->metaObject()->method(signal->index());
            QByteArray sig = method.signature();
            sigs.insert(sig, signal->expression());
        } 
    }

    quint32 objectId = m_watches->objectId(object);

    const QMetaObject *mo = object->metaObject();
    for(int ii = 0; ii < mo->propertyCount(); ++ii) {
        QMetaProperty p = mo->property(ii);

        if(QmlMetaType::isList(p.userType()) ||
           QmlMetaType::isQmlList(p.userType()))
            continue;

        QmlPropertyViewItem *item = new QmlPropertyViewItem(m_tree);

        item->object = object;
        item->property = p;

        item->setText(0, QLatin1String(p.name()));
        if(m_watches->hasWatch(objectId, p.name()))
            item->setForeground(0, Qt::red);

        static int refreshIdx = -1;
        if(refreshIdx == -1) 
            refreshIdx = QmlPropertyViewItem::staticMetaObject.indexOfMethod("refresh()");

        if(p.hasNotifySignal()) 
            QMetaObject::connect(object, p.notifySignalIndex(), 
                                 item, refreshIdx);


        QMultiHash<QByteArray, QPair<QString, quint32> >::Iterator iter = 
            bindings.find(p.name());

        while(iter != bindings.end() && iter.key() == p.name()) {
            QmlPropertyViewItem *binding = new QmlPropertyViewItem(item);
            binding->exprId = iter.value().second;
            binding->setText(1, iter.value().first);
            binding->setForeground(1, Qt::green);
            ++iter;
        }

        item->setExpanded(true);
        item->refresh();
    }
    
    for(QHash<QByteArray, QString>::ConstIterator iter = sigs.begin();
            iter != sigs.end();
            ++iter) {

        QTreeWidgetItem *item = new QTreeWidgetItem(m_tree);
        item->setText(0, QString::fromAscii(iter.key()));
        item->setForeground(0, Qt::blue);
        item->setText(1, iter.value());
    }
}

void QmlPropertyView::refresh()
{
    setObject(m_object);
}

#include "qmlpropertyview.moc"
