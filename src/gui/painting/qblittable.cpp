#include "qblittable_p.h"

#ifndef QT_NO_BLITTABLE
QT_BEGIN_NAMESPACE

class QBlittablePrivate
{
public:
    QBlittablePrivate(const QSize &size, QBlittable::Capabilities caps)
        : caps(caps), m_size(size), locked(false), cachedImg(0)
    {}
    QBlittable::Capabilities caps;
    QSize m_size;
    bool locked;
    QImage *cachedImg;
};


QBlittable::QBlittable(const QSize &size, Capabilities caps)
    : d_ptr(new QBlittablePrivate(size,caps))
{
}

QBlittable::~QBlittable()
{
    delete d_ptr;
}


QBlittable::Capabilities QBlittable::capabilities() const
{
    Q_D(const QBlittable);
    return d->caps;
}

QSize QBlittable::size() const
{
    Q_D(const QBlittable);
    return d->m_size;
}

QImage *QBlittable::lock()
{
    Q_D(QBlittable);
    if (!d->locked) {
        d->cachedImg = doLock();
        d->locked = true;
    }

    return d->cachedImg;
}

void QBlittable::unlock()
{
    Q_D(QBlittable);
    if (d->locked) {
        doUnlock();
        d->locked = false;
    }
}

QT_END_NAMESPACE
#endif //QT_NO_BLITTABLE

