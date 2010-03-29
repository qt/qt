#include "qblittable_image.h"

#include <private/qpixmap_blitter_p.h>
#include <private/qwindowsurface_qws_p.h>

class QBlittablePixmapData;

class Q_GUI_EXPORT QWSBlittableMemSurface : public QWSLocalMemSurface
{
public:
    QWSBlittableMemSurface( QWidget *window )
        : QWSLocalMemSurface(window),
        m_currentPaintDevice(0),
        pmData(new QBlittablePixmapData(QPixmapData::PixmapType)),
        pm(pmData)
    {
    }

    QPaintDevice *paintDevice()
    {
        if (QWSLocalMemSurface::paintDevice() != m_currentPaintDevice) {
            QPaintDevice *device = QWSLocalMemSurface::paintDevice();
            if (device->devType() == QInternal::Image) {
                img = *static_cast<QImage *>(device);
                pmData->setBlittable(new QImageBlittable(&img,false));
            }
        }
        return &pm;
    }

private:
    QPaintDevice *m_currentPaintDevice;
    QBlittablePixmapData *pmData;
    QPixmap pm;
};

