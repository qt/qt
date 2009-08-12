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

#ifndef QMLVALUETYPE_P_H
#define QMLVALUETYPE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qobject.h>
#include <QtCore/qrect.h>
#include <QtCore/qvariant.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qfont.h>

QT_BEGIN_NAMESPACE

class QmlValueType : public QObject
{
    Q_OBJECT
public:
    QmlValueType(QObject *parent = 0);
    virtual void read(QObject *, int) = 0;
    virtual void write(QObject *, int) = 0;
};

class QmlValueTypeFactory
{
public:
    QmlValueTypeFactory();
    ~QmlValueTypeFactory();
    static QmlValueType *valueType(int);

    QmlValueType *valueTypes[QVariant::UserType - 1]; 
    QmlValueType *operator[](int idx) const { return valueTypes[idx]; }
};

class QmlPointFValueType : public QmlValueType
{
    Q_PROPERTY(qreal x READ x WRITE setX);
    Q_PROPERTY(qreal y READ y WRITE setY);
    Q_OBJECT
public:
    QmlPointFValueType(QObject *parent = 0);

    virtual void read(QObject *, int);
    virtual void write(QObject *, int);

    qreal x() const;
    qreal y() const;
    void setX(qreal);
    void setY(qreal);

private:
    QPointF point;
};

class QmlPointValueType : public QmlValueType
{
    Q_PROPERTY(int x READ x WRITE setX);
    Q_PROPERTY(int y READ y WRITE setY);
    Q_OBJECT
public:
    QmlPointValueType(QObject *parent = 0);

    virtual void read(QObject *, int);
    virtual void write(QObject *, int);

    int x() const;
    int y() const;
    void setX(int);
    void setY(int);

private:
    QPoint point;
};

class QmlSizeFValueType : public QmlValueType
{
    Q_PROPERTY(qreal width READ width WRITE setWidth);
    Q_PROPERTY(qreal height READ height WRITE setHeight);
    Q_OBJECT
public:
    QmlSizeFValueType(QObject *parent = 0);

    virtual void read(QObject *, int);
    virtual void write(QObject *, int);

    qreal width() const;
    qreal height() const;
    void setWidth(qreal);
    void setHeight(qreal);

private:
    QSizeF size;
};

class QmlSizeValueType : public QmlValueType
{
    Q_PROPERTY(int width READ width WRITE setWidth);
    Q_PROPERTY(int height READ height WRITE setHeight);
    Q_OBJECT
public:
    QmlSizeValueType(QObject *parent = 0);

    virtual void read(QObject *, int);
    virtual void write(QObject *, int);

    int width() const;
    int height() const;
    void setWidth(int);
    void setHeight(int);

private:
    QSize size;
};

class QmlRectFValueType : public QmlValueType
{
    Q_PROPERTY(qreal x READ x WRITE setX);
    Q_PROPERTY(qreal y READ y WRITE setY);
    Q_PROPERTY(qreal width READ width WRITE setWidth);
    Q_PROPERTY(qreal height READ height WRITE setHeight);
    Q_OBJECT
public:
    QmlRectFValueType(QObject *parent = 0);

    virtual void read(QObject *, int);
    virtual void write(QObject *, int);

    qreal x() const;
    qreal y() const;
    void setX(qreal);
    void setY(qreal);
    
    qreal width() const;
    qreal height() const;
    void setWidth(qreal);
    void setHeight(qreal);

private:
    QRectF rect;
};

class QmlRectValueType : public QmlValueType
{
    Q_PROPERTY(int x READ x WRITE setX);
    Q_PROPERTY(int y READ y WRITE setY);
    Q_PROPERTY(int width READ width WRITE setWidth);
    Q_PROPERTY(int height READ height WRITE setHeight);
    Q_OBJECT
public:
    QmlRectValueType(QObject *parent = 0);

    virtual void read(QObject *, int);
    virtual void write(QObject *, int);

    int x() const;
    int y() const;
    void setX(int);
    void setY(int);
    
    int width() const;
    int height() const;
    void setWidth(int);
    void setHeight(int);

private:
    QRect rect;
};

class QmlVector3DValueType : public QmlValueType
{
    Q_PROPERTY(qreal x READ x WRITE setX);
    Q_PROPERTY(qreal y READ y WRITE setY);
    Q_PROPERTY(qreal z READ z WRITE setZ);
    Q_OBJECT
public:
    QmlVector3DValueType(QObject *parent = 0);

    virtual void read(QObject *, int);
    virtual void write(QObject *, int);

    qreal x() const;
    qreal y() const;
    qreal z() const;
    void setX(qreal);
    void setY(qreal);
    void setZ(qreal);

private:
    QVector3D vector;
};

class QmlFontValueType : public QmlValueType
{
    Q_OBJECT
    Q_PROPERTY(QString family READ family WRITE setFamily)
    Q_PROPERTY(bool bold READ bold WRITE setBold)
    Q_PROPERTY(bool italic READ italic WRITE setItalic)
    Q_PROPERTY(bool underline READ underline WRITE setUnderline)
    Q_PROPERTY(qreal pointSize READ pointSize WRITE setPointSize)
    Q_PROPERTY(int pixelSize READ pixelSize WRITE setPixelSize)
public:
    QmlFontValueType(QObject *parent = 0);

    virtual void read(QObject *, int);
    virtual void write(QObject *, int);

    QString family() const;
    void setFamily(const QString &);

    bool bold() const;
    void setBold(bool b);

    bool italic() const;
    void setItalic(bool b);

    bool underline() const;
    void setUnderline(bool b);

    qreal pointSize() const;
    void setPointSize(qreal size);

    int pixelSize() const;
    void setPixelSize(int size);

private:
    QFont font;
    bool hasPixelSize;
};

QT_END_NAMESPACE

#endif  // QMLVALUETYPE_P_H
