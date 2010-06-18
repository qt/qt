/*
    Copyright (C) 2009 Jakub Wieczorek <faw217@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include <QtTest/QtTest>

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <qgraphicswebview.h>
#include <qwebpage.h>
#include <qwebframe.h>

/**
 * Starts an event loop that runs until the given signal is received.
 * Optionally the event loop
 * can return earlier on a timeout.
 *
 * \return \p true if the requested signal was received
 *         \p false on timeout
 */
static bool waitForSignal(QObject* obj, const char* signal, int timeout = 10000)
{
    QEventLoop loop;
    QObject::connect(obj, signal, &loop, SLOT(quit()));
    QTimer timer;
    QSignalSpy timeoutSpy(&timer, SIGNAL(timeout()));
    if (timeout > 0) {
        QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
        timer.setSingleShot(true);
        timer.start(timeout);
    }
    loop.exec();
    return timeoutSpy.isEmpty();
}

class tst_QGraphicsWebView : public QObject
{
    Q_OBJECT

private slots:
    void qgraphicswebview();
    void crashOnViewlessWebPages();
    void focusInputTypes();
};

void tst_QGraphicsWebView::qgraphicswebview()
{
    QGraphicsWebView item;
    item.url();
    item.title();
    item.icon();
    item.zoomFactor();
    item.history();
    item.settings();
    item.page();
    item.setPage(0);
    item.page();
    item.setUrl(QUrl());
    item.setZoomFactor(0);
    item.load(QUrl());
    item.setHtml(QString());
    item.setContent(QByteArray());
    item.isModified();
}

class WebPage : public QWebPage
{
    Q_OBJECT

public:
    WebPage(QObject* parent = 0): QWebPage(parent)
    {
    }

    QGraphicsWebView* webView;

private slots:
    // Force a webview deletion during the load.
    // It should not cause WebPage to crash due to
    // it accessing invalid pageClient pointer.
    void aborting()
    {
        delete webView;
    }
};

class GraphicsWebView : public QGraphicsWebView
{
    Q_OBJECT

public:
    GraphicsWebView(QGraphicsItem* parent = 0): QGraphicsWebView(parent)
    {
    }

    void fireMouseClick(QPointF point) {
        QGraphicsSceneMouseEvent presEv(QEvent::GraphicsSceneMousePress);
        presEv.setPos(point);
        presEv.setButton(Qt::LeftButton);
        presEv.setButtons(Qt::LeftButton);
        QGraphicsSceneMouseEvent relEv(QEvent::GraphicsSceneMouseRelease);
        relEv.setPos(point);
        relEv.setButton(Qt::LeftButton);
        relEv.setButtons(Qt::LeftButton);
        QGraphicsWebView::sceneEvent(&presEv);
        QGraphicsWebView::sceneEvent(&relEv);
    }
};

void tst_QGraphicsWebView::crashOnViewlessWebPages()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);

    QGraphicsWebView* webView = new QGraphicsWebView;
    WebPage* page = new WebPage;
    webView->setPage(page);
    page->webView = webView;
    connect(page->mainFrame(), SIGNAL(initialLayoutCompleted()), page, SLOT(aborting()));

    scene.addItem(webView);

    view.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    view.resize(600, 480);
    webView->resize(view.geometry().size());
    QTest::qWait(200);
    view.show();

    page->mainFrame()->setHtml(QString("data:text/html,"
                                            "<frameset cols=\"25%,75%\">"
                                                "<frame src=\"data:text/html,foo \">"
                                                "<frame src=\"data:text/html,bar\">"
                                            "</frameset>"));

    QVERIFY(::waitForSignal(page, SIGNAL(loadFinished(bool))));
}

void tst_QGraphicsWebView::focusInputTypes()
{
    QWebPage* page = new QWebPage;
    GraphicsWebView* webView = new GraphicsWebView;
    webView->setPage( page );
    QGraphicsView* view = new QGraphicsView;
    QGraphicsScene* scene = new QGraphicsScene(view);
    view->setScene(scene);
    scene->addItem(webView);
    view->setGeometry(QRect(0,0,500,500));
    QCoreApplication::processEvents();
    QUrl url("qrc:///resources/input_types.html");
    page->mainFrame()->load(url);
    page->mainFrame()->setFocus();

    QVERIFY(waitForSignal(page, SIGNAL(loadFinished(bool))));

    // 'text' type
    webView->fireMouseClick(QPointF(20.0, 10.0));
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6) || defined(Q_OS_SYMBIAN)
    QVERIFY(webView->inputMethodHints() & Qt::ImhNoAutoUppercase);
    QVERIFY(webView->inputMethodHints() & Qt::ImhNoPredictiveText);
#else
    QVERIFY(webView->inputMethodHints() == Qt::ImhNone);
#endif

    // 'password' field
    webView->fireMouseClick(QPointF(20.0, 60.0));
    QVERIFY(webView->inputMethodHints() & Qt::ImhHiddenText);

    // 'tel' field
    webView->fireMouseClick(QPointF(20.0, 110.0));
    QVERIFY(webView->inputMethodHints() & Qt::ImhDialableCharactersOnly);

    // 'number' field
    webView->fireMouseClick(QPointF(20.0, 160.0));
    QVERIFY(webView->inputMethodHints() & Qt::ImhDigitsOnly);

    // 'email' field
    webView->fireMouseClick(QPointF(20.0, 210.0));
    QVERIFY(webView->inputMethodHints() & Qt::ImhEmailCharactersOnly);

    // 'url' field
    webView->fireMouseClick(QPointF(20.0, 260.0));
    QVERIFY(webView->inputMethodHints() & Qt::ImhUrlCharactersOnly);

    delete webView;
    delete view;
}

QTEST_MAIN(tst_QGraphicsWebView)

#include "tst_qgraphicswebview.moc"
