/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demos of the Qt Toolkit.
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

#ifndef BROWSERWINDOW_H
#define BROWSERWINDOW_H

#include <QWidget>
class QPropertyAnimation;
class QUrl;

class BrowserView;
class HomeView;

class BrowserWindow : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal slideValue READ slideValue WRITE setSlideValue)

public:
    BrowserWindow();

private slots:
    void navigate(const QUrl &url);
    void gotoAddress(const QString &address);
    void animationFinished();

public slots:
    void showBrowserView();
    void showHomeView();

protected:
    void keyReleaseEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    void setSlideValue(qreal);
    qreal slideValue() const;

    QWidget *m_slidingSurface;
    HomeView *m_homeView;
    BrowserView *m_browserView;
    QPropertyAnimation *m_animation;
};

#endif // BROWSERWINDOW_H
