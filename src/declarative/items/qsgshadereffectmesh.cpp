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

    QRectF srcRect(0, 0, 1, 1);
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


class CustomMeshGeometry : public QSGGeometry
{
public:
    // Takes ownership over 'attribs'. 'attribs' is a pointer to a memory block consisting of
    // an AttributeSet and a number of Attributes. The memory block is freed with free().
    CustomMeshGeometry(QSGGeometry::AttributeSet *attribs, int vertexCount,
                       int indexCount = 0, int indexType = GL_UNSIGNED_SHORT);
    ~CustomMeshGeometry();

private:
    AttributeSet *m_attributes;
};

CustomMeshGeometry::CustomMeshGeometry(QSGGeometry::AttributeSet *attribs, int vertexCount,
                                       int indexCount, int indexType)
    : QSGGeometry(*attribs, vertexCount, indexCount, indexType)
    , m_attributes(attribs)
{
    Q_ASSERT(m_attributes);
}

CustomMeshGeometry::~CustomMeshGeometry()
{
    free(m_attributes);
}


QSGCustomMesh::QSGCustomMesh(QObject *parent)
    : QSGShaderEffectMesh(parent)
    , m_primitiveType(TriangleStrip)
{
    connect(this, SIGNAL(primitiveTypeChanged()), this, SIGNAL(geometryChanged()));
    connect(this, SIGNAL(indicesChanged()), this, SIGNAL(geometryChanged()));
}

void QSGCustomMesh::componentComplete()
{
    int beginProperty = QSGCustomMesh::staticMetaObject.propertyCount();
    int endProperty = metaObject()->propertyCount();
    for (int i = beginProperty; i < endProperty; ++i) {
        QMetaProperty mp = metaObject()->property(i);
        if (!mp.hasNotifySignal())
            qWarning("QSGCustomMesh: Property '%s' does not have notification method", mp.name());
        QByteArray signalName("2");
        signalName.append(mp.notifySignal().signature());
        connect(this, signalName, this, SIGNAL(geometryChanged()));
    }
}

QSGGeometry *QSGCustomMesh::updateGeometry(QSGGeometry *geometryPtr, const QVector<QByteArray> &attributes, const QRectF &) const
{
    typedef QSGGeometry::Attribute Attribute;
    typedef QSGGeometry::AttributeSet AttributeSet;

    // Automatic clean-up if returning early.
    QScopedPointer<QSGGeometry> geometry(geometryPtr);
    QScopedPointer<AttributeSet, QScopedPointerPodDeleter> attrSet
        ((AttributeSet *)malloc(sizeof(AttributeSet) + attributes.count() * sizeof(Attribute)));
    attrSet->count = attributes.count();
    attrSet->stride = 0;
    attrSet->attributes = (Attribute *)(attrSet.data() + 1);

    Q_ASSERT(geometry.isNull() || geometry->attributeCount() == attributes.count());

    int vertexCount = 0;
    for (int i = 0; i < attributes.count(); ++i) {
        Attribute &attr = const_cast<Attribute &>(attrSet->attributes[i]);
        attr.position = i;

        QVariant p = property(attributes.at(i).constData());
        if (!p.isValid()) {
            qWarning("QSGCustomMesh: Attribute '%s' does not have a matching property.", attributes.at(i).constData());
            return 0;
        }
        if (p.type() != QVariant::List) {
            qWarning("QSGCustomMesh: Property '%s' is not a list.", attributes.at(i).constData());
            return 0;
        }
        QVariantList list = p.toList();
        if (list.isEmpty()) {
            qWarning("QSGCustomMesh: Property '%s' is an empty list.", attributes.at(i).constData());
            return 0;
        }
        p = list.first();
        if (i == 0) {
            vertexCount = list.count();
        } else if (vertexCount != list.count()) {
            qWarning("QSGCustomMesh: The length of '%s' (%i) is different from '%s' (%i).",
                     attributes.at(i).constData(), list.count(),
                     attributes.at(0).constData(), vertexCount);
            return 0;
        }

        switch (p.type()) {
        case QVariant::Color:
            attr.tupleSize = 4;
            attr.type = GL_UNSIGNED_BYTE;
            attrSet->stride += 4;
            break;
        case QVariant::Double:
        case QVariant::Int:
            attr.tupleSize = 1;
            attr.type = GL_FLOAT;
            attrSet->stride += sizeof(GLfloat);
            break;
        case QVariant::Bool:
            attr.tupleSize = 1;
            attr.type = GL_UNSIGNED_BYTE;
            attrSet->stride += 1;
            break;
        case QVariant::Size:
        case QVariant::SizeF:
        case QVariant::Point:
        case QVariant::PointF:
            attr.tupleSize = 2;
            attr.type = GL_FLOAT;
            attrSet->stride += 2 * sizeof(GLfloat);
            break;
        case QVariant::Rect:
        case QVariant::RectF:
            attr.tupleSize = 4;
            attr.type = GL_FLOAT;
            attrSet->stride += 4 * sizeof(GLfloat);
            break;
        case QVariant::Vector3D:
            attr.tupleSize = 3;
            attr.type = GL_FLOAT;
            attrSet->stride += 3 * sizeof(GLfloat);
            break;
        default:
            qWarning("QSGCustomMesh: The type of items in list property '%s' are unsupported.",
                     attributes.at(i).constData());
            return 0;
        }
        if (!geometry.isNull() && (geometry->attributes()[i].tupleSize != attr.tupleSize
            || geometry->attributes()[i].type != attr.type))
        {
            geometry.reset(0);
        }
    }

    Q_ASSERT(geometry.isNull() || geometry->stride() == attrSet->stride);

    if (geometry.isNull())
        geometry.reset(new CustomMeshGeometry(attrSet.take(), vertexCount, m_indices.count()));
    else
        geometry->allocate(vertexCount, m_indices.count());

    const int stride = geometry->stride();
    GLubyte *vertexData = (GLubyte *)geometry->vertexData();
    for (int i = 0; i < attributes.count(); ++i) {
        QVariant p = property(attributes.at(i).constData());
        Q_ASSERT(p.isValid());
        Q_ASSERT(p.type() == QVariant::List);
        QVariantList list = p.toList();
        Q_ASSERT(!list.isEmpty());
        p = list.first();
        Q_ASSERT(vertexCount == list.count());

        switch (p.type()) {
        case QVariant::Color:
            for (int j = 0; j < vertexCount; ++j) {
                if (list.at(j).type() != QVariant::Color) {
                    qWarning("QSGCustomMesh: Property '%s' is a list of mixed types.", attributes.at(i).constData());
                    return 0;
                }
                QRgb color = qt_premultiply_color(qvariant_cast<QColor>(list.at(j))).rgba();
                (vertexData + stride * j)[0] = qRed(color);
                (vertexData + stride * j)[1] = qGreen(color);
                (vertexData + stride * j)[2] = qBlue(color);
                (vertexData + stride * j)[3] = qAlpha(color);
            }
            vertexData += 4;
            break;
        case QVariant::Double:
        case QVariant::Int:
            for (int j = 0; j < vertexCount; ++j) {
                if (list.at(j).type() != QVariant::Double && list.at(j).type() != QVariant::Int) {
                    qWarning("QSGCustomMesh: Property '%s' is a list of mixed types.", attributes.at(i).constData());
                    return 0;
                }
                *(GLfloat *)(vertexData + stride * j) = GLfloat(list.at(j).toReal());
            }
            vertexData += sizeof(GLfloat);
            break;
        case QVariant::Bool:
            for (int j = 0; j < vertexCount; ++j) {
                if (list.at(j).type() != QVariant::Bool) {
                    qWarning("QSGCustomMesh: Property '%s' is a list of mixed types.", attributes.at(i).constData());
                    return 0;
                }
                *(GLubyte *)(vertexData + stride * j) = list.at(j).toBool() ? 1 : 0;
            }
            vertexData += 1;
            break;
        case QVariant::Size:
        case QVariant::SizeF:
            for (int j = 0; j < vertexCount; ++j) {
                if (list.at(j).type() != QVariant::Size && list.at(j).type() != QVariant::SizeF) {
                    qWarning("QSGCustomMesh: Property '%s' is a list of mixed types.", attributes.at(i).constData());
                    return 0;
                }
                QSizeF v = list.at(j).toSizeF();
                GLfloat *p = (GLfloat *)(vertexData + stride * j);
                p[0] = GLfloat(v.width());
                p[1] = GLfloat(v.height());
            }
            vertexData += 2 * sizeof(GLfloat);
            break;
        case QVariant::Point:
        case QVariant::PointF:
            for (int j = 0; j < vertexCount; ++j) {
                if (list.at(j).type() != QVariant::Point && list.at(j).type() != QVariant::PointF) {
                    qWarning("QSGCustomMesh: Property '%s' is a list of mixed types.", attributes.at(i).constData());
                    return 0;
                }
                QPointF v = list.at(j).toPointF();
                GLfloat *p = (GLfloat *)(vertexData + stride * j);
                p[0] = GLfloat(v.x());
                p[1] = GLfloat(v.y());
            }
            vertexData += 2 * sizeof(GLfloat);
            break;
        case QVariant::Rect:
        case QVariant::RectF:
            for (int j = 0; j < vertexCount; ++j) {
                if (list.at(j).type() != QVariant::Rect && list.at(j).type() != QVariant::RectF) {
                    qWarning("QSGCustomMesh: Property '%s' is a list of mixed types.", attributes.at(i).constData());
                    return 0;
                }
                QRectF v = list.at(j).toRectF();
                GLfloat *p = (GLfloat *)(vertexData + stride * j);
                p[0] = GLfloat(v.x());
                p[1] = GLfloat(v.y());
                p[2] = GLfloat(v.width());
                p[3] = GLfloat(v.height());
            }
            vertexData += 4 * sizeof(GLfloat);
            break;
        case QVariant::Vector3D:
            for (int j = 0; j < vertexCount; ++j) {
                if (list.at(j).type() != QVariant::Vector3D) {
                    qWarning("QSGCustomMesh: Property '%s' is a list of mixed types.", attributes.at(i).constData());
                    return 0;
                }
                QVector3D v = qvariant_cast<QVector3D>(list.at(j));
                GLfloat *p = (GLfloat *)(vertexData + stride * j);
                p[0] = GLfloat(v.x());
                p[1] = GLfloat(v.y());
                p[2] = GLfloat(v.z());
            }
            vertexData += 3 * sizeof(GLfloat);
            break;
        default:
            Q_ASSERT(false);
            break;
        }
    }

    quint16 *indices = geometry->indexDataAsUShort();
    // Check properties.
    for (int i = 0; i < m_indices.count(); ++i) {
        bool ok;
        indices[i] = m_indices.at(i).toInt(&ok);
        if (!ok) {
            qWarning("QSGCustomMesh: Indices need to be integer.");
            return 0;
        }
    }

    geometry->setDrawingMode(GLenum(m_primitiveType));

    return geometry.take();
}

void QSGCustomMesh::setPrimitiveType(PrimitiveType type)
{
    if (type == m_primitiveType)
        return;
    m_primitiveType = type;
    emit primitiveTypeChanged();
}

QSGCustomMesh::PrimitiveType QSGCustomMesh::primitiveType() const
{
    return m_primitiveType;
}

void QSGCustomMesh::setIndices(const QVariantList &indices)
{
    if (!indices.isEmpty() && indices.count() == m_indices.count() && &indices.at(0) == &m_indices.at(0))
        return;
    m_indices = indices;
    emit indicesChanged();
}

QVariantList QSGCustomMesh::indices() const
{
    return m_indices;
}

QT_END_NAMESPACE
