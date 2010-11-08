/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "htmlpage.h"
#include "baselineprotocol.h"
#include "baselineserver.h"
#include <QDir>
#include <QProcess>
#include <QUrl>

HTMLPage::HTMLPage()
    : headerWritten(false)
{
}

HTMLPage::~HTMLPage()
{
    end();
}

QString HTMLPage::filePath()
{
    return path;
}

void HTMLPage::start(const QString &storagepath, const QString &runId, const PlatformInfo pinfo, const QString &hostAddress, const ImageItemList &itemList)
{
    end();

    id = runId;
    plat = pinfo;
    address = hostAddress;
    root = storagepath;
    imageItems = itemList;
    QString dir = root + QLS("reports/");
    QDir cwd;
    if (!cwd.exists(dir))
        cwd.mkpath(dir);
}


void HTMLPage::writeHeader(const ImageItem &item)
{
    path = QLS("reports/") + id + QLC('_') + item.engineAsString()
            + QLC('_') + item.formatAsString() + QLS(".html");

    QString pageUrl = BaselineServer::baseUrl() + path;

    file.setFileName(root + path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        qWarning() << "Failed to open report file" << file.fileName();
    out.setDevice(&file);

    out << "<html><body><h1>Lancelot results from run " << id << "</h1>\n\n";
    out << "<h3>Platform Info:</h3>\n";
    out << "<table>\n";
    foreach (QString key, plat.keys())
        out << "<tr><td>" << key << "</td><td>" << plat.value(key) << "</td></tr>\n";
    out << "</table>\n";

#if 0
    out << "<h3><a href=\"/cgi-bin/server.cgi?cmd=updateAllBaselines&id="<< id << "&host=" << plat.hostName
        << "&engine=" << item.engineAsString() << "&format=" << item.formatAsString()
        << "&url=" << pageUrl
        << "\">Update all baselines</a><br>";
#endif
    out << "<table border=\"2\">\n"
           "<tr>\n"
           "<td><b>Script</b></td>\n"
           "<td><b>Baseline</b></td>\n"
           "<td><b>Rendered</b></td>\n"
           "<td><b>Comparison</b></td>\n"
           "<td><b>Info/Action</b></td>\n"
           "</b></tr><br>";
}


void HTMLPage::writeFooter()
{
    out << "</table>\n</body></html>\n";
}


void HTMLPage::addItem(const QString &baseline, const QString &rendered, const ImageItem &item)
{
    if (!headerWritten) {
        writeHeader(item);
        headerWritten = true;
    }
    QString compared = generateCompared(baseline, rendered);
    QString pageUrl = BaselineServer::baseUrl() + path;

    out << "<tr>\n";
    out << "<td>" << item.scriptName << "</td>\n";
    QStringList images = QStringList() << baseline << rendered << compared;
    foreach(const QString& img, images)
        out << "<td><a href=\"/" << img << "\"><img src=\"/" << img << "\" width=240 height=240></a></td>\n";
    out << "<td><a href=\"/cgi-bin/server.cgi?cmd=updateSingleBaseline&oldBaseline=" << baseline
        << "&newBaseline=" << rendered << "&url=" << pageUrl << "\">Update baseline</a><br>"
#if 0
           "<a href=\"/cgi-bin/server.cgi?cmd=blacklist&scriptName=" << item.scriptName
           << "&host=" << plat.hostName << "&engine=" << item.engineAsString()
           << "&format=" << item.formatAsString()
           << "&url=" << pageUrl << "\">Blacklist test</a>"
#endif
        << "</td>\n";
        out << "</tr>\n\n";

    QMutableVectorIterator<ImageItem> it(imageItems);
    while (it.hasNext()) {
        it.next();
        if (it.value().scriptName == item.scriptName) {
            it.remove();
            break;
        }
    }
}


void HTMLPage::end()
{
    if (file.isOpen()) {
        // Add the names of the scripts that passed the test, or were blacklisted
        QString pageUrl = BaselineServer::baseUrl() + path;
        for (int i=0; i<imageItems.count(); ++i) {
            out << "<tr><td>" << imageItems.at(i).scriptName << "</td><td>N/A</td><td>N/A</td><td>N/A</td><td>";
            if (imageItems.at(i).status == ImageItem::IgnoreItem) {
                out << "<span style=\"background-color:yellow\">Blacklisted</span><br>"
                       "<a href=\"/cgi-bin/server.cgi?cmd=whitelist&scriptName="
                    << imageItems.at(i).scriptName << "&host=" << plat.value(PI_HostName)
                    << "&engine=" << imageItems.at(i).engineAsString()
                    << "&format=" << imageItems.at(i).formatAsString()
                    << "&url=" << pageUrl
                    << "\">Whitelist test</a>";
            } else {
                out << "<span style=\"color:green\">Test passed</span>";
            }
            out << "</td></tr>\n";
        }

        writeFooter();
        out.flush();
        file.close();
        path.clear();
        headerWritten = false;
    }
}


QString HTMLPage::generateCompared(const QString &baseline, const QString &rendered, bool fuzzy)
{
    QString res = rendered;
    QFileInfo fi(res);
    res.chop(fi.suffix().length() + 1);
    res += QLS(fuzzy ? "_fuzzycompared.png" : "_compared.png");
    QStringList args;
    if (fuzzy)
        args << QLS("-fuzz") << QLS("5%");
    args << root+baseline << root+rendered << root+res;
    QProcess::execute(QLS("compare"), args);
    return res;
}


void HTMLPage::handleCGIQuery(const QString &query)
{
    QUrl cgiUrl(QLS("http://dummy/cgi-bin/dummy.cgi?") + query);
    QTextStream s(stdout);
    s << "Content-Type: text/html\r\n\r\n"
      << "<HTML>";
//      << "Contents of QUERY_STRING:<br>"
//      << "Full string = " << query << "<br>";

    QString command(cgiUrl.queryItemValue("cmd"));

    if (command == QLS("updateSingleBaseline")) {
        s << BaselineHandler::updateSingleBaseline(cgiUrl.queryItemValue(QLS("oldBaseline")),
                                                   cgiUrl.queryItemValue(QLS("newBaseline")));
    } else if (command == QLS("updateAllBaselines")) {
        s << BaselineHandler::updateAllBaselines(cgiUrl.queryItemValue(QLS("host")),
                                                 cgiUrl.queryItemValue(QLS("id")),
                                                 cgiUrl.queryItemValue(QLS("engine")),
                                                 cgiUrl.queryItemValue(QLS("format")));
    } else if (command == QLS("blacklist")) {
        // blacklist a test
        s << BaselineHandler::blacklistTest(cgiUrl.queryItemValue(QLS("scriptName")),
                                                  cgiUrl.queryItemValue(QLS("host")),
                                                  cgiUrl.queryItemValue(QLS("engine")),
                                                  cgiUrl.queryItemValue(QLS("format")));
    } else if (command == QLS("whitelist")) {
        // whitelist a test
        s << BaselineHandler::whitelistTest(cgiUrl.queryItemValue(QLS("scriptName")),
                                                    cgiUrl.queryItemValue(QLS("host")),
                                                    cgiUrl.queryItemValue(QLS("engine")),
                                                    cgiUrl.queryItemValue(QLS("format")));
    } else {
        s << "Unknown query:<br>" << query << "<br>";
    }
    s << "<p><a href=\"" << cgiUrl.queryItemValue(QLS("url")) << "\">Back to report</a>";
    s << "</HTML>";
}
