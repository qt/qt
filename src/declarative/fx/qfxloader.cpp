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

#include "qfxloader_p.h"
#include <private/qmlengine_p.h>

QT_BEGIN_NAMESPACE

QFxLoaderPrivate::QFxLoaderPrivate()
: item(0), qmlcomp(0)
{
}

QFxLoaderPrivate::~QFxLoaderPrivate()
{
}

QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,Loader,QFxLoader)

/*!
    \qmlclass Loader
    \inherits Item

    \brief The Loader item allows you to dynamically load an Item-based
    subtree from a QML URL.
 */

/*!
    \internal
    \class QFxLoader
    \qmlclass Loader
 */

/*!
    Create a new QFxLoader instance.
 */
QFxLoader::QFxLoader(QFxItem *parent)
  : QFxItem(*(new QFxLoaderPrivate), parent)
{
}

/*!
    Destroy the loader instance.
 */
QFxLoader::~QFxLoader()
{
}

/*!
    \internal
    \fn void QFxItem::sourceChanged()
    This signal is emitted whenever the item's dynamic QML
    source url changes.
 */

/*!
    \qmlproperty url Loader::source
    This property holds the dynamic URL of the QML for the item.

    This property is used for dynamically loading QML into the
    item.
*/
QUrl QFxLoader::source() const
{
    Q_D(const QFxLoader);
    return d->source;
}

void QFxLoader::setSource(const QUrl &url)
{
    Q_D(QFxLoader);
    if (d->source == url)
        return;

    if (!d->source.isEmpty()) {
        QHash<QString, QFxItem *>::Iterator iter = d->cachedChildren.find(d->source.toString());
        if (iter != d->cachedChildren.end())
            (*iter)->setOpacity(0.);
    }

    d->source = url;
    d->item = 0;

    if (d->source.isEmpty()) {
        emit sourceChanged();
        return;
    }

    QHash<QString, QFxItem *>::Iterator iter = d->cachedChildren.find(d->source.toString());
    if (iter != d->cachedChildren.end()) {
        (*iter)->setOpacity(1.);
        d->item = (*iter);
        emit sourceChanged();
    } else {
        d->qmlcomp =
            new QmlComponent(qmlEngine(this), d->source, this);
        if (!d->qmlcomp->isLoading())
            d->_q_sourceLoaded();
        else
            connect(d->qmlcomp, SIGNAL(statusChanged(QmlComponent::Status)),
                    this, SLOT(_q_sourceLoaded()));
    }
}

void QFxLoaderPrivate::_q_sourceLoaded()
{
    Q_Q(QFxLoader);

    if (qmlcomp) {
        QmlContext *ctxt = new QmlContext(qmlContext(q));
        ctxt->addDefaultObject(q);

        if (!qmlcomp->errors().isEmpty()) {
            qWarning() << qmlcomp->errors();
            delete qmlcomp;
            qmlcomp = 0;
            emit q->sourceChanged();
            return;
        }
        QObject *obj = qmlcomp->create(ctxt);
        if (!qmlcomp->errors().isEmpty())
            qWarning() << qmlcomp->errors();
        QFxItem *qmlChild = qobject_cast<QFxItem *>(obj);
        if (qmlChild) {
            qmlChild->setParentItem(q);
            cachedChildren.insert(source.toString(), qmlChild);
            item = qmlChild;
        } else {
            delete qmlChild;
            source = QUrl();
        }
        delete qmlcomp;
        qmlcomp = 0;
        emit q->sourceChanged();
    }
}

/*!
    \qmlproperty url Loader::item
    This property holds the top-level item created from source.
*/
QFxItem *QFxLoader::item() const
{
    Q_D(const QFxLoader);
    return d->item;
}

QT_END_NAMESPACE

#include "moc_qfxloader.cpp"
