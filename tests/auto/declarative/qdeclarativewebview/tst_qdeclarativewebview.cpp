/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#include <qtest.h>
#include <QtTest/QSignalSpy>
#include "../../../shared/util.h"
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <private/qdeclarativepositioners_p.h>
#include <QtWebKit/qwebpage.h>
#include <QtWebKit/qwebframe.h>
#include <QtCore/qdir.h>
#include <QtCore/qfile.h>
#include <QtGui/qpainter.h>

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

class tst_qdeclarativewebview : public QObject
{
    Q_OBJECT
public:
    tst_qdeclarativewebview() {}

private slots:
    void initTestCase();
    void basicProperties();
    void settings();
    void historyNav();
    void multipleWindows();
    void elementAreaAt();
    void loadError();
    void setHtml();
    void javaScript();
    void cleanupTestCase();
    //void pixelCache();
    void newWindowParent();
    void newWindowComponent();
    void renderingEnabled();
    void pressGrabTime();

private:
    void checkNoErrors(const QDeclarativeComponent& component);
    QDeclarativeEngine engine;
    QString tmpDir() const
    {
        static QString tmpd = QDir::tempPath()+"/tst_qdeclarativewebview-"
            + QDateTime::currentDateTime().toString(QLatin1String("yyyyMMddhhmmss"));
        return tmpd;
    }
};

void tst_qdeclarativewebview::initTestCase()
{
}

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

void tst_qdeclarativewebview::cleanupTestCase()
{
    removeRecursive(tmpDir());
}

void tst_qdeclarativewebview::checkNoErrors(const QDeclarativeComponent& component)
{
    // Wait until the component is ready
    QTRY_VERIFY(component.isReady() || component.isError());

    if (component.isError()) {
        QList<QDeclarativeError> errors = component.errors();
        for (int ii = 0; ii < errors.count(); ++ii) {
            const QDeclarativeError &error = errors.at(ii);
            QByteArray errorStr = QByteArray::number(error.line()) + ":" +
                                  QByteArray::number(error.column()) + ":" +
                                  error.description().toUtf8();
            qWarning() << errorStr;
        }
    }
    QVERIFY(!component.isError());
}

void tst_qdeclarativewebview::basicProperties()
{
    QDeclarativeComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/basic.qml"));
    checkNoErrors(component);
    QWebSettings::enablePersistentStorage(tmpDir());

    QObject *wv = component.create();
    QVERIFY(wv != 0);
    QTRY_COMPARE(wv->property("progress").toDouble(), 1.0);
    QCOMPARE(wv->property("title").toString(),QString("Basic"));
    QTRY_COMPARE(qvariant_cast<QPixmap>(wv->property("icon")).width(), 48);
    QCOMPARE(qvariant_cast<QPixmap>(wv->property("icon")),QPixmap(SRCDIR "/data/basic.png"));
    QCOMPARE(wv->property("statusText").toString(),QString("status here"));
    QCOMPARE(strippedHtml(fileContents(SRCDIR "/data/basic.html")), strippedHtml(wv->property("html").toString()));
    QCOMPARE(wv->property("preferredWidth").toInt(), 0);
    QCOMPARE(wv->property("preferredHeight").toInt(), 0);
    QCOMPARE(wv->property("url").toUrl(), QUrl::fromLocalFile(SRCDIR "/data/basic.html"));
    QCOMPARE(wv->property("status").toInt(), 1 /*QDeclarativeWebView::Ready*/);
    QVERIFY(qvariant_cast<QAction*>(wv->property("reload")));
    QVERIFY(qvariant_cast<QAction*>(wv->property("reload"))->isEnabled());
    QVERIFY(qvariant_cast<QAction*>(wv->property("back")));
    QVERIFY(!qvariant_cast<QAction*>(wv->property("back"))->isEnabled());
    QVERIFY(qvariant_cast<QAction*>(wv->property("forward")));
    QVERIFY(!qvariant_cast<QAction*>(wv->property("forward"))->isEnabled());
    QVERIFY(qvariant_cast<QAction*>(wv->property("stop")));
    QVERIFY(!qvariant_cast<QAction*>(wv->property("stop"))->isEnabled());

    wv->setProperty("pixelCacheSize", 0); // mainly testing that it doesn't crash or anything!
    QCOMPARE(wv->property("pixelCacheSize").toInt(),0);
    qvariant_cast<QAction*>(wv->property("reload"))->trigger();
    QTRY_COMPARE(wv->property("progress").toDouble(), 1.0);
}

void tst_qdeclarativewebview::settings()
{
    QDeclarativeComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/basic.qml"));
    checkNoErrors(component);
    QObject *wv = component.create();
    QVERIFY(wv != 0);
    QTRY_COMPARE(wv->property("progress").toDouble(), 1.0);

    QObject *s = QDeclarativeProperty(wv,"settings").object();
    QVERIFY(s != 0);

    // merely tests that setting gets stored (in QWebSettings)
    // behavioural tests are in WebKit.
    for (int b=0; b<=1; ++b) {
        bool on = !!b;

        s->setProperty("autoLoadImages", on);
        s->setProperty("developerExtrasEnabled", on);
        s->setProperty("javaEnabled", on);
        s->setProperty("javascriptCanAccessClipboard", on);
        s->setProperty("javascriptCanOpenWindows", on);
        s->setProperty("javascriptEnabled", on);
        s->setProperty("linksIncludedInFocusChain", on);
        s->setProperty("localContentCanAccessRemoteUrls", on);
        s->setProperty("localStorageDatabaseEnabled", on);
        s->setProperty("offlineStorageDatabaseEnabled", on);
        s->setProperty("offlineWebApplicationCacheEnabled", on);
        s->setProperty("pluginsEnabled", on);
        s->setProperty("printElementBackgrounds", on);
        s->setProperty("privateBrowsingEnabled", on);
        s->setProperty("zoomTextOnly", on);

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

void tst_qdeclarativewebview::historyNav()
{
    QDeclarativeComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/basic.qml"));
    checkNoErrors(component);
    QWebSettings::enablePersistentStorage(tmpDir());

    QObject *wv = component.create();
    QVERIFY(wv != 0);
    for (int i=1; i<=2; ++i) {
        QTRY_COMPARE(wv->property("progress").toDouble(), 1.0);
        QCOMPARE(wv->property("title").toString(),QString("Basic"));
        QTRY_COMPARE(qvariant_cast<QPixmap>(wv->property("icon")).width(), 48);
        QCOMPARE(qvariant_cast<QPixmap>(wv->property("icon")),QPixmap(SRCDIR "/data/basic.png"));
        QCOMPARE(wv->property("statusText").toString(),QString("status here"));
        QCOMPARE(strippedHtml(fileContents(SRCDIR "/data/basic.html")), strippedHtml(wv->property("html").toString()));
        QCOMPARE(wv->property("preferredWidth").toDouble(), 0.0);
        QCOMPARE(wv->property("url").toUrl(), QUrl::fromLocalFile(SRCDIR "/data/basic.html"));
        QCOMPARE(wv->property("status").toInt(), 1 /*QDeclarativeWebView::Ready*/);
        QVERIFY(qvariant_cast<QAction*>(wv->property("reload")));
        QVERIFY(qvariant_cast<QAction*>(wv->property("reload"))->isEnabled());
        QVERIFY(qvariant_cast<QAction*>(wv->property("back")));
        QVERIFY(!qvariant_cast<QAction*>(wv->property("back"))->isEnabled());
        QVERIFY(qvariant_cast<QAction*>(wv->property("forward")));
        QVERIFY(!qvariant_cast<QAction*>(wv->property("forward"))->isEnabled());
        QVERIFY(qvariant_cast<QAction*>(wv->property("stop")));
        QVERIFY(!qvariant_cast<QAction*>(wv->property("stop"))->isEnabled());

        qvariant_cast<QAction*>(wv->property("reload"))->trigger();
    }

    wv->setProperty("url", QUrl::fromLocalFile(SRCDIR "/data/forward.html"));
    QTRY_COMPARE(wv->property("progress").toDouble(), 1.0);
    QCOMPARE(wv->property("title").toString(),QString("Forward"));
    QTRY_COMPARE(qvariant_cast<QPixmap>(wv->property("icon")).width(), 32);
    QCOMPARE(qvariant_cast<QPixmap>(wv->property("icon")),QPixmap(SRCDIR "/data/forward.png"));
    QCOMPARE(strippedHtml(fileContents(SRCDIR "/data/forward.html")), strippedHtml(wv->property("html").toString()));
    QCOMPARE(wv->property("url").toUrl(), QUrl::fromLocalFile(SRCDIR "/data/forward.html"));
    QCOMPARE(wv->property("status").toInt(), 1 /*QDeclarativeWebView::Ready*/);
    QCOMPARE(wv->property("statusText").toString(),QString(""));
    QVERIFY(qvariant_cast<QAction*>(wv->property("reload")));
    QVERIFY(qvariant_cast<QAction*>(wv->property("reload"))->isEnabled());
    QVERIFY(qvariant_cast<QAction*>(wv->property("back")));
    QVERIFY(qvariant_cast<QAction*>(wv->property("back"))->isEnabled());
    QVERIFY(qvariant_cast<QAction*>(wv->property("forward")));
    QVERIFY(!qvariant_cast<QAction*>(wv->property("forward"))->isEnabled());
    QVERIFY(qvariant_cast<QAction*>(wv->property("stop")));
    QVERIFY(!qvariant_cast<QAction*>(wv->property("stop"))->isEnabled());

    qvariant_cast<QAction*>(wv->property("back"))->trigger();

    QTRY_COMPARE(wv->property("progress").toDouble(), 1.0);
    QCOMPARE(wv->property("title").toString(),QString("Basic"));
    QCOMPARE(strippedHtml(fileContents(SRCDIR "/data/basic.html")), strippedHtml(wv->property("html").toString()));
    QCOMPARE(wv->property("url").toUrl(), QUrl::fromLocalFile(SRCDIR "/data/basic.html"));
    QCOMPARE(wv->property("status").toInt(), 1 /*QDeclarativeWebView::Ready*/);
    QVERIFY(qvariant_cast<QAction*>(wv->property("reload")));
    QVERIFY(qvariant_cast<QAction*>(wv->property("reload"))->isEnabled());
    QVERIFY(qvariant_cast<QAction*>(wv->property("back")));
    QVERIFY(!qvariant_cast<QAction*>(wv->property("back"))->isEnabled());
    QVERIFY(qvariant_cast<QAction*>(wv->property("forward")));
    QVERIFY(qvariant_cast<QAction*>(wv->property("forward"))->isEnabled());
    QVERIFY(qvariant_cast<QAction*>(wv->property("stop")));
    QVERIFY(!qvariant_cast<QAction*>(wv->property("stop"))->isEnabled());
}

void tst_qdeclarativewebview::multipleWindows()
{
    QDeclarativeComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/newwindows.qml"));
    checkNoErrors(component);

    QDeclarativeGrid *grid = qobject_cast<QDeclarativeGrid*>(component.create());
    QVERIFY(grid != 0);
    QTRY_COMPARE(grid->children().count(), 2+4); // Component, Loader (with 1 WebView), 4 new-window WebViews
    QDeclarativeItem* popup = qobject_cast<QDeclarativeItem*>(grid->children().at(2)); // first popup after Component and Loader.
    QVERIFY(popup != 0);
    QTRY_COMPARE(popup->x(), 150.0);
}

void tst_qdeclarativewebview::loadError()
{
    QDeclarativeComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/loadError.qml"));
    checkNoErrors(component);
    QWebSettings::enablePersistentStorage(tmpDir());

    QObject *wv = component.create();
    QVERIFY(wv != 0);
    for (int i=1; i<=2; ++i) {
        QTRY_COMPARE(wv->property("progress").toDouble(), 1.0);
        QCOMPARE(wv->property("title").toString(),QString(""));
        QCOMPARE(wv->property("statusText").toString(),QString("")); // HTML 'status bar' text, not error message
        QCOMPARE(wv->property("url").toUrl(), QUrl::fromLocalFile(SRCDIR "/data/does-not-exist.html")); // Unlike QWebPage, which loses url
        QCOMPARE(wv->property("status").toInt(), 3 /*QDeclarativeWebView::Error*/);

        qvariant_cast<QAction*>(wv->property("reload"))->trigger();
    }
}

void tst_qdeclarativewebview::setHtml()
{
    QDeclarativeComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/sethtml.qml"));
    checkNoErrors(component);
    QObject *wv = component.create();
    QVERIFY(wv != 0);
    QCOMPARE(wv->property("html").toString(),QString("<html><head></head><body><p>This is a <b>string</b> set on the WebView</p></body></html>"));

    QSignalSpy spy(wv, SIGNAL(htmlChanged()));
    wv->setProperty("html", QString("<html><head><title>Basic</title></head><body><p>text</p></body></html>"));
    QCOMPARE(spy.count(),1);
}

void tst_qdeclarativewebview::elementAreaAt()
{
    QDeclarativeComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/elements.qml"));
    checkNoErrors(component);
    QObject *wv = component.create();
    QVERIFY(wv != 0);
    QTRY_COMPARE(wv->property("progress").toDouble(), 1.0);

    /* not now it's a plugin...
    QCOMPARE(wv->elementAreaAt(40,30,100,100),QRect(1,1,75,54)); // Area A in data/elements.html
    QCOMPARE(wv->elementAreaAt(130,30,200,100),QRect(78,3,110,50)); // Area B
    QCOMPARE(wv->elementAreaAt(40,30,400,400),QRect(0,0,310,100)); // Whole view
    QCOMPARE(wv->elementAreaAt(130,30,280,280),QRect(76,1,223,54)); // Area BC
    QCOMPARE(wv->elementAreaAt(130,30,400,400),QRect(0,0,310,100)); // Whole view
    */
}

void tst_qdeclarativewebview::javaScript()
{
    QDeclarativeComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/javaScript.qml"));
    checkNoErrors(component);
    QObject *wv = component.create();
    QVERIFY(wv != 0);
    QTRY_COMPARE(wv->property("progress").toDouble(), 1.0);
    /* not now it's a plugin...
    QCOMPARE(wv->evaluateJavaScript("123").toInt(), 123);
    QCOMPARE(wv->evaluateJavaScript("window.status").toString(), QString("status here"));
    QCOMPARE(wv->evaluateJavaScript("window.myjsname.qmlprop").toString(), QString("qmlvalue"));
    */
}

/*
Cannot be done now that webkit is a plugin

void tst_qdeclarativewebview::pixelCache()
{

    QDeclarativeComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/pixelCache.qml"));
    checkNoErrors(component);
    MyWebView *wv = qobject_cast<MyWebView*>(component.create());
    QVERIFY(wv != 0);
    QTRY_COMPARE(wv->property("progress"), 1.0);
    QPixmap pm(150,150);
    QPainter p(&pm);
    wv->paint(&p,0,0);
    const int expected = 120*(150+128); // 120 = width of HTML page, 150=pixmap height, 128=cache extra area
    QCOMPARE(wv->property("pixelsPainted"), expected);
    wv->paint(&p,0,0);
    QCOMPARE(wv->property("pixelsPainted"), expected); // nothing new needed to be painted
    wv->setProperty("pixelCacheSize", 0); // clears the cache
    wv->paint(&p,0,0);
    QCOMPARE(wv->property("pixelsPainted"), expected*2); // everything needed to be painted
    // Note that painted things always go into the cache (even if they don't "fit"),
    // just that they will be removed if anything else needs to be painted.
    wv->setProperty("pixelCacheSize", expected); // won't clear the cache
    wv->paint(&p,0,0);
    QCOMPARE(wv->property("pixelsPainted"), expected*2); // still there
    wv->setProperty("pixelCacheSize", expected-1); // too small - will clear the cache
    wv->paint(&p,0,0);
    QCOMPARE(wv->property("pixelsPainted"), expected*3); // repainted
}
*/

void tst_qdeclarativewebview::newWindowParent()
{
    QDeclarativeComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/propertychanges.qml"));
    checkNoErrors(component);
    QDeclarativeItem *rootItem = qobject_cast<QDeclarativeItem*>(component.create());
    QObject *wv = rootItem->findChild<QObject*>("webView");
    QVERIFY(rootItem != 0);
    QVERIFY(wv != 0);
    QTRY_COMPARE(wv->property("progress").toDouble(), 1.0);

    QDeclarativeItem* oldWindowParent = rootItem->findChild<QDeclarativeItem*>("oldWindowParent");
    QCOMPARE(qvariant_cast<QDeclarativeItem*>(wv->property("newWindowParent")), oldWindowParent);
    QSignalSpy newWindowParentSpy(wv, SIGNAL(newWindowParentChanged()));

    QDeclarativeItem* newWindowParent = rootItem->findChild<QDeclarativeItem*>("newWindowParent");
    wv->setProperty("newWindowParent", QVariant::fromValue(newWindowParent));
    QVERIFY(newWindowParent);
    QVERIFY(oldWindowParent);
    QVERIFY(oldWindowParent->childItems().count() == 0);
    QCOMPARE(wv->property("newWindowParent"), QVariant::fromValue(newWindowParent));
    QCOMPARE(newWindowParentSpy.count(),1);

    wv->setProperty("newWindowParent", QVariant::fromValue(newWindowParent));
    QCOMPARE(newWindowParentSpy.count(),1);

    wv->setProperty("newWindowParent", QVariant::fromValue((QDeclarativeItem*)0));
    QCOMPARE(newWindowParentSpy.count(),2);
}

void tst_qdeclarativewebview::newWindowComponent()
{
    QDeclarativeComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/propertychanges.qml"));
    checkNoErrors(component);
    QDeclarativeItem *rootItem = qobject_cast<QDeclarativeItem*>(component.create());
    QObject *wv = rootItem->findChild<QObject*>("webView");
    QVERIFY(rootItem != 0);
    QVERIFY(wv != 0);
    QTRY_COMPARE(wv->property("progress").toDouble(), 1.0);

    QDeclarativeComponent substituteComponent(&engine);
    substituteComponent.setData("import Qt 4.7; WebView { objectName: 'newWebView'; url: 'basic.html'; }", QUrl::fromLocalFile(""));
    QSignalSpy newWindowComponentSpy(wv, SIGNAL(newWindowComponentChanged()));

    wv->setProperty("newWindowComponent", QVariant::fromValue(&substituteComponent));
    QCOMPARE(wv->property("newWindowComponent"), QVariant::fromValue(&substituteComponent));
    QCOMPARE(newWindowComponentSpy.count(),1);

    wv->setProperty("newWindowComponent", QVariant::fromValue(&substituteComponent));
    QCOMPARE(newWindowComponentSpy.count(),1);

    wv->setProperty("newWindowComponent", QVariant::fromValue((QDeclarativeComponent*)0));
    QCOMPARE(newWindowComponentSpy.count(),2);
}

void tst_qdeclarativewebview::renderingEnabled()
{
    QDeclarativeComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/propertychanges.qml"));
    checkNoErrors(component);
    QDeclarativeItem *rootItem = qobject_cast<QDeclarativeItem*>(component.create());
    QObject *wv = rootItem->findChild<QObject*>("webView");
    QVERIFY(rootItem != 0);
    QVERIFY(wv != 0);
    QTRY_COMPARE(wv->property("progress").toDouble(), 1.0);

    QVERIFY(wv->property("renderingEnabled").toBool());
    QSignalSpy renderingEnabledSpy(wv, SIGNAL(renderingEnabledChanged()));

    wv->setProperty("renderingEnabled", false);
    QVERIFY(!wv->property("renderingEnabled").toBool());
    QCOMPARE(renderingEnabledSpy.count(),1);

    wv->setProperty("renderingEnabled", false);
    QCOMPARE(renderingEnabledSpy.count(),1);

    wv->setProperty("renderingEnabled", true);
    QCOMPARE(renderingEnabledSpy.count(),2);
}

void tst_qdeclarativewebview::pressGrabTime()
{
    QDeclarativeComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/propertychanges.qml"));
    checkNoErrors(component);
    QDeclarativeItem *rootItem = qobject_cast<QDeclarativeItem*>(component.create());
    QObject *wv = rootItem->findChild<QObject*>("webView");
    QVERIFY(rootItem != 0);
    QVERIFY(wv != 0);
    QTRY_COMPARE(wv->property("progress").toDouble(), 1.0);
    QCOMPARE(wv->property("pressGrabTime").toInt(), 200);
    QSignalSpy pressGrabTimeSpy(wv, SIGNAL(pressGrabTimeChanged()));

    wv->setProperty("pressGrabTime", 100);
    QCOMPARE(wv->property("pressGrabTime").toInt(), 100);
    QCOMPARE(pressGrabTimeSpy.count(),1);

    wv->setProperty("pressGrabTime", 100);
    QCOMPARE(pressGrabTimeSpy.count(),1);

    wv->setProperty("pressGrabTime", 0);
    QCOMPARE(pressGrabTimeSpy.count(),2);
}

QTEST_MAIN(tst_qdeclarativewebview)

#include "tst_qdeclarativewebview.moc"
