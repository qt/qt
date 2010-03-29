#include <qimage.h>

#include <private/qblittable_p.h>


class QImageBlittable : public QBlittable
{
public:
    QImageBlittable(QImage *image, bool deleteImage)
        : QBlittable(image->size(), QBlittable::Capabilities(QBlittable::SolidRectCapability
                    |QBlittable::SourcePixmapCapability
                    |QBlittable::SourceOverPixmapCapability
                    |QBlittable::SourceOverScaledPixmapCapability)),
            m_image(image),  m_deleteImage(deleteImage)
    {

    }

    ~QImageBlittable() {
        if (m_deleteImage)
            delete m_image;
    }

    void fillRect(const QRectF &rect, const QColor &color)
    {
        QPainter p(lock());
        p.fillRect(rect,color);
    }
    void drawPixmap(const QRectF &rect, const QPixmap &pixmap, const QRectF &source)
    {
        //here it is possible to do a pixmap.pixmapData()->buffer()
        //but is like this to show how to get the the blitter
        QPixmapData *data = pixmap.pixmapData();
        Q_ASSERT(data->width() && data->height());
        Q_ASSERT(data->classId() == QPixmapData::BlitterClass);
        QBlittablePixmapData *blittableData = static_cast<QBlittablePixmapData*>(data);

        QPainter p(lock());
        p.drawImage(rect, *blittableData->blittable()->lock(),source);
    }

protected:
    QImage *doLock() { return m_image; }
    void doUnlock() { }
private:
    QImage *m_image;
    bool m_deleteImage;
};
