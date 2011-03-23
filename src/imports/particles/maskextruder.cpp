#include "maskextruder.h"
#include <QImage>
#include <QDebug>
QT_BEGIN_NAMESPACE
MaskExtruder::MaskExtruder(QObject *parent) :
    ParticleExtruder(parent)
  , m_lastWidth(-1)
  , m_lastHeight(-1)
{
}

QPointF MaskExtruder::extrude(const QRectF &r)
{
    ensureInitialized(r);
    if(!m_mask.count())
        return r.topLeft();
    const QPointF p = m_mask[rand() % m_mask.count()];
    //### Should random sub-pixel positioning be added?
    return p + r.topLeft();
}

void MaskExtruder::ensureInitialized(const QRectF &r)
{
    if(m_lastWidth == r.width() && m_lastHeight == r.height())
        return;
    m_lastWidth = r.width();
    m_lastHeight = r.height();

    m_mask.clear();
    if(m_source.isEmpty())
        return;

    QImage img(m_source.toLocalFile());
    img = img.createAlphaMask();
    img = img.convertToFormat(QImage::Format_Mono);//Else LSB, but I think that's easier
    img = img.scaled(r.size().toSize());//TODO: Do they need aspect ratio stuff? Or tiling?
    for(int i=0; i<r.width(); i++){
        for(int j=0; j<r.height(); j++){
            if(img.pixelIndex(i,j))//Direct bit manipulation is presumably more efficient
                m_mask << QPointF(i,j);
        }
    }
}
QT_END_NAMESPACE
