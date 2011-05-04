/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QSGMATRIX4X4STACK_H
#define QSGMATRIX4X4STACK_H

#include <QtGui/qmatrix4x4.h>
#include <QtCore/qscopedpointer.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QSGMatrix4x4StackPrivate;

class Q_DECLARATIVE_EXPORT QSGMatrix4x4Stack
{
public:
    QSGMatrix4x4Stack();
    ~QSGMatrix4x4Stack();

    const QMatrix4x4 &top() const;

    void push();
    void pop();

    void setToIdentity();

    void translate(qreal x, qreal y, qreal z);
    void translate(const QVector3D& vector);
    void scale(qreal x, qreal y, qreal z);
    void scale(qreal factor);
    void scale(const QVector3D& vector);
    void rotate(qreal angle, qreal x, qreal y, qreal z);
    void rotate(qreal angle, const QVector3D& vector);
    void rotate(const QQuaternion &quaternion);

    QSGMatrix4x4Stack& operator=(const QMatrix4x4& matrix);
    QSGMatrix4x4Stack& operator*=(const QMatrix4x4& matrix);

    operator const QMatrix4x4 &() const;

    bool isDirty() const;
    void setDirty(bool dirty);

private:
    Q_DISABLE_COPY(QSGMatrix4x4Stack)
    Q_DECLARE_PRIVATE(QSGMatrix4x4Stack)

    QScopedPointer<QSGMatrix4x4StackPrivate> d_ptr;

    friend class QGLPainter;
};

inline QSGMatrix4x4Stack::operator const QMatrix4x4 &() const
{
    return top();
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
