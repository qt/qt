#ifndef QDIRECTFBBLITTER_H
#define QDIRECTFBBLITTER_H

#include "qdirectfbconvenience.h"

#include <private/qblittable_p.h>

#include <directfb.h>

class QDirectFbBlitter : public QBlittable
{
public:
    QDirectFbBlitter(const QSize &size, IDirectFBSurface *surface = 0);
    virtual ~QDirectFbBlitter();

    virtual void fillRect(const QRectF &rect, const QColor &color);
    virtual void drawPixmap(const QRectF &rect, const QPixmap &pixmap, const QRectF &subrect);

protected:
    virtual QImage *doLock();
    virtual void doUnlock();

    IDirectFBSurface *m_surface;
    QImage m_image;

    friend class QDirectFbConvenience;
};

class QDirectFbBlitterPixmapData : public QBlittablePixmapData
{
public:
    QBlittable *createBlittable(const QSize &size) const { return new QDirectFbBlitter(size); }
};

#endif // QDIRECTFBBLITTER_H
