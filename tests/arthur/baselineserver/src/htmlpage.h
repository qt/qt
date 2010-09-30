#ifndef HTMLPAGE_H
#define HTMLPAGE_H

#include "baselineprotocol.h"
#include <QFile>
#include <QTextStream>

class HTMLPage
{
public:
    HTMLPage();
    ~HTMLPage();

    void start(const QString &storagePath, const QString &runId, const PlatformInfo pinfo);
    void addItem(const QString &baseline, const QString &rendered, const ImageItem &item);
    void end();
    QString filePath();

    static void handleCGIQuery(const QString &query);

private:
    void writeHeader(const ImageItem &item);
    void writeFooter();
    QString generateCompared(const QString &baseline, const QString &rendered, bool fuzzy = false);

    QString root;
    QString path;
    QFile file;
    QTextStream out;
    QString id;
    PlatformInfo plat;
    bool headerWritten;
};

#endif // HTMLPAGE_H
