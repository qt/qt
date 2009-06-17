#include "testtypes.h"

class BaseExtensionObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int baseExtendedProperty READ extendedProperty WRITE setExtendedProperty NOTIFY extendedPropertyChanged);
public:
    BaseExtensionObject(QObject *parent) : QObject(parent), m_value(0) {}

    int extendedProperty() const { return m_value; }
    void setExtendedProperty(int v) { m_value = v; emit extendedPropertyChanged(); }

signals:
    void extendedPropertyChanged();
private:
    int m_value;
};

class ExtensionObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int extendedProperty READ extendedProperty WRITE setExtendedProperty NOTIFY extendedPropertyChanged);
public:
    ExtensionObject(QObject *parent) : QObject(parent), m_value(0) {}

    int extendedProperty() const { return m_value; }
    void setExtendedProperty(int v) { m_value = v; emit extendedPropertyChanged(); }

signals:
    void extendedPropertyChanged();
private:
    int m_value;
};

QML_DEFINE_TYPE(MyQmlObject,MyQmlObject);
QML_DEFINE_TYPE(MyDeferredObject,MyDeferredObject);
QML_DEFINE_TYPE(MyQmlContainer,MyQmlContainer);
QML_DEFINE_EXTENDED_TYPE(MyBaseExtendedObject,MyBaseExtendedObject,BaseExtensionObject);
QML_DEFINE_EXTENDED_TYPE(MyExtendedObject,MyExtendedObject,ExtensionObject);

#include "testtypes.moc"
