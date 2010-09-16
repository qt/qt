#include "htmlpage.h"
#include "baselineprotocol.h"
#include <QDir>
#include <QProcess>

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

void HTMLPage::start(const QString &storagepath, const QString &runId, const PlatformInfo pinfo)
{
    end();

    id = runId;
    plat = pinfo;
    root = storagepath;
    QString dir = root + QLS("reports/");
    QDir cwd;
    if (!cwd.exists(dir))
        cwd.mkpath(dir);
}


void HTMLPage::writeHeader(const ImageItem &item)
{
    path = QLS("reports/") + id + QLC('_') + item.engineAsString()
            + QLC('_') + item.formatAsString() + QLS(".html");
    file.setFileName(root + path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        qWarning() << "Failed to open report file" << file.fileName();
    out.setDevice(&file);

    out << "<html><body><h1>Lancelot results from run " << id << "</h1>\n\n";
    out << "<h3>Host: " << plat.hostname << "</h3>\n";
    out << "<h3>Qt version: " << plat.qtVersion << "</h3>\n";
    out << "<h3>Build key: " << plat.buildKey << "</h3>\n";
    out << "<h3>Engine: " << item.engineAsString() << "</h3>\n";
    out << "<h3>Format: " << item.formatAsString() << "</h3>\n\n";
    out << "<table border=\"2\">\n"
           "<tr>\n"
           "<td><b>Script</b></td>\n"
           "<td><b>Baseline</b></td>\n"
           "<td><b>Rendered</b></td>\n"
           "<td><b>Comparison</b></td>\n"
           "<td><b>Fuzzy Comparison</b></td>\n"
           "<td><b>Score</b></td>\n"
           "</b></tr>\n\n";
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
    QString fuzzy = generateCompared(baseline, rendered, true);

    out << "<tr>\n";
    out << "<td>" << item.scriptName << "</td>\n";
    QStringList images = QStringList() << baseline << rendered << compared << fuzzy;
    foreach(const QString& img, images)
        out << "<td><a href=\"/" << img << "\"><img src=\"/" << img << "\" width=240 height=240></a></td>\n";
    out << "<tr>\n\n";
}


void HTMLPage::end()
{
    if (file.isOpen()) {
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
