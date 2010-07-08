#include "eglnullwsscreenplugin.h"
#include "eglnullwsscreen.h"

#include <QScreenDriverPlugin>
#include <QStringList>

class EGLNullWSScreenPlugin : public QScreenDriverPlugin
{
public:
    virtual QStringList keys() const;
    virtual QScreen *create(const QString& driver, int displayId);
};

QStringList EGLNullWSScreenPlugin::keys() const
{
    return QStringList() << QLatin1String(PluginName);
}

QScreen *EGLNullWSScreenPlugin::create(const QString& driver, int displayId)
{
    return (driver.toLower() == QLatin1String(PluginName) ?
        new EGLNullWSScreen(displayId) : 0);
}

Q_EXPORT_PLUGIN2(qeglnullws, EGLNullWSScreenPlugin)
