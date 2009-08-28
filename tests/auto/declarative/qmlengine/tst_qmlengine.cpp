#include <qtest.h>
#include <QtDeclarative/qml.h>
#include <QtDeclarative/QmlComponent>
#include <QtDeclarative/QmlEngine>

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QUrl>

class tst_qmlengine : public QObject
{
    Q_OBJECT
public:
    tst_qmlengine() {}

private slots:
    void valueTypeFunctions();

private:
    QmlEngine engine;
};

class MyTypeObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QPoint pointProperty READ pointProperty WRITE setPointProperty);
    Q_PROPERTY(QPointF pointFProperty READ pointFProperty WRITE setPointFProperty);
    Q_PROPERTY(QSize sizeProperty READ sizeProperty WRITE setSizeProperty);
    Q_PROPERTY(QSizeF sizeFProperty READ sizeFProperty WRITE setSizeFProperty);
    Q_PROPERTY(QRect rectProperty READ rectProperty WRITE setRectProperty NOTIFY rectPropertyChanged);
    Q_PROPERTY(QRectF rectFProperty READ rectFProperty WRITE setRectFProperty);
    
public:
    MyTypeObject() {}

    QPoint pointPropertyValue;
    QPoint pointProperty() const {
       return pointPropertyValue;
    }
    void setPointProperty(const QPoint &v) {
        pointPropertyValue = v;
    }

    QPointF pointFPropertyValue;
    QPointF pointFProperty() const {
       return pointFPropertyValue;
    }
    void setPointFProperty(const QPointF &v) {
        pointFPropertyValue = v;
    }

    QSize sizePropertyValue;
    QSize sizeProperty() const {
       return sizePropertyValue;
    }
    void setSizeProperty(const QSize &v) {
        sizePropertyValue = v;
    }

    QSizeF sizeFPropertyValue;
    QSizeF sizeFProperty() const {
       return sizeFPropertyValue;
    }
    void setSizeFProperty(const QSizeF &v) {
        sizeFPropertyValue = v;
    }

    QRect rectPropertyValue;
    QRect rectProperty() const {
       return rectPropertyValue;
    }
    void setRectProperty(const QRect &v) {
        rectPropertyValue = v;
    }

    QRectF rectFPropertyValue;
    QRectF rectFProperty() const {
       return rectFPropertyValue;
    }
    void setRectFProperty(const QRectF &v) {
        rectFPropertyValue = v;
    }

};
QML_DECLARE_TYPE(MyTypeObject);
QML_DEFINE_TYPE(Test, 1, 0, 0, MyTypeObject, MyTypeObject);

void tst_qmlengine::valueTypeFunctions()
{
    QmlComponent component(&engine, SRCDIR "/functions.qml");
    MyTypeObject *obj = qobject_cast<MyTypeObject*>(component.create());
    QCOMPARE(obj->rectProperty(), QRect(0,0,100,100));
    QCOMPARE(obj->rectFProperty(), QRectF(0,0.5,100,99.5));
}

QTEST_MAIN(tst_qmlengine)

#include "tst_qmlengine.moc"
