#include <QtGui>

#ifndef _DUMMYOBJECT_H__

class DummyObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QRect rect READ rect WRITE setRect)
    Q_PROPERTY(float opacity READ opacity WRITE setOpacity)
public:
    DummyObject();
    QRect rect() const;
    void setRect(const QRect &r);
    float opacity() const;
    void setOpacity(float);

private:
    QRect m_rect;
    float m_opacity;
};


#endif