#ifndef VARYINGVECTOR_H
#define VARYINGVECTOR_H

#include <QObject>
#include <QPointF>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)


class VaryingVector : public QObject
{
    Q_OBJECT
public:
    explicit VaryingVector(QObject *parent = 0);

    virtual const QPointF &sample(const QPointF &from);
signals:

public slots:

protected:
    QPointF m_ret;
};

QT_END_NAMESPACE
#endif // VARYINGVECTOR_H
