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
    out << "<h3>Host: " << plat.hostName << " [" << address << "] OS: " << plat.osName << " [enum: " << plat.osVersion << "]</h3>\n";
    out << "<h3>Qt version: " << plat.qtVersion << " [commit: " << plat.gitCommit << "] Build key: \"" << plat.buildKey << "\"</h3>\n";
    out << "<h3>Engine: " << item.engineAsString() << " Format: " << item.formatAsString() << "</h3>\n\n";
    out << "<h3><a href=\"/cgi-bin/server.cgi?cmd=updateAllBaselines&id="<< id << "&host=" << plat.hostName
        << "&engine=" << item.engineAsString() << "&format=" << item.formatAsString()
        << "&url=" << pageUrl
        << "\">Update all baselines</a><br>";
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
           "<a href=\"/cgi-bin/server.cgi?cmd=blacklist&scriptName=" << item.scriptName
           << "&host=" << plat.hostName << "&engine=" << item.engineAsString()
           << "&format=" << item.formatAsString()
           << "&url=" << pageUrl << "\">Blacklist test</a></td>\n";
    out << "<tr>\n\n";

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
                    << imageItems.at(i).scriptName << "&host=" << plat.hostName
                    << "&engine=" << imageItems.at(i).engineAsString()
                    << "&format=" << imageItems.at(i).formatAsString()
                    << "&url=" << pageUrl
                    << "\">Whitelist test</a>";
            } else {
                out << "<span style=\"color:green\">Test passed</span>";
            }
            out << "</td><tr>";
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
