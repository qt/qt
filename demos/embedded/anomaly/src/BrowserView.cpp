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

#include "BrowserView.h"

#include <QtGui>
#include <QtNetwork>
#include <QtWebKit>

#include "ControlStrip.h"
#include "TitleBar.h"
#include "flickcharm.h"
#include "ZoomStrip.h"

BrowserView::BrowserView(QWidget *parent)
    : QWidget(parent)
    , m_titleBar(0)
    , m_webView(0)
    , m_progress(0)
    , m_currentZoom(100)
{
    m_titleBar = new TitleBar(this);
    m_webView = new QWebView(this);
    m_zoomStrip = new ZoomStrip(this);
    m_controlStrip = new ControlStrip(this);

    m_zoomLevels << 30 << 50 << 67 << 80 << 90;
    m_zoomLevels << 100;
    m_zoomLevels << 110 << 120 << 133 << 150 << 170 << 200 << 240 << 300;

    QTimer::singleShot(0, this, SLOT(initialize()));
}

void BrowserView::initialize()
{
    connect(m_zoomStrip, SIGNAL(zoomInClicked()), SLOT(zoomIn()));
    connect(m_zoomStrip, SIGNAL(zoomOutClicked()), SLOT(zoomOut()));

    connect(m_controlStrip, SIGNAL(menuClicked()), SIGNAL(menuButtonClicked()));
    connect(m_controlStrip, SIGNAL(backClicked()), m_webView, SLOT(back()));
    connect(m_controlStrip, SIGNAL(forwardClicked()), m_webView, SLOT(forward()));

    QPalette pal = m_webView->palette();
    pal.setBrush(QPalette::Base, Qt::white);
    m_webView->setPalette(pal);

    FlickCharm *flickCharm = new FlickCharm(this);
    flickCharm->activateOn(m_webView);

    m_webView->setZoomFactor(static_cast<qreal>(m_currentZoom)/100.0);
    connect(m_webView, SIGNAL(loadStarted()), SLOT(start()));
    connect(m_webView, SIGNAL(loadProgress(int)), SLOT(setProgress(int)));
    connect(m_webView, SIGNAL(loadFinished(bool)), SLOT(finish(bool)));
    connect(m_webView, SIGNAL(urlChanged(QUrl)), SLOT(updateTitleBar()));

    m_webView->load(QUrl("http://news.bbc.co.uk/text_only.stm"));
    m_webView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_webView->setFocus();
}

void BrowserView::start()
{
    m_progress = 0;
    updateTitleBar();
    //m_titleBar->setText(m_webView->url().toString());
}

void BrowserView::setProgress(int percent)
{
    m_progress = percent;
    updateTitleBar();
    //m_titleBar->setText(QString("Loading %1%").arg(percent));
}

void BrowserView::updateTitleBar()
{
    QUrl url = m_webView->url();
    m_titleBar->setHost(url.host());
    m_titleBar->setTitle(m_webView->title());
    m_titleBar->setProgress(m_progress);
}

void BrowserView::finish(bool ok)
{
    m_progress = 0;
    updateTitleBar();

    // TODO: handle error
    if (!ok) {
        //m_titleBar->setText("Loading failed.");
    }
}

void BrowserView::zoomIn()
{
    int i = m_zoomLevels.indexOf(m_currentZoom);
    Q_ASSERT(i >= 0);
    if (i < m_zoomLevels.count() - 1)
        m_currentZoom = m_zoomLevels[i + 1];

    m_webView->setZoomFactor(static_cast<qreal>(m_currentZoom)/100.0);
}

void BrowserView::zoomOut()
{
    int i = m_zoomLevels.indexOf(m_currentZoom);
    Q_ASSERT(i >= 0);
    if (i > 0)
        m_currentZoom = m_zoomLevels[i - 1];

    m_webView->setZoomFactor(static_cast<qreal>(m_currentZoom)/100.0);
}

void BrowserView::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    int h1 = m_titleBar->sizeHint().height();
    int h2 = m_controlStrip->sizeHint().height();

    m_titleBar->setGeometry(0, 0, width(), h1);
    m_controlStrip->setGeometry(0, height() - h2, width(), h2);
    m_webView->setGeometry(0, h1, width(), height() - h1);

    int zw = m_zoomStrip->sizeHint().width();
    int zh = m_zoomStrip->sizeHint().height();
    m_zoomStrip->move(width() - zw, (height() - zh) / 2);
}

void BrowserView::navigate(const QUrl &url)
{
    m_webView->load(url);
}
