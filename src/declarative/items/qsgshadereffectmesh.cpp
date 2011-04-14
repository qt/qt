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

#include "qsgshadereffectmesh_p.h"
#include "qsggeometry.h"
#include "qsgshadereffectitem_p.h"

QT_BEGIN_NAMESPACE

QSGShaderEffectMesh::QSGShaderEffectMesh(QObject *parent)
    : QObject(parent)
{
}


QSGGridMesh::QSGGridMesh(QObject *parent)
    : QSGShaderEffectMesh(parent)
    , m_resolution(1, 1)
{
    connect(this, SIGNAL(resolutionChanged()), this, SIGNAL(geometryChanged()));
}

QSGGeometry *QSGGridMesh::updateGeometry(QSGGeometry *geometry, const QVector<QByteArray> &attributes, const QRectF &dstRect) const
{
    int vmesh = m_resolution.height();
    int hmesh = m_resolution.width();
    int attrCount = attributes.count();

    if (!geometry) {
        bool error = true;
        switch (attrCount) {
        case 0:
            qWarning("QSGGridMesh:: No attributes specified.");
            break;
        case 1:
            if (attributes.at(0) == qtPositionAttributeName()) {
                error = false;
                break;
            }
            qWarning("QSGGridMesh:: Missing \'%s\' attribute.",
                     qtPositionAttributeName());
            break;
        case 2:
            if (attributes.contains(qtPositionAttributeName())
                && attributes.contains(qtTexCoordAttributeName()))
            {
                error = false;
                break;
            }
            qWarning("QSGGridMesh:: Missing \'%s\' or \'%s\' attribute.",
                     qtPositionAttributeName(), qtTexCoordAttributeName());
            break;
        default:
            qWarning("QSGGridMesh:: Too many attributes specified.");
            break;;
        }

        if (error) {
            delete geometry;
            return 0;
        }

        geometry = new QSGGeometry(attrCount == 1
                                   ? QSGGeometry::defaultAttributes_Point2D()
                                   : QSGGeometry::defaultAttributes_TexturedPoint2D(),
                                   (vmesh + 1) * (hmesh + 1), vmesh * 2 * (hmesh + 2),
                                   GL_UNSIGNED_SHORT);

    } else {
        geometry->allocate((vmesh + 1) * (hmesh + 1), vmesh * 2 * (hmesh + 2));
    }

    QSGGeometry::Point2D *vdata = static_cast<QSGGeometry::Point2D *>(geometry->vertexData());

    bool positionFirst = attributes.at(0) == qtPositionAttributeName();

    QRectF srcRect(0, 1, 1, -1);
    for (int iy = 0; iy <= vmesh; ++iy) {
        float fy = iy / float(vmesh);
        float y = float(dstRect.top()) + fy * float(dstRect.height());
        float ty = float(srcRect.top()) + fy * float(srcRect.height());
        for (int ix = 0; ix <= hmesh; ++ix) {
            float fx = ix / float(hmesh);
            for (int ia = 0; ia < attrCount; ++ia) {
                if (positionFirst == (ia == 0)) {
                    vdata->x = float(dstRect.left()) + fx * float(dstRect.width());
                    vdata->y = y;
                    ++vdata;
                } else {
                    vdata->x = float(srcRect.left()) + fx * float(srcRect.width());
                    vdata->y = ty;
                    ++vdata;
                }
            }
        }
    }

    quint16 *indices = (quint16 *)geometry->indexDataAsUShort();
    int i = 0;
    for (int iy = 0; iy < vmesh; ++iy) {
        *(indices++) = i + hmesh + 1;
        for (int ix = 0; ix <= hmesh; ++ix, ++i) {
            *(indices++) = i + hmesh + 1;
            *(indices++) = i;
        }
        *(indices++) = i - 1;
    }

    return geometry;
}

void QSGGridMesh::setResolution(const QSize &res)
{
    if (res == m_resolution)
        return;
    if (res.width() < 1 || res.height() < 1) {
        qWarning("QSGGridMesh: Resolution must be at least 1x1");
        return;
    }
    m_resolution = res;
    emit resolutionChanged();
}

QSize QSGGridMesh::resolution() const
{
    return m_resolution;
}

QT_END_NAMESPACE
