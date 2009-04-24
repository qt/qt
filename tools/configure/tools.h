#ifndef _TOOLS_H_
#define _TOOLS_H_

#include <QString>
#include <QStringList>
#include <QMap>


class Tools
{
public:
    static void checkLicense(QMap<QString,QString> &dictionary, QMap<QString,QString> &licenseInfo,
                             const QString &path);
};

#endif // _TOOLS_H_

