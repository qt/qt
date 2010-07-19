#ifndef QDIRECTFBCURSOR_H
#define QDIRECTFBCURSOR_H

#include <QPlatformCursor>
#include <directfb.h>
class QDirectFbScreen;
class QDirectFbBlitter;

class QDirectFBCursor : public QPlatformCursor
{
public:
    QDirectFBCursor(QPlatformScreen *screem);
    void changeCursor(QCursor * cursor, QWidget * widget);

private:
    IDirectFBDisplayLayer * m_layer;
    IDirectFBSurface * surface;
    QPlatformCursorImage * image;
    QDirectFbBlitter *blitter;
};

#endif // QDIRECTFBCURSOR_H
