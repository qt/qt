/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#ifndef Q3TITLEBAR_P_H
#define Q3TITLEBAR_P_H

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

#include "qwidget.h"
#include "qstyleoption.h"

QT_BEGIN_NAMESPACE

#if !defined(QT_NO_TITLEBAR)

class QToolTip;
class Q3TitleBarPrivate;
class QPixmap;

class Q_COMPAT_EXPORT Q3TitleBar : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Q3TitleBar)
    Q_PROPERTY(bool autoRaise READ autoRaise WRITE setAutoRaise)
    Q_PROPERTY(bool movable READ isMovable WRITE setMovable)

public:
    Q3TitleBar (QWidget *w, QWidget *parent, Qt::WindowFlags f = 0);
    ~Q3TitleBar();

    bool isActive() const;
    bool usesActiveColor() const;

    bool isMovable() const;
    void setMovable(bool);

    bool autoRaise() const;
    void setAutoRaise(bool);

    QWidget *window() const;
    bool isTool() const;

    QSize sizeHint() const;
    QStyleOptionTitleBar getStyleOption() const;

    // These flags are fake, stored in QTitleBarPrivate. They are not set as the widget's window flags.
    void setFakeWindowFlags(Qt::WindowFlags f);
    Qt::WindowFlags fakeWindowFlags() const;

public Q_SLOTS:
    void setActive(bool);

Q_SIGNALS:
    void doActivate();
    void doNormal();
    void doClose();
    void doMaximize();
    void doMinimize();
    void doShade();
    void showOperationMenu();
    void popupOperationMenu(const QPoint&);
    void doubleClicked();

protected:
    bool event(QEvent *);
    void resizeEvent(QResizeEvent *);
    void contextMenuEvent(QContextMenuEvent *);
    void changeEvent(QEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);
    void paintEvent(QPaintEvent *p);

    virtual void cutText();

private:
    Q_DISABLE_COPY(Q3TitleBar)
};

#endif

QT_END_NAMESPACE

#endif //Q3TITLEBAR_P_H
