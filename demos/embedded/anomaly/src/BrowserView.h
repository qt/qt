/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Anomaly project on Qt Labs.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 or 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
** http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef BROWSERVIEW_H
#define BROWSERVIEW_H

#include <QWidget>
#include <QVector>

class QUrl;
class QWebView;
class TitleBar;
class ControlStrip;
class ZoomStrip;

class BrowserView : public QWidget
{
    Q_OBJECT

public:
    BrowserView(QWidget *parent = 0);

public slots:
    void navigate(const QUrl &url);
    void zoomIn();
    void zoomOut();
#ifdef Q_OS_SYMBIAN
    void setDefaultIap();
#endif

private slots:
    void initialize();
    void start();
    void setProgress(int percent);
    void finish(bool);
    void updateTitleBar();

signals:
    void menuButtonClicked();

protected:
    void resizeEvent(QResizeEvent *event);

private:
    TitleBar *m_titleBar;
    QWebView *m_webView;
    ZoomStrip *m_zoomStrip;
    ControlStrip *m_controlStrip;
    int m_progress;
    int m_currentZoom;
    QVector<int> m_zoomLevels;
};

#endif // BROWSERVIEW_H
