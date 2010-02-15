/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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
#include <qtest.h>
#include "../../../shared/util.h"
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <private/qmlgraphicswebview_p.h>
#include <private/qmlgraphicswebview_p_p.h>
#include <private/qmlgraphicspositioners_p.h>
#include <QtWebKit/qwebpage.h>
#include <QtWebKit/qwebframe.h>
#include <QtCore/qdir.h>
#include <QtCore/qfile.h>
#include <QtGui/qpainter.h>
#include "testtypes.h"

class tst_qmlgraphicswebview : public QObject
{
    Q_OBJECT
public:
    tst_qmlgraphicswebview() {}

private slots:
    void basicProperties();
    void settings();
    void historyNav();
    void multipleWindows();
    void elementAreaAt();
    void loadError();
    void setHtml();
    void javaScript();
    void cleanupTestCase();
    void pixelCache();

private:
    void checkNoErrors(const QmlComponent& component);
    QmlEngine engine;
    QString tmpDir() const
    {
        static QString tmpd = QDir::tempPath()+"/tst_qmlgraphicswebview-"
            + QDateTime::currentDateTime().toString(QLatin1String("yyyyMMddhhmmss"));
        return tmpd;
    }
};

static QString strippedHtml(QString html)
{
    html.replace(QRegExp("\\s+"),"");
    return html;
}

static QString fileContents(const QString& filename)
{
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    return QString::fromUtf8(file.readAll());
}


static void removeRecursive(const QString& dirname)
{
    QDir dir(dirname);
    QFileInfoList entries(dir.entryInfoList(QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot));
    for (int i = 0; i < entries.count(); ++i)
        if (entries[i].isDir())
            removeRecursive(entries[i].filePath());
        else
            dir.remove(entries[i].fileName());
    QDir().rmdir(dirname);
}

void tst_qmlgraphicswebview::cleanupTestCase()
{
    removeRecursive(tmpDir());
}

void tst_qmlgraphicswebview::checkNoErrors(const QmlComponent& component)
{
    // Wait until the component is ready
    QTRY_VERIFY(component.isReady());


    if (component.isError()) {
        QList<QmlError> errors = component.errors();
        for (int ii = 0; ii < errors.count(); ++ii) {
            const QmlError &error = errors.at(ii);
            QByteArray errorStr = QByteArray::number(error.line()) + ":" +
                                  QByteArray::number(error.column()) + ":" +
                                  error.description().toUtf8();
            qWarning() << errorStr;
        }
    }
    QVERIFY(!component.isError());
}

void tst_qmlgraphicswebview::basicProperties()
{
    QmlComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/basic.qml"));
    checkNoErrors(component);
    QWebSettings::enablePersistentStorage(tmpDir());

    QmlGraphicsWebView *wv = qobject_cast<QmlGraphicsWebView*>(component.create());
    QVERIFY(wv != 0);
    QTRY_COMPARE(wv->progress(), 1.0);
    QCOMPARE(wv->title(),QString("Basic"));
    QTRY_COMPARE(wv->icon().width(), 48);
    QCOMPARE(wv->icon(),QPixmap(SRCDIR "/data/basic.png"));
    QCOMPARE(wv->statusText(),QString("status here"));
    QCOMPARE(strippedHtml(fileContents(SRCDIR "/data/basic.html")), strippedHtml(wv->html()));
    QCOMPARE(wv->width(), 123.0);
    QCOMPARE(wv->preferredWidth(), 0);
    QCOMPARE(wv->preferredHeight(), 0);
    QCOMPARE(wv->zoomFactor(), 1.0);
    QCOMPARE(wv->url(), QUrl::fromLocalFile(SRCDIR "/data/basic.html"));
    QCOMPARE(wv->status(), QmlGraphicsWebView::Ready);
    QVERIFY(wv->reloadAction());
    QVERIFY(wv->reloadAction()->isEnabled());
    QVERIFY(wv->backAction());
    QVERIFY(!wv->backAction()->isEnabled());
    QVERIFY(wv->forwardAction());
    QVERIFY(!wv->forwardAction()->isEnabled());
    QVERIFY(wv->stopAction());
    QVERIFY(!wv->stopAction()->isEnabled());

    wv->setPixelCacheSize(0); // mainly testing that it doesn't crash or anything!
    QCOMPARE(wv->pixelCacheSize(),0);
    wv->reloadAction()->trigger();
    QTRY_COMPARE(wv->progress(), 1.0);
}

void tst_qmlgraphicswebview::settings()
{
    QmlComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/basic.qml"));
    checkNoErrors(component);
    QmlGraphicsWebView *wv = qobject_cast<QmlGraphicsWebView*>(component.create());
    QVERIFY(wv != 0);
    QTRY_COMPARE(wv->progress(), 1.0);

    QmlGraphicsWebSettings *s = wv->settingsObject();

    // merely tests that setting gets stored (in QWebSettings)
    // behavioural tests are in WebKit.
    for (int b=0; b<=1; ++b) {
        bool on = !!b;

        s->setAutoLoadImages(on);
        s->setDeveloperExtrasEnabled(on);
        s->setJavaEnabled(on);
        s->setJavascriptCanAccessClipboard(on);
        s->setJavascriptCanOpenWindows(on);
        s->setJavascriptEnabled(on);
        s->setLinksIncludedInFocusChain(on);
        s->setLocalContentCanAccessRemoteUrls(on);
        s->setLocalStorageDatabaseEnabled(on);
        s->setOfflineStorageDatabaseEnabled(on);
        s->setOfflineWebApplicationCacheEnabled(on);
        s->setPluginsEnabled(on);
        s->setPrintElementBackgrounds(on);
        s->setPrivateBrowsingEnabled(on);
        s->setZoomTextOnly(on);

        QVERIFY(s->autoLoadImages() == on);
        QVERIFY(s->developerExtrasEnabled() == on);
        QVERIFY(s->javaEnabled() == on);
        QVERIFY(s->javascriptCanAccessClipboard() == on);
        QVERIFY(s->javascriptCanOpenWindows() == on);
        QVERIFY(s->javascriptEnabled() == on);
        QVERIFY(s->linksIncludedInFocusChain() == on);
        QVERIFY(s->localContentCanAccessRemoteUrls() == on);
        QVERIFY(s->localStorageDatabaseEnabled() == on);
        QVERIFY(s->offlineStorageDatabaseEnabled() == on);
        QVERIFY(s->offlineWebApplicationCacheEnabled() == on);
        QVERIFY(s->pluginsEnabled() == on);
        QVERIFY(s->printElementBackgrounds() == on);
        QVERIFY(s->privateBrowsingEnabled() == on);
        QVERIFY(s->zoomTextOnly() == on);

        QVERIFY(s->property("autoLoadImages") == on);
        QVERIFY(s->property("developerExtrasEnabled") == on);
        QVERIFY(s->property("javaEnabled") == on);
        QVERIFY(s->property("javascriptCanAccessClipboard") == on);
        QVERIFY(s->property("javascriptCanOpenWindows") == on);
        QVERIFY(s->property("javascriptEnabled") == on);
        QVERIFY(s->property("linksIncludedInFocusChain") == on);
        QVERIFY(s->property("localContentCanAccessRemoteUrls") == on);
        QVERIFY(s->property("localStorageDatabaseEnabled") == on);
        QVERIFY(s->property("offlineStorageDatabaseEnabled") == on);
        QVERIFY(s->property("offlineWebApplicationCacheEnabled") == on);
        QVERIFY(s->property("pluginsEnabled") == on);
        QVERIFY(s->property("printElementBackgrounds") == on);
        QVERIFY(s->property("privateBrowsingEnabled") == on);
        QVERIFY(s->property("zoomTextOnly") == on);
    }
}

void tst_qmlgraphicswebview::historyNav()
{
    QmlComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/basic.qml"));
    checkNoErrors(component);
    QWebSettings::enablePersistentStorage(tmpDir());

    QmlGraphicsWebView *wv = qobject_cast<QmlGraphicsWebView*>(component.create());
    QVERIFY(wv != 0);
    for (int i=1; i<=2; ++i) {
        QTRY_COMPARE(wv->progress(), 1.0);
        QCOMPARE(wv->title(),QString("Basic"));
        QTRY_COMPARE(wv->icon().width(), 48);
        QCOMPARE(wv->icon(),QPixmap(SRCDIR "/data/basic.png"));
        QCOMPARE(wv->statusText(),QString("status here"));
        QCOMPARE(strippedHtml(fileContents(SRCDIR "/data/basic.html")), strippedHtml(wv->html()));
        QCOMPARE(wv->width(), 123.0);
        QCOMPARE(wv->preferredWidth(), 0);
        QCOMPARE(wv->zoomFactor(), 1.0);
        QCOMPARE(wv->url(), QUrl::fromLocalFile(SRCDIR "/data/basic.html"));
        QCOMPARE(wv->status(), QmlGraphicsWebView::Ready);
        QVERIFY(wv->reloadAction());
        QVERIFY(wv->reloadAction()->isEnabled());
        QVERIFY(wv->backAction());
        QVERIFY(!wv->backAction()->isEnabled());
        QVERIFY(wv->forwardAction());
        QVERIFY(!wv->forwardAction()->isEnabled());
        QVERIFY(wv->stopAction());
        QVERIFY(!wv->stopAction()->isEnabled());

        wv->reloadAction()->trigger();
    }

    wv->setUrl(QUrl::fromLocalFile(SRCDIR "/data/forward.html"));
    QTRY_COMPARE(wv->progress(), 1.0);
    QCOMPARE(wv->title(),QString("Forward"));
    QCOMPARE(strippedHtml(fileContents(SRCDIR "/data/forward.html")), strippedHtml(wv->html()));
    QCOMPARE(wv->url(), QUrl::fromLocalFile(SRCDIR "/data/forward.html"));
    QCOMPARE(wv->status(), QmlGraphicsWebView::Ready);
    QCOMPARE(wv->statusText(),QString(""));
    QVERIFY(wv->reloadAction());
    QVERIFY(wv->reloadAction()->isEnabled());
    QVERIFY(wv->backAction());
    QVERIFY(wv->backAction()->isEnabled());
    QVERIFY(wv->forwardAction());
    QVERIFY(!wv->forwardAction()->isEnabled());
    QVERIFY(wv->stopAction());
    QVERIFY(!wv->stopAction()->isEnabled());

    wv->backAction()->trigger();

    QTRY_COMPARE(wv->progress(), 1.0);
    QCOMPARE(wv->title(),QString("Basic"));
    QCOMPARE(strippedHtml(fileContents(SRCDIR "/data/basic.html")), strippedHtml(wv->html()));
    QCOMPARE(wv->url(), QUrl::fromLocalFile(SRCDIR "/data/basic.html"));
    QCOMPARE(wv->status(), QmlGraphicsWebView::Ready);
    QVERIFY(wv->reloadAction());
    QVERIFY(wv->reloadAction()->isEnabled());
    QVERIFY(wv->backAction());
    QVERIFY(!wv->backAction()->isEnabled());
    QVERIFY(wv->forwardAction());
    QVERIFY(wv->forwardAction()->isEnabled());
    QVERIFY(wv->stopAction());
    QVERIFY(!wv->stopAction()->isEnabled());
}

void tst_qmlgraphicswebview::multipleWindows()
{
    QmlComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/newwindows.qml"));
    checkNoErrors(component);

    QmlGraphicsGrid *grid = qobject_cast<QmlGraphicsGrid*>(component.create());
    QVERIFY(grid != 0);
    QTRY_COMPARE(grid->children().count(), 2+5); // Component, Loader, 5 WebViews
    QmlGraphicsItem* popup = qobject_cast<QmlGraphicsItem*>(grid->children().at(3)); // first popup after Component, Loaded, original.
    QVERIFY(popup != 0);
    QTRY_COMPARE(popup->x(), 150.0);
}

void tst_qmlgraphicswebview::loadError()
{
    QmlComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/loadError.qml"));
    checkNoErrors(component);
    QWebSettings::enablePersistentStorage(tmpDir());

    QmlGraphicsWebView *wv = qobject_cast<QmlGraphicsWebView*>(component.create());
    QVERIFY(wv != 0);
    for (int i=1; i<=2; ++i) {
        QTRY_COMPARE(wv->progress(), 1.0);
        QCOMPARE(wv->title(),QString(""));
        QCOMPARE(wv->statusText(),QString("")); // HTML 'status bar' text, not error message
        QCOMPARE(wv->url(), QUrl::fromLocalFile(SRCDIR "/data/does-not-exist.html")); // Unlike QWebPage, which loses url
        QCOMPARE(wv->status(), QmlGraphicsWebView::Error);

        wv->reloadAction()->trigger();
    }
}

void tst_qmlgraphicswebview::setHtml()
{
    QmlComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/sethtml.qml"));
    checkNoErrors(component);
    QmlGraphicsWebView *wv = qobject_cast<QmlGraphicsWebView*>(component.create());
    QVERIFY(wv != 0);
    QCOMPARE(wv->html(),QString("<html><head></head><body><p>This is a <b>string</b> set on the WebView</p></body></html>"));
}

void tst_qmlgraphicswebview::elementAreaAt()
{
    QmlComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/elements.qml"));
    checkNoErrors(component);
    QmlGraphicsWebView *wv = qobject_cast<QmlGraphicsWebView*>(component.create());
    QVERIFY(wv != 0);
    QTRY_COMPARE(wv->progress(), 1.0);

    QCOMPARE(wv->elementAreaAt(40,30,100,100),QRect(1,1,75,54)); // Area A in data/elements.html
    QCOMPARE(wv->elementAreaAt(130,30,200,100),QRect(78,3,110,50)); // Area B
    QCOMPARE(wv->elementAreaAt(40,30,400,400),QRect(0,0,310,100)); // Whole view
    QCOMPARE(wv->elementAreaAt(130,30,280,280),QRect(76,1,223,54)); // Area BC
    QCOMPARE(wv->elementAreaAt(130,30,400,400),QRect(0,0,310,100)); // Whole view
}

void tst_qmlgraphicswebview::javaScript()
{
    QmlComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/javaScript.qml"));
    checkNoErrors(component);
    QmlGraphicsWebView *wv = qobject_cast<QmlGraphicsWebView*>(component.create());
    QVERIFY(wv != 0);
    QTRY_COMPARE(wv->progress(), 1.0);
    QCOMPARE(wv->evaluateJavaScript("123").toInt(), 123);
    QCOMPARE(wv->evaluateJavaScript("window.status").toString(), QString("status here"));
    QCOMPARE(wv->evaluateJavaScript("window.myjsname.qmlprop").toString(), QString("qmlvalue"));
}

void tst_qmlgraphicswebview::pixelCache()
{
    QmlComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/pixelCache.qml"));
    checkNoErrors(component);
    MyWebView *wv = qobject_cast<MyWebView*>(component.create());
    QVERIFY(wv != 0);
    QTRY_COMPARE(wv->progress(), 1.0);
    QPixmap pm(150,150);
    QPainter p(&pm);
    wv->paint(&p,0,0);
    const int expected = 120*(150+128); // 120 = width of HTML page, 150=pixmap height, 128=cache extra area
    QCOMPARE(wv->pixelsPainted(), expected);
    wv->paint(&p,0,0);
    QCOMPARE(wv->pixelsPainted(), expected); // nothing new needed to be painted
    wv->setPixelCacheSize(0); // clears the cache
    wv->paint(&p,0,0);
    QCOMPARE(wv->pixelsPainted(), expected*2); // everything needed to be painted
    // Note that painted things always go into the cache (even if they don't "fit"),
    // just that they will be removed if anything else needs to be painted.
    wv->setPixelCacheSize(expected); // won't clear the cache
    wv->paint(&p,0,0);
    QCOMPARE(wv->pixelsPainted(), expected*2); // still there
    wv->setPixelCacheSize(expected-1); // too small - will clear the cache
    wv->paint(&p,0,0);
    QCOMPARE(wv->pixelsPainted(), expected*3); // repainted
}

QTEST_MAIN(tst_qmlgraphicswebview)

#include "tst_qmlgraphicswebview.moc"
