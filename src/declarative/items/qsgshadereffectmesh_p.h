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

#include "qdeclarativeparserstatus.h"

#include <QtCore/qobject.h>
#include <QtCore/qsize.h>
#include <QtCore/qvariant.h>
#include <QtOpenGL/qglfunctions.h>

#ifndef SHADEREFFECTMESH_H
#define SHADEREFFECTMESH_H

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QSGGeometry;
class QRectF;

class QSGShaderEffectMesh : public QObject
{
    Q_OBJECT
public:
    QSGShaderEffectMesh(QObject *parent = 0);
    // If 'geometry' != 0, 'attributes' is the same as last time the function was called.
    virtual QSGGeometry *updateGeometry(QSGGeometry *geometry, const QVector<QByteArray> &attributes, const QRectF &rect) const = 0;

Q_SIGNALS:
    // Emitted when the geometry needs to be updated.
    void geometryChanged();
};

class QSGGridMesh : public QSGShaderEffectMesh
{
    Q_OBJECT
    Q_PROPERTY(QSize resolution READ resolution WRITE setResolution NOTIFY resolutionChanged)
public:
    QSGGridMesh(QObject *parent = 0);
    virtual QSGGeometry *updateGeometry(QSGGeometry *geometry, const QVector<QByteArray> &attributes, const QRectF &rect) const;

    void setResolution(const QSize &res);
    QSize resolution() const;

Q_SIGNALS:
    void resolutionChanged();

private:
    QSize m_resolution;
};

inline QColor qt_premultiply_color(const QColor &c)
{
    return QColor::fromRgbF(c.redF() * c.alphaF(), c.greenF() * c.alphaF(), c.blueF() * c.alphaF(), c.alphaF());
}


QT_END_NAMESPACE

QT_END_HEADER

#endif // SHADEREFFECTITEM_H
