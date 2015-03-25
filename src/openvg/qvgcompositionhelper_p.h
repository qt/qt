/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtOpenVG module of the Qt Toolkit.
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

#ifndef QVGCOMPOSITIONHELPER_H
#define QVGCOMPOSITIONHELPER_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qwindowsurface_vgegl_p.h"

QT_BEGIN_NAMESPACE

#if !defined(QVG_NO_SINGLE_CONTEXT) && !defined(QT_NO_EGL)

class QVGPaintEnginePrivate;
class QVGEGLWindowSurfacePrivate;

class Q_OPENVG_EXPORT QVGCompositionHelper
{
public:
    QVGCompositionHelper();
    virtual ~QVGCompositionHelper();

    void startCompositing(const QSize& screenSize);
    void endCompositing();

    void blitWindow(VGImage image, const QSize& imageSize,
                    const QRect& rect, const QPoint& topLeft, int opacity);
    void fillBackground(const QRegion& region, const QBrush& brush);
    void drawCursorPixmap(const QPixmap& pixmap, const QPoint& offset);
    void setScissor(const QRegion& region);
    void clearScissor();

private:
    QVGPaintEnginePrivate *d;
    QSize screenSize;
};

#endif

QT_END_NAMESPACE

#endif
