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
    cookie.setDomain(".foo.tld");
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

    preset.clear();
    result.clear();
    QNetworkCookie finalCookie = cookie;
    cookie.setDomain("foo.tld");
    finalCookie.setDomain(".foo.tld");
    result += finalCookie;
    QTest::newRow("should-add-dot-prefix") << preset << cookie << "http://www.foo.tld" << result << true;

    result.clear();
    cookie.setDomain("");
    finalCookie.setDomain("www.foo.tld");
    result += finalCookie;
    QTest::newRow("should-set-default-domain") << preset << cookie << "http://www.foo.tld" << result << true;

    // security test:
    result.clear();
    preset.clear();
    cookie.setDomain("something.completely.different");
    QTest::newRow("security-domain-1") << preset << cookie << "http://www.foo.tld" << result << false;

    // we want the cookie to be accepted although the path does not match, see QTBUG-5815
    cookie.setDomain(".foo.tld");
    cookie.setPath("/something");
    result += cookie;
    QTest::newRow("security-path-1") << preset << cookie << "http://www.foo.tld" << result << true;

    // setting the defaults:
    finalCookie = cookie;
    finalCookie.setPath("/something/");
    finalCookie.setDomain("www.foo.tld");
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
    cookie.setDomain(".nokia.com");
    allCookies += cookie;

    QTest::newRow("no-match-1") << allCookies << "http://foo.bar/" << result;
    QTest::newRow("no-match-2") << allCookies << "http://foo.bar/web" << result;
    QTest::newRow("no-match-3") << allCookies << "http://foo.bar/web/wiki" << result;
    QTest::newRow("no-match-4") << allCookies << "http://nokia.com" << result;
    QTest::newRow("no-match-5") << allCookies << "http://qt.nokia.com" << result;
    QTest::newRow("no-match-6") << allCookies << "http://nokia.com/webinar" << result;
    QTest::newRow("no-match-7") << allCookies << "http://qt.nokia.com/webinar" << result;

    result = allCookies;
    QTest::newRow("match-1") << allCookies << "http://nokia.com/web" << result;
    QTest::newRow("match-2") << allCookies << "http://nokia.com/web/" << result;
    QTest::newRow("match-3") << allCookies << "http://nokia.com/web/content" << result;
    QTest::newRow("match-4") << allCookies << "http://qt.nokia.com/web" << result;
    QTest::newRow("match-4") << allCookies << "http://qt.nokia.com/web/" << result;
    QTest::newRow("match-6") << allCookies << "http://qt.nokia.com/web/content" << result;

    cookie.setPath("/web/wiki");
    allCookies += cookie;

    // exact same results as before:
    QTest::newRow("one-match-1") << allCookies << "http://nokia.com/web" << result;
    QTest::newRow("one-match-2") << allCookies << "http://nokia.com/web/" << result;
    QTest::newRow("one-match-3") << allCookies << "http://nokia.com/web/content" << result;
    QTest::newRow("one-match-4") << allCookies << "http://qt.nokia.com/web" << result;
    QTest::newRow("one-match-4") << allCookies << "http://qt.nokia.com/web/" << result;
    QTest::newRow("one-match-6") << allCookies << "http://qt.nokia.com/web/content" << result;

    result.prepend(cookie);     // longer path, it must match first
    QTest::newRow("two-matches-1") << allCookies << "http://nokia.com/web/wiki" << result;
    QTest::newRow("two-matches-2") << allCookies << "http://qt.nokia.com/web/wiki" << result;

    // invert the order;
    allCookies.clear();
    allCookies << result.at(1) << result.at(0);
    QTest::newRow("two-matches-3") << allCookies << "http://nokia.com/web/wiki" << result;
    QTest::newRow("two-matches-4") << allCookies << "http://qt.nokia.com/web/wiki" << result;

    // expired cookie
    allCookies.clear();
    cookie.setExpirationDate(QDateTime::fromString("09-Nov-1999", "dd-MMM-yyyy"));
    allCookies += cookie;
    result.clear();
    QTest::newRow("exp-match-1") << allCookies << "http://nokia.com/web" << result;
    QTest::newRow("exp-match-2") << allCookies << "http://nokia.com/web/" << result;
    QTest::newRow("exp-match-3") << allCookies << "http://nokia.com/web/content" << result;
    QTest::newRow("exp-match-4") << allCookies << "http://qt.nokia.com/web" << result;
    QTest::newRow("exp-match-4") << allCookies << "http://qt.nokia.com/web/" << result;
    QTest::newRow("exp-match-6") << allCookies << "http://qt.nokia.com/web/content" << result;

    // path matching
    allCookies.clear();
    QNetworkCookie anotherCookie;
    anotherCookie.setName("a");
    anotherCookie.setPath("/web");
    anotherCookie.setDomain(".nokia.com");
    allCookies += anotherCookie;
    result.clear();
    QTest::newRow("path-unmatch-1") << allCookies << "http://nokia.com/" << result;
    QTest::newRow("path-unmatch-2") << allCookies << "http://nokia.com/something/else" << result;
    result += anotherCookie;
    QTest::newRow("path-match-1") << allCookies << "http://nokia.com/web" << result;
    QTest::newRow("path-match-2") << allCookies << "http://nokia.com/web/" << result;
    QTest::newRow("path-match-3") << allCookies << "http://nokia.com/web/content" << result;

    // secure cookies
    allCookies.clear();
    result.clear();
    QNetworkCookie secureCookie;
    secureCookie.setName("a");
    secureCookie.setPath("/web");
    secureCookie.setDomain(".nokia.com");
    secureCookie.setSecure(true);
    allCookies += secureCookie;
    QTest::newRow("no-match-secure-1") << allCookies << "http://nokia.com/web" << result;
    QTest::newRow("no-match-secure-2") << allCookies << "http://qt.nokia.com/web" << result;
    result += secureCookie;
    QTest::newRow("match-secure-1") << allCookies << "https://nokia.com/web" << result;
    QTest::newRow("match-secure-2") << allCookies << "https://qt.nokia.com/web" << result;

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

