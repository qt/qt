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

#ifndef QSGCONTEXT_H
#define QSGCONTEXT_H

#include <QObject>
#include <qabstractanimation.h>

#include "node.h"

class QSGContextPrivate;
class RectangleNodeInterface;
class TextureNodeInterface;
class GlyphNodeInterface;
class Renderer;
class QSGTextureManager;

class QGLContext;

class Q_DECLARATIVE_EXPORT QSGContext : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QSGContext)

public:
    explicit QSGContext(QObject *parent = 0);

    void initialize(QGLContext *context);

    Renderer *renderer() const;

    QSGTextureManager *textureManager() const;

    void setRootNode(RootNode *node);
    RootNode *rootNode() const;

    QGLContext *glContext() const;

    static QSGContext *current; // Evil nasty hack!! Get rid of this!

    bool isReady() const;

    virtual void renderNextFrame();

    virtual RectangleNodeInterface *createRectangleNode();
    virtual TextureNodeInterface *createTextureNode();
    virtual GlyphNodeInterface *createGlyphNode();
    virtual GlyphNodeInterface *createGlyphNode(const QFont &font);
    virtual Renderer *createRenderer();
    virtual QSGTextureManager *createTextureManager();
    virtual QAnimationDriver *createAnimationDriver(QWidget *window);

    static QSGContext *createDefaultContext();

signals:
    void ready();

    void aboutToRenderNextFrame();
};

#endif // QSGCONTEXT_H
