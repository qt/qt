#ifndef QDIRECTFBBLITTER_H
#define QDIRECTFBBLITTER_H

#include <private/qpaintengine_blitter_p.h>

#include <directfb/directfb.h>

class QDirectFbBlitter : public QBlittable
{
public:
    QDirectFbBlitter(const QRect &rect, IDirectFBSurface *surface = 0);
    virtual ~QDirectFbBlitter(){ }

    virtual void fillRect(const QRectF &rect, const QColor &color);
    virtual void drawPixmap(const QRectF &rect, const QPixmap &pixmap, const QRectF &subrect);

protected:
    virtual QImage *doLock();
    virtual void doUnlock();

    IDirectFBSurface *m_surface;
    QImage m_image;
};

#endif // QDIRECTFBBLITTER_H
