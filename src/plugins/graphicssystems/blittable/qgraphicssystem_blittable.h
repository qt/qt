#include <private/qgraphicssystem_p.h>
#include <private/qblittable_p.h>
#include <private/qpixmap_blitter_p.h>
#include <private/qpixmap_raster_p.h>
#include <private/qwindowsurface_rasterblittable_p.h>

#include "qblittable_image.h"

class QImageBlittableWindowSurface : public QRasterBlittableWindowSurface
{
public:
    QImageBlittableWindowSurface(QWidget *widget)
        : QRasterBlittableWindowSurface(widget)
    {}

    QBlittable *createBlittable(QImage *rasterSurface)
    {
        return new QImageBlittable(rasterSurface,false); 
    }

};

class QBlittableGraphicsSystem : public QGraphicsSystem
{
public:
    ~QBlittableGraphicsSystem() { }

    QPixmapData *createPixmapData(QPixmapData::PixelType type) const
    {
        if (type == QPixmapData::PixmapType)
            return new QBlittablePixmapData(type);
        else
            return new QRasterPixmapData(type);
    }

    QWindowSurface *createWindowSurface(QWidget *widget) const
    {
        return new QImageBlittableWindowSurface(widget);
    }

    QBlittable *createBlittable(const QSize &size) const
    {
        QImage *image = new QImage(size, QImage::Format_ARGB32_Premultiplied);
        return new QImageBlittable(image,true);
    }

    QList<QGraphicsSystemScreen *> screens()
    { return m_screens; }

    QList<QGraphicsSystemScreen *> m_screens;
};


