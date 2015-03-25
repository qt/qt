/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <private/qgraphicssystemplugin_p.h>
#include <private/qgraphicssystem_gl_p.h>
#include <qgl.h>

QT_BEGIN_NAMESPACE

class QGLGraphicsSystemPlugin : public QGraphicsSystemPlugin
{
public:
    QStringList keys() const;
    QGraphicsSystem *create(const QString&);
};

QStringList QGLGraphicsSystemPlugin::keys() const
{
    QStringList list;
    list << QLatin1String("OpenGL") << QLatin1String("OpenGL1");
#if !defined(QT_OPENGL_ES_1)
    list << QLatin1String("OpenGL2");
#endif
#if defined(Q_WS_X11) && !defined(QT_NO_EGL)
    list << QLatin1String("X11GL");
#endif
    return list;
}

QGraphicsSystem* QGLGraphicsSystemPlugin::create(const QString& system)
{
    if (system.toLower() == QLatin1String("opengl1")) {
        QGL::setPreferredPaintEngine(QPaintEngine::OpenGL);
        return new QGLGraphicsSystem(false);
    }

#if !defined(QT_OPENGL_ES_1)
    if (system.toLower() == QLatin1String("opengl2")) {
        QGL::setPreferredPaintEngine(QPaintEngine::OpenGL2);
        return new QGLGraphicsSystem(false);
    }
#endif

#if defined(Q_WS_X11) && !defined(QT_NO_EGL)
    if (system.toLower() == QLatin1String("x11gl"))
        return new QGLGraphicsSystem(true);
#endif

    if (system.toLower() == QLatin1String("opengl"))
        return new QGLGraphicsSystem(false);

    return 0;
}

Q_EXPORT_PLUGIN2(opengl, QGLGraphicsSystemPlugin)

QT_END_NAMESPACE
