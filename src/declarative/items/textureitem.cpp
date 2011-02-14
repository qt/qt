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

#include "textureitem.h"
#include "qsgtexturemanager.h"
#include "qsgitem_p.h"
#include "adaptationlayer.h"
#include "renderer.h"
#include "qsgcanvas_p.h"

#include "qglframebufferobject.h"
#include "qmath.h"

QT_BEGIN_NAMESPACE

TextureItem::TextureItem(QSGItem *parent)
    : QSGItem(parent)
    , m_textureProvider(0)
{
    setFlag(ItemHasContents);
}

QSGTextureProvider *TextureItem::textureProvider() const
{
    return m_textureProvider;
}

void TextureItem::setTextureProvider(QSGTextureProvider *provider, bool requiresPreprocess)
{
    Q_ASSERT(m_textureProvider == 0); // Can only be set once.
    m_textureProvider = provider;
    m_requiresPreprocess = requiresPreprocess;
}

Node *TextureItem::updatePaintNode(Node *oldNode, UpdatePaintNodeData *data)
{
    TextureNodeInterface *node = static_cast<TextureNodeInterface *>(oldNode);
    if (!node) {
        node = QSGContext::current->createTextureNode();
        node->setFlag(Node::UsePreprocess, m_requiresPreprocess);
        node->setTexture(m_textureProvider);
    }

    m_textureProvider->setClampToEdge(true);
    m_textureProvider->setLinearFiltering(QSGItemPrivate::get(this)->smooth);

    node->setTargetRect(QRectF(0, 0, width(), height()));
    node->setSourceRect(QRectF(0, 1, 1, -1));
    node->update();

    return node;
}

QT_END_NAMESPACE
