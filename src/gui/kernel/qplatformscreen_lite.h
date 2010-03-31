#ifndef QPLATFORMSCREEN_H
#define QPLATFORMSCREEN_H

#include <QtCore/qrect.h>
#include <QtGui/qimage.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class Q_GUI_EXPORT QPlatformScreen
{
public:
    virtual ~QPlatformScreen() { }

    virtual QRect geometry() const = 0;
    virtual QRect availableGeometry() const {return geometry();};
    virtual int depth() const = 0;
    virtual QImage::Format format() const = 0;
    virtual QSize physicalSize() const = 0;
    virtual void setDirty(const QRect &) {}
    virtual QWidget *topLevelAt(const QPoint &point) const;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QPLATFORMSCREEN_H
