/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QX11MENUBAR_P_H
#define QX11MENUBAR_P_H

#ifndef QT_NO_MENUBAR

#include "qabstractplatformmenubar_p.h"

QT_BEGIN_NAMESPACE

class QMenuBar;

class QX11MenuBar : public QAbstractPlatformMenuBar
{
public:
    ~QX11MenuBar();

    virtual void init(QMenuBar *);

    virtual void setVisible(bool visible);

    virtual void actionEvent(QActionEvent *e);

    virtual void handleReparent(QWidget *oldParent, QWidget *newParent, QWidget *oldWindow, QWidget *newWindow);

    virtual bool allowCornerWidgets() const;

    virtual void popupAction(QAction*);

    virtual void setNativeMenuBar(bool);
    virtual bool isNativeMenuBar() const;

    virtual bool shortcutsHandledByNativeMenuBar() const;
    virtual bool menuBarEventFilter(QObject *, QEvent *event);

private:
    QMenuBar *menuBar;
    int nativeMenuBar : 3;  // Only has values -1, 0, and 1
};

QPlatformMenuBarFactoryInterface *qt_guiPlatformMenuBarFactory();

QT_END_NAMESPACE

#endif // QT_NO_MENUBAR

#endif /* QX11MENUBAR_P_H */
