#ifndef QDIRECTFBCURSOR_H
#define QDIRECTFBCURSOR_H

#include <QGraphicsSystemCursor>
#include <directfb.h>
class QDirectFbScreen;
class QDirectFbBlitter;

class QDirectFBCursor : public QGraphicsSystemCursor
{
public:
    QDirectFBCursor(QPlatformScreen *screem);
    void changeCursor(QCursor * cursor, QWidget * widget);

private:
    IDirectFBDisplayLayer * m_layer;
    IDirectFBSurface * surface;
    QGraphicsSystemCursorImage * image;
    QDirectFbBlitter *blitter;
};

#endif // QDIRECTFBCURSOR_H
