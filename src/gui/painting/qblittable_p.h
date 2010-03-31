#ifndef QBLITTABLE_P_H
#define QBLITTABLE_P_H

#include <QtCore/qsize.h>
#include <QtGui/qpixmap.h>

#ifndef QT_NO_BLITTABLE
QT_BEGIN_NAMESPACE

class QImage;
class QBlittablePrivate;

class Q_GUI_EXPORT QBlittable
{
    Q_DECLARE_PRIVATE(QBlittable);
public:
    enum Capability {

        SolidRectCapability             = 0x0001,
        SourcePixmapCapability          = 0x0002,
        SourceOverPixmapCapability      = 0x0004,
        SourceOverScaledPixmapCapability = 0x0008,

        // Internal ones
        OutlineCapability               = 0x0001000,
    };
    Q_DECLARE_FLAGS (Capabilities, Capability);

    QBlittable(const QSize &size, Capabilities caps);
    virtual ~QBlittable();

    Capabilities capabilities() const;
    QSize size() const;

    virtual void fillRect(const QRectF &rect, const QColor &color) = 0;
    virtual void drawPixmap(const QRectF &rect, const QPixmap &pixmap, const QRectF &subrect) = 0;

    QImage *lock();
    void unlock();

protected:
    virtual QImage *doLock() = 0;
    virtual void doUnlock() = 0;
    QBlittablePrivate *d_ptr;
};

QT_END_NAMESPACE
#endif //QT_NO_BLITTABLE
#endif //QBLITTABLE_P_H
