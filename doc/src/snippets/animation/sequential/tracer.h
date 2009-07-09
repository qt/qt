#ifndef TRACER_H
#define TRACER_H

#include <QObject>
#include <QVariant>

class Tracer : public QObject
{
    Q_OBJECT

public:
    Tracer(QObject *parent = 0);

public slots:
    void checkValue();
    void recordValue(const QVariant &value);

private:
    QVariant value;
    int time;
};

#endif
