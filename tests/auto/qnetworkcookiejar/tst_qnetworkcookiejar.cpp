/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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


#include <QtTest/QtTest>
#include <QtNetwork/QNetworkCookieJar>

class tst_QNetworkCookieJar: public QObject
{
    Q_OBJECT

private slots:
    void getterSetter();
    void setCookiesFromUrl_data();
    void setCookiesFromUrl();
    void cookiesForUrl_data();
    void cookiesForUrl();
};

QT_BEGIN_NAMESPACE

namespace QTest {
    template<>
    char *toString(const QNetworkCookie &cookie)
    {
        return qstrdup(cookie.toRawForm());
    }

    template<>
    char *toString(const QList<QNetworkCookie> &list)
    {
        QString result = "QList(";
        bool first = true;
        foreach (QNetworkCookie cookie, list) {
            if (!first)
                result += ", ";
            first = false;
            result += QString::fromLatin1("QNetworkCookie(%1)").arg(QLatin1String(cookie.toRawForm()));
        }

        return qstrdup(result.append(')').toLocal8Bit());
    }
}

QT_END_NAMESPACE

class MyCookieJar: public QNetworkCookieJar
{
public:
    inline QList<QNetworkCookie> allCookies() const
        { return QNetworkCookieJar::allCookies(); }
    inline void setAllCookies(const QList<QNetworkCookie> &cookieList)
        { QNetworkCookieJar::setAllCookies(cookieList); }
};

void tst_QNetworkCookieJar::getterSetter()
{
    MyCookieJar jar;

    QVERIFY(jar.allCookies().isEmpty());

    QList<QNetworkCookie> list;
    QNetworkCookie cookie;
    cookie.setName("a");
    list << cookie;

    jar.setAllCookies(list);
    QCOMPARE(jar.allCookies(), list);
}

void tst_QNetworkCookieJar::setCookiesFromUrl_data()
{
    QTest::addColumn<QList<QNetworkCookie> >("preset");
    QTest::addColumn<QNetworkCookie>("newCookie");
    QTest::addColumn<QString>("referenceUrl");
    QTest::addColumn<QList<QNetworkCookie> >("expectedResult");
    QTest::addColumn<bool>("setCookies");

    QList<QNetworkCookie> preset;
    QList<QNetworkCookie> result;
    QNetworkCookie cookie;

    cookie.setName("a");
    cookie.setPath("/");
    cookie.setDomain("www.foo.tld");
    result += cookie;
    QTest::newRow("just-add") << preset << cookie << "http://www.foo.tld" << result << true;

    preset = result;
    QTest::newRow("replace-1") << preset << cookie << "http://www.foo.tld" << result << true;

    cookie.setValue("bc");
    result.clear();
    result += cookie;
    QTest::newRow("replace-2") << preset << cookie << "http://www.foo.tld" << result << true;

    preset = result;
    cookie.setName("d");
    result += cookie;
    QTest::newRow("append") << preset << cookie << "http://www.foo.tld" << result << true;

    cookie = preset.at(0);
    result = preset;
    cookie.setPath("/something");
    result += cookie;
    QTest::newRow("diff-path") << preset << cookie << "http://www.foo.tld/something" << result << true;

    preset.clear();
    preset += cookie;
    cookie.setPath("/");
    QTest::newRow("diff-path-order") << preset << cookie << "http://www.foo.tld" << result << true;

    // security test:
    result.clear();
    preset.clear();
    cookie.setDomain("something.completely.different");
    QTest::newRow("security-domain-1") << preset << cookie << "http://www.foo.tld" << result << false;

    cookie.setDomain("www.foo.tld");
    cookie.setPath("/something");
    QTest::newRow("security-path-1") << preset << cookie << "http://www.foo.tld" << result << false;

    // setting the defaults:
    QNetworkCookie finalCookie = cookie;
    finalCookie.setPath("/something/");
    cookie.setPath("");
    cookie.setDomain("");
    result.clear();
    result += finalCookie;
    QTest::newRow("defaults-1") << preset << cookie << "http://www.foo.tld/something/" << result << true;

    finalCookie.setPath("/");
    result.clear();
    result += finalCookie;
    QTest::newRow("defaults-2") << preset << cookie << "http://www.foo.tld" << result << true;

    // security test: do not accept cookie domains like ".com" nor ".com." (see RFC 2109 section 4.3.2)
    result.clear();
    preset.clear();
    cookie.setDomain(".com");
    QTest::newRow("rfc2109-4.3.2-ex3") << preset << cookie << "http://x.foo.com" << result << false;

    result.clear();
    preset.clear();
    cookie.setDomain(".com.");
    QTest::newRow("rfc2109-4.3.2-ex3-2") << preset << cookie << "http://x.foo.com" << result << false;
}

void tst_QNetworkCookieJar::setCookiesFromUrl()
{
    QFETCH(QList<QNetworkCookie>, preset);
    QFETCH(QNetworkCookie, newCookie);
    QFETCH(QString, referenceUrl);
    QFETCH(QList<QNetworkCookie>, expectedResult);
    QFETCH(bool, setCookies);

    QList<QNetworkCookie> cookieList;
    cookieList += newCookie;
    MyCookieJar jar;
    jar.setAllCookies(preset);
    QCOMPARE(jar.setCookiesFromUrl(cookieList, referenceUrl), setCookies);

    QList<QNetworkCookie> result = jar.allCookies();
    foreach (QNetworkCookie cookie, expectedResult) {
        QVERIFY2(result.contains(cookie), cookie.toRawForm());
        result.removeAll(cookie);
    }
    QVERIFY2(result.isEmpty(), QTest::toString(result));
}

void tst_QNetworkCookieJar::cookiesForUrl_data()
{
    QTest::addColumn<QList<QNetworkCookie> >("allCookies");
    QTest::addColumn<QString>("url");
    QTest::addColumn<QList<QNetworkCookie> >("expectedResult");

    QList<QNetworkCookie> allCookies;
    QList<QNetworkCookie> result;

    QTest::newRow("no-cookies") << allCookies << "http://foo.bar/" << result;

    QNetworkCookie cookie;
    cookie.setName("a");
    cookie.setPath("/web");
    cookie.setDomain(".trolltech.com");
    allCookies += cookie;

    QTest::newRow("no-match-1") << allCookies << "http://foo.bar/" << result;
    QTest::newRow("no-match-2") << allCookies << "http://foo.bar/web" << result;
    QTest::newRow("no-match-3") << allCookies << "http://foo.bar/web/wiki" << result;
    QTest::newRow("no-match-4") << allCookies << "http://trolltech.com" << result;
    QTest::newRow("no-match-5") << allCookies << "http://www.trolltech.com" << result;
    QTest::newRow("no-match-6") << allCookies << "http://trolltech.com/webinar" << result;
    QTest::newRow("no-match-7") << allCookies << "http://www.trolltech.com/webinar" << result;

    result = allCookies;
    QTest::newRow("match-1") << allCookies << "http://trolltech.com/web" << result;
    QTest::newRow("match-2") << allCookies << "http://trolltech.com/web/" << result;
    QTest::newRow("match-3") << allCookies << "http://trolltech.com/web/content" << result;
    QTest::newRow("match-4") << allCookies << "http://www.trolltech.com/web" << result;
    QTest::newRow("match-4") << allCookies << "http://www.trolltech.com/web/" << result;
    QTest::newRow("match-6") << allCookies << "http://www.trolltech.com/web/content" << result;

    cookie.setPath("/web/wiki");
    allCookies += cookie;

    // exact same results as before:
    QTest::newRow("one-match-1") << allCookies << "http://trolltech.com/web" << result;
    QTest::newRow("one-match-2") << allCookies << "http://trolltech.com/web/" << result;
    QTest::newRow("one-match-3") << allCookies << "http://trolltech.com/web/content" << result;
    QTest::newRow("one-match-4") << allCookies << "http://www.trolltech.com/web" << result;
    QTest::newRow("one-match-4") << allCookies << "http://www.trolltech.com/web/" << result;
    QTest::newRow("one-match-6") << allCookies << "http://www.trolltech.com/web/content" << result;

    result.prepend(cookie);     // longer path, it must match first
    QTest::newRow("two-matches-1") << allCookies << "http://trolltech.com/web/wiki" << result;
    QTest::newRow("two-matches-2") << allCookies << "http://www.trolltech.com/web/wiki" << result;

    // invert the order;
    allCookies.clear();
    allCookies << result.at(1) << result.at(0);
    QTest::newRow("two-matches-3") << allCookies << "http://trolltech.com/web/wiki" << result;
    QTest::newRow("two-matches-4") << allCookies << "http://www.trolltech.com/web/wiki" << result;

    // expired cookie
    allCookies.clear();
    cookie.setExpirationDate(QDateTime::fromString("09-Nov-1999", "dd-MMM-yyyy"));
    allCookies += cookie;
    result.clear();
    QTest::newRow("exp-match-1") << allCookies << "http://trolltech.com/web" << result;
    QTest::newRow("exp-match-2") << allCookies << "http://trolltech.com/web/" << result;
    QTest::newRow("exp-match-3") << allCookies << "http://trolltech.com/web/content" << result;
    QTest::newRow("exp-match-4") << allCookies << "http://www.trolltech.com/web" << result;
    QTest::newRow("exp-match-4") << allCookies << "http://www.trolltech.com/web/" << result;
    QTest::newRow("exp-match-6") << allCookies << "http://www.trolltech.com/web/content" << result;
}

void tst_QNetworkCookieJar::cookiesForUrl()
{
    QFETCH(QList<QNetworkCookie>, allCookies);
    QFETCH(QString, url);
    QFETCH(QList<QNetworkCookie>, expectedResult);

    MyCookieJar jar;
    jar.setAllCookies(allCookies);

    QList<QNetworkCookie> result = jar.cookiesForUrl(url);
    QCOMPARE(result, expectedResult);
}

QTEST_MAIN(tst_QNetworkCookieJar)
#include "tst_qnetworkcookiejar.moc"

