#ifndef QWAYLANDXCOMPOSITEBUFFER_H
#define QWAYLANDXCOMPOSITEBUFFER_H

#include "qwaylandbuffer.h"

#include "wayland-xcomposite-client-protocol.h"

class QWaylandXCompositeBuffer : public QWaylandBuffer
{
public:
    QWaylandXCompositeBuffer(struct wl_xcomposite *xcomposite,
                             uint32_t window,
                             const QSize &size,
                             struct wl_visual *visual);

    QSize size() const;
private:
    QSize mSize;
};

#endif // QWAYLANDXCOMPOSITEBUFFER_H
