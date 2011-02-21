// Commit: ebd4bc73c46c2962742a682b6a391fb68c482aec
/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSGFLIPABLE_P_H
#define QSGFLIPABLE_P_H

#include "qsgitem.h"

#include <QtGui/qtransform.h>
#include <QtGui/qvector3d.h>
#include <QtCore/qobject.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QSGFlipablePrivate;
class Q_AUTOTEST_EXPORT QSGFlipable : public QSGItem
{
    Q_OBJECT

    Q_ENUMS(Side)
    Q_PROPERTY(QSGItem *front READ front WRITE setFront NOTIFY frontChanged)
    Q_PROPERTY(QSGItem *back READ back WRITE setBack NOTIFY backChanged)
    Q_PROPERTY(Side side READ side NOTIFY sideChanged)
    //### flipAxis
    //### flipRotation
public:
    QSGFlipable(QSGItem *parent=0);
    ~QSGFlipable();

    QSGItem *front();
    void setFront(QSGItem *);

    QSGItem *back();
    void setBack(QSGItem *);

    enum Side { Front, Back };
    Side side() const;

Q_SIGNALS:
    void frontChanged();
    void backChanged();
    void sideChanged();

protected:
    virtual void updatePolish();

private Q_SLOTS:
    void retransformBack();

private:
    Q_DISABLE_COPY(QSGFlipable)
    Q_DECLARE_PRIVATE(QSGFlipable)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QSGFlipable)

QT_END_HEADER

#endif // QSGFLIPABLE_P_H
