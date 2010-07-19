#include <private/qwindowsurface_raster_p.h>
#include <private/qpixmapdata_p.h>

class QBlittable;
class QBlittablePixmapData;

class Q_GUI_EXPORT QRasterBlittableWindowSurface : public QRasterWindowSurface
{
public:
    QRasterBlittableWindowSurface(QWidget *widget);

    virtual QBlittable *createBlittable(QImage *rasterSurface) = 0;

    QPaintDevice *paintDevice();

private:
        QImage *m_currentImage;
        QBlittable *m_blittable;
        QBlittablePixmapData *m_pmData;
        QPixmap *m_pixmap;
};
