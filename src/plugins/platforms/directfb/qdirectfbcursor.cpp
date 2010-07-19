#include "qdirectfbcursor.h"
#include "qdirectfbconvenience.h"


QDirectFBCursor::QDirectFBCursor(QPlatformScreen* screen) :
        QPlatformCursor(screen), surface(0)
{
    QDirectFbConvenience::dfbInterface()->GetDisplayLayer(QDirectFbConvenience::dfbInterface(),DLID_PRIMARY, &m_layer);
    image = new QPlatformCursorImage(0, 0, 0, 0, 0, 0);
}

void QDirectFBCursor::changeCursor(QCursor * cursor, QWidget * widget)
{
    Q_UNUSED(widget);
    int xSpot;
    int ySpot;
    QPixmap map;

    if (cursor->shape() != Qt::BitmapCursor) {
        image->set(cursor->shape());
        xSpot = image->hotspot().x();
        ySpot = image->hotspot().y();
        QImage *i = image->image();
        map = QPixmap::fromImage(*i);
    } else {
        QPoint point = cursor->hotSpot();
        xSpot = point.x();
        ySpot = point.y();
        map = cursor->pixmap();
    }

    IDirectFBSurface *surface = QDirectFbConvenience::dfbSurfaceForPixmapData(map.pixmapData());

    if (m_layer->SetCooperativeLevel(m_layer, DLSCL_ADMINISTRATIVE) != DFB_OK) {
        return;
    }
    m_layer->SetCursorShape( m_layer, surface, xSpot, ySpot);
    m_layer->SetCooperativeLevel(m_layer, DLSCL_SHARED);
}
