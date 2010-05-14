/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenVG module of the Qt Toolkit.
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
#include "qopenkodewindow.h"

#include <KD/NV_display.h>

#include <QtGui/private/qeglcontext_p.h>
#include <QtGui/qwidget.h>
#include <QtCore/QDebug>

QOpenKODEWindow::QOpenKODEWindow(QWidget *tlw)
    : QPlatformWindow(tlw)
{
    /* Initialize EGL display */
    EGLBoolean rvbool = eglInitialize(QEgl::display(), KD_NULL, KD_NULL);
    if (!rvbool) {
        kdLogMessage("EGL failed to initialize display\n");
    }

    kdWindow = kdCreateWindow(QEgl::display(),
                              QEgl::defaultConfig(QInternal::Widget,QEgl::OpenGL,QEgl::Renderable),
                              KD_NULL);
    if (!kdWindow) {
        qErrnoWarning(kdGetError(), "Error creating native window");
        return;
    }

    const KDint windowSize[2]  = { tlw->width(), tlw->height()-1 };
    if (kdSetWindowPropertyiv(kdWindow, KD_WINDOWPROPERTY_SIZE, windowSize)) {
        qErrnoWarning(kdGetError(), "Could not set native window size");
        return;
    }

    KDboolean visibillity(false);
    if (kdSetWindowPropertybv(kdWindow, KD_WINDOWPROPERTY_VISIBILITY, &visibillity)) {
        qErrnoWarning(kdGetError(), "Could not set visibillity to false");
    }

//    const KDboolean windowExclusive[] = { false };
//    if (kdSetWindowPropertybv(kdWindow, KD_WINDOWPROPERTY_DESKTOP_EXCLUSIVE_NV, windowExclusive)) {
//        qErrnoWarning(kdGetError(), "Could not set exclusive bit");
//        //return;
//    }
//
    const KDint windowPos[2] = { tlw->x(), tlw->y() };
    if (kdSetWindowPropertyiv(kdWindow, KD_WINDOWPROPERTY_DESKTOP_OFFSET_NV, windowPos)) {
        qErrnoWarning(kdGetError(), "Could not set native window position");
        return;
    }

    if (kdRealizeWindow(kdWindow, &eglWindow)) {
        qErrnoWarning(kdGetError(), "Could not realize native window");
        return;
    }
}

QOpenKODEWindow::~QOpenKODEWindow()
{
    qDebug() << "destroying window";
    kdDestroyWindow(kdWindow);
}
void QOpenKODEWindow::setGeometry(const QRect &rect)
{
    const QRect geo = geometry();
    if (geo.size() != rect.size()) {
        const KDint windowSize[2]  = { rect.width(), rect.height() };
        if (kdSetWindowPropertyiv(kdWindow, KD_WINDOWPROPERTY_SIZE, windowSize)) {
            qErrnoWarning(kdGetError(), "Could not set native window size");
            //return;
        }
    }

    if (geo.topLeft() != rect.topLeft()) {
        const KDint windowPos[2] = { rect.x(), rect.y() };
        if (kdSetWindowPropertyiv(kdWindow, KD_WINDOWPROPERTY_DESKTOP_OFFSET_NV, windowPos)) {
            qErrnoWarning(kdGetError(), "Could not set native window position");
            //return;
        }
    }

}

void QOpenKODEWindow::setVisible(bool visible)
{
    KDboolean visibillity(visible);
    if (kdSetWindowPropertybv(kdWindow, KD_WINDOWPROPERTY_VISIBILITY, &visibillity)) {
        qErrnoWarning(kdGetError(), "Could not set visibillity to false");
    }
}
