#ifndef TESTTYPES_H
#define TESTTYPES_H

#include <QtCore/qobject.h>
#include <QtDeclarative/qml.h>

class MyQmlObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int result READ result WRITE setResult);
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged);
    Q_PROPERTY(MyQmlObject *object READ object WRITE setObject NOTIFY objectChanged);
    Q_PROPERTY(QmlList<QObject *> *data READ data);
    Q_CLASSINFO("DefaultProperty", "data");
public:
    MyQmlObject() : m_result(0), m_value(0), m_object(0) {}

    int result() const { return m_result; }
    void setResult(int r) { m_result = r; }

    int value() const { return m_value; }
    void setValue(int v) { m_value = v; emit valueChanged(); }

    QmlList<QObject *> *data() { return &m_data; }

    MyQmlObject *object() const { return m_object; }
    void setObject(MyQmlObject *o) { m_object = o; emit objectChanged(); }

signals:
    void valueChanged();
    void objectChanged();

private:
    QmlConcreteList<QObject *> m_data;
    int m_result;
    int m_value;
    MyQmlObject *m_object;
};
QML_DECLARE_TYPE(MyQmlObject);

#endif // TESTTYPES_H
