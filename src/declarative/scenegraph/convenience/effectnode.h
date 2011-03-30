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

#ifndef EFFECTNODE_H
#define EFFECTNODE_H

#include "node.h"
#include "quadnode.h"

#include <qobject.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QGLFramebufferObject;

class EffectSubtreeNode : public QObject, public GeometryNode
{
    Q_OBJECT
public:
    EffectSubtreeNode();
    ~EffectSubtreeNode();

    void preprocess();

    QSize size() const { return m_size; }
    void setSize(const QSize &size);

    QSize margins() const { return m_margins; }
    void setMargins(const QSize &margins);

    QRectF sourceRect() const;

    int subtreeCount() const { return m_subtrees.size(); }
    void setSubtreeCount(int count);

    RootNode *subtree(int index) const { Q_ASSERT(index >= 0 && index < m_subtrees.size()); return m_subtrees.at(index).node; }
    void setSubtree(RootNode *node, int index);

    void setSubtreeMatrix(const QMatrix4x4 &matrix, int index);
    QMatrix4x4 subtreeMatrix(int index) const { Q_ASSERT(index >= 0 && index < m_subtrees.size()); return m_subtrees.at(index).matrix; }

protected slots:
    void sceneGraphChanged();

signals:
    void textureCreated(uint id, const QSize &size, int sourceIndex);
    void repaintRequired();
    void aboutToRender();

protected:
    struct SourceData
    {
        QMatrix4x4 matrix;
        RootNode *node;
        Renderer *renderer;
        QGLFramebufferObject *fbo;
    };
    QVector<SourceData> m_subtrees;
    QSize m_size;
    QSize m_margins;

    bool m_dirty;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // EFFECTNODE_H
