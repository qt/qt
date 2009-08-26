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

#ifndef QFXLOADER_H
#define QFXLOADER_H

#include <QtDeclarative/qfxitem.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QFxLoaderPrivate;
class Q_DECLARATIVE_EXPORT QFxLoader : public QFxItem
{
    Q_OBJECT

    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QFxItem *item READ item) //### NOTIFY itemChanged
    //### sourceItem
    //### sourceComponent
    //### status, progress
    //### resizeMode { NoResize, SizeLoaderToItem (default), SizeItemToLoader }

public:
    QFxLoader(QFxItem *parent=0);
    virtual ~QFxLoader();

    QUrl source() const;
    void setSource(const QUrl &);

    QFxItem *item() const;

Q_SIGNALS:
    void sourceChanged();

private:
    Q_DISABLE_COPY(QFxLoader)
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr, QFxLoader)
    Q_PRIVATE_SLOT(d_func(), void _q_sourceLoaded())
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QFxLoader)

QT_END_HEADER

#endif // QFXLOADER_H
