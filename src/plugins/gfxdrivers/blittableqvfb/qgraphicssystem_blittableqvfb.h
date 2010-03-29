#include <private/qgraphicssystem_p.h>
#include <private/qpixmap_blitter_p.h>
#include <private/qpixmap_raster_p.h>
#include <QScreen>

#include "qwindowsurface_qwsblittable.h"
#include "qblittable_image.h"
#include <qapplication.h>

class QBlittableVFbGraphicsSystem : public QGraphicsSystem
{
public:
        QPixmapData *createPixmapData(QPixmapData::PixelType type) const
        {
            if (type == QPixmapData::PixmapType) {
                return new QBlittablePixmapData(type);
            } else {
                return new QRasterPixmapData(type);
            }
        }

        QWindowSurface *createWindowSurface(QWidget *widget) const
        {
             if (QApplication::type() == QApplication::GuiServer)
                 return new QWSBlittableMemSurface(widget);
             else
                 return QScreen::instance()->createSurface(widget);
        }

        QBlittable *createBlittable(const QSize &size) const
        {
            QImage *image = new QImage(size,QImage::Format_ARGB32);
            return new QImageBlittable(image,true);
        }

        static QGraphicsSystem *instance()
        {
            static QGraphicsSystem *system = 0;
            if (!system) {
                system = new QBlittableVFbGraphicsSystem;
            }
            return system;
        }

private:
        QBlittableVFbGraphicsSystem() { }
};
