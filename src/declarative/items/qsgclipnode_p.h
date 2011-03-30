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

#ifndef QSGCLIPNODE_P_H
#define QSGCLIPNODE_P_H

#include <qsgnode.h>

class QSGDefaultClipNode : public QSGClipNode
{
public:
    QSGDefaultClipNode(const QRectF &);

    void setRect(const QRectF &);
    QRectF rect() const { return m_rect; }

    void setRadius(qreal radius);
    qreal radius() const { return m_radius; }

    virtual void update();

private:
    void updateGeometry();
    QRectF m_rect;
    qreal m_radius;

    uint m_dirty_geometry : 1;
    uint m_reserved : 31;

    QSGGeometry m_geometry;
};

#endif // QSGCLIPNODE_P_H
