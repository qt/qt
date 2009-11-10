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
#include <QtWebKit/qwebpage.h>
#include <QtWebKit/qwebframe.h>
#include <QtCore/qdir.h>
#include <QtCore/qfile.h>

class tst_qmlgraphicswebview : public QObject
{
    Q_OBJECT
public:
    tst_qmlgraphicswebview() {}

private slots:
    void basicProperties();
    void historyNav();
    void loadError();
    void setHtml();
    void javaScript();
    void cleanupTestCase();

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
    Q_ASSERT(file.open(QIODevice::ReadOnly));
    return file.readAll();
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

QTEST_MAIN(tst_qmlgraphicswebview)

#include "tst_qmlgraphicswebview.moc"
