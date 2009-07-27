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

#ifndef QFXFLIPABLE_H
#define QFXFLIPABLE_H

#include <QtCore/QObject>
#include <QtGui/QTransform>
#include <QtDeclarative/qfxitem.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QGraphicsAxis;
class QFxFlipablePrivate;
class Q_DECLARATIVE_EXPORT QFxFlipable : public QFxItem
{
    Q_OBJECT

    Q_ENUMS(Side)
    Q_PROPERTY(QFxItem *front READ front WRITE setFront)
    Q_PROPERTY(QFxItem *back READ back WRITE setBack)
    Q_PROPERTY(QGraphicsAxis *axis READ axis WRITE setAxis)
    Q_PROPERTY(qreal rotation READ rotation WRITE setRotation)
    Q_PROPERTY(Side side READ side NOTIFY sideChanged)
public:
    QFxFlipable(QFxItem *parent=0);
    ~QFxFlipable();

    QFxItem *front();
    void setFront(QFxItem *);

    QFxItem *back();
    void setBack(QFxItem *);

    QGraphicsAxis *axis();
    void setAxis(QGraphicsAxis *axis);

    qreal rotation() const;
    void setRotation(qreal angle);

    enum Side { Front, Back };
    Side side() const;

protected:
    virtual void transformChanged(const QTransform &);

Q_SIGNALS:
    void sideChanged();

private:
    Q_PRIVATE_SLOT(d_func(), void _q_updateAxis())
    Q_DISABLE_COPY(QFxFlipable)
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr, QFxFlipable)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QFxFlipable)

QT_END_HEADER

#endif // QFXFLIPABLE_H
