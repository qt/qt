/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qglcustomshaderstage_p.h"
#include "qglengineshadermanager_p.h"
#include "qpaintengineex_opengl2_p.h"
#include <private/qpainter_p.h>

class QGLCustomShaderStagePrivate
{
public:
    QGLCustomShaderStagePrivate() :
        m_manager(0) {}

    QGLEngineShaderManager* m_manager;
    QByteArray              m_source;
};




QGLCustomShaderStage::QGLCustomShaderStage()
    : d_ptr(new QGLCustomShaderStagePrivate)
{
}

QGLCustomShaderStage::~QGLCustomShaderStage()
{
    Q_D(QGLCustomShaderStage);
    if (d->m_manager)
        d->m_manager->removeCustomStage(this);
}

void QGLCustomShaderStage::setUniformsDirty()
{
    Q_D(QGLCustomShaderStage);
    if (d->m_manager)
        d->m_manager->setDirty(); // ### Probably a bit overkill!
}

bool QGLCustomShaderStage::setOnPainter(QPainter* p)
{
    Q_D(QGLCustomShaderStage);
    if (p->paintEngine()->type() != QPaintEngine::OpenGL2) {
        qWarning("QGLCustomShaderStage::setOnPainter() - paint engine not OpenGL2");
        return false;
    }

    // Might as well go through the paint engine to get to the context
    const QGLContext* ctx = static_cast<QGL2PaintEngineEx*>(p->paintEngine())->context();
    d->m_manager = QGLEngineShaderManager::managerForContext(ctx);
    Q_ASSERT(d->m_manager);

    d->m_manager->setCustomStage(this);
    return true;
}

const char* QGLCustomShaderStage::source()
{
    Q_D(QGLCustomShaderStage);
    return d->m_source.constData();
}

void QGLCustomShaderStage::setSource(const QByteArray& s)
{
    Q_D(QGLCustomShaderStage);
    d->m_source = s;
}
