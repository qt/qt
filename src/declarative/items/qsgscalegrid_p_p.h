// Commit: ac5c099cc3c5b8c7eec7a49fdeb8a21037230350
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

#ifndef QSGSCALEGRID_P_P_H
#define QSGSCALEGRID_P_P_H

#include "qsgborderimage_p.h"

#include <QtDeclarative/qdeclarative.h>
#include <QtCore/qobject.h>

#include <private/qdeclarativepixmapcache_p.h>
#include <private/qdeclarativeglobal_p.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class Q_DECLARATIVE_PRIVATE_EXPORT QSGScaleGrid : public QObject
{
    Q_OBJECT
    Q_ENUMS(TileRule)

    Q_PROPERTY(int left READ left WRITE setLeft NOTIFY borderChanged)
    Q_PROPERTY(int top READ top WRITE setTop NOTIFY borderChanged)
    Q_PROPERTY(int right READ right WRITE setRight NOTIFY borderChanged)
    Q_PROPERTY(int bottom READ bottom WRITE setBottom NOTIFY borderChanged)

public:
    QSGScaleGrid(QObject *parent=0);
    ~QSGScaleGrid();

    bool isNull() const;

    int left() const { return _left; }
    void setLeft(int);

    int top() const { return _top; }
    void setTop(int);

    int right() const { return _right; }
    void setRight(int);

    int  bottom() const { return _bottom; }
    void setBottom(int);

Q_SIGNALS:
    void borderChanged();

private:
    int _left;
    int _top;
    int _right;
    int _bottom;
};

class Q_DECLARATIVE_PRIVATE_EXPORT QSGGridScaledImage
{
public:
    QSGGridScaledImage();
    QSGGridScaledImage(const QSGGridScaledImage &);
    QSGGridScaledImage(QIODevice*);
    QSGGridScaledImage &operator=(const QSGGridScaledImage &);
    bool isValid() const;
    int gridLeft() const;
    int gridRight() const;
    int gridTop() const;
    int gridBottom() const;
    QSGBorderImage::TileMode horizontalTileRule() const { return _h; }
    QSGBorderImage::TileMode verticalTileRule() const { return _v; }

    QString pixmapUrl() const;

private:
    static QSGBorderImage::TileMode stringToRule(const QString &);

private:
    int _l;
    int _r;
    int _t;
    int _b;
    QSGBorderImage::TileMode _h;
    QSGBorderImage::TileMode _v;
    QString _pix;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QSGScaleGrid)

QT_END_HEADER

#endif // QSGSCALEGRID_P_P_H
