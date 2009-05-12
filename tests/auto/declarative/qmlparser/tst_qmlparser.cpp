#include <qtest.h>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <QtCore/qfile.h>
#include <QtCore/qdebug.h>

class MyInterface 
{
public:
    MyInterface() : id(913) {}
    int id;
};

Q_DECLARE_INTERFACE(MyInterface, "com.trolltech.Qt.Test.MyInterface");
QML_DECLARE_INTERFACE(MyInterface);
QML_DEFINE_INTERFACE(MyInterface);

class MyQmlObject : public QObject, public MyInterface
{
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue)
    Q_PROPERTY(QString readOnlyString READ readOnlyString)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled)
    Q_PROPERTY(QRect rect READ rect WRITE setRect)
    Q_PROPERTY(QMatrix matrix READ matrix WRITE setMatrix)  //assumed to be unsupported by QML
    Q_PROPERTY(MyInterface *interface READ interface WRITE setInterface)
    Q_INTERFACES(MyInterface)
public:
    MyQmlObject() : m_value(-1), m_interface(0) {}

    int value() const { return m_value; }
    void setValue(int v) { m_value = v; }

    QString readOnlyString() const { return QLatin1String(""); }

    bool enabled() const { return false; }
    void setEnabled(bool) {}

    QRect rect() const { return QRect(); }
    void setRect(const QRect&) {}

    QMatrix matrix() const { return QMatrix(); }
    void setMatrix(const QMatrix&) {}

    MyInterface *interface() const { return m_interface; }
    void setInterface(MyInterface *iface) { m_interface = iface; }

    Q_CLASSINFO("DefaultMethod", "basicSlot()");

public slots:
    void basicSlot() { qWarning("MyQmlObject::basicSlot"); }

signals:
    void basicSignal();

private:
    friend class tst_qmlparser;
    int m_value;
    MyInterface *m_interface;
};

QML_DECLARE_TYPE(MyQmlObject);
QML_DEFINE_TYPE(MyQmlObject,MyQmlObject);

class MyContainer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<QObject*>* children READ children)
    Q_PROPERTY(QList<MyInterface*>* qlistInterfaces READ qlistInterfaces)
    Q_PROPERTY(QmlList<MyInterface*>* qmllistInterfaces READ qmllistInterfaces)
    Q_CLASSINFO("DefaultProperty", "children");
public:
    MyContainer() {}

    QList<QObject*> *children() { return &m_children; }
    QList<MyInterface *> *qlistInterfaces() { return &m_interfaces; }
    QmlList<MyInterface *> *qmllistInterfaces() { return &m_qmlinterfaces; }
    const QmlConcreteList<MyInterface *> &qmllistAccessor() const { return m_qmlinterfaces; }

private:
    QList<QObject*> m_children;
    QList<MyInterface *> m_interfaces;
    QmlConcreteList<MyInterface *> m_qmlinterfaces;
};

QML_DECLARE_TYPE(MyContainer);
QML_DEFINE_TYPE(MyContainer,MyContainer);

class MyDotPropertyObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(MyQmlObject *obj READ obj)
    Q_PROPERTY(MyQmlObject *readWriteObj READ readWriteObj WRITE setReadWriteObj)
public:
    MyDotPropertyObject() : m_rwobj(0), m_ownRWObj(false) {}
    ~MyDotPropertyObject()
    {
        if (m_ownRWObj)
            delete m_rwobj;
    }

    MyQmlObject *obj() { return 0; }

    MyQmlObject *readWriteObj()
    {
        if (!m_rwobj) {
            m_rwobj = new MyQmlObject;
            m_ownRWObj = true;
        }
        return m_rwobj;
    }

    void setReadWriteObj(MyQmlObject *obj)
    {
        if (m_ownRWObj) {
            delete m_rwobj;
            m_ownRWObj = false;
        }

        m_rwobj = obj;
    }

private:
    MyQmlObject *m_rwobj;
    bool m_ownRWObj;
};

QML_DECLARE_TYPE(MyDotPropertyObject);
QML_DEFINE_TYPE(MyDotPropertyObject,MyDotPropertyObject);

class tst_qmlparser : public QObject
{
    Q_OBJECT
public:
    tst_qmlparser() {}

private slots:

    void errors_data();
    void errors();

    void simpleObject();
    void simpleContainer();
    void interfaceProperty();
    void interfaceQmlList();
    void interfaceQList();

    void assignObjectToSignal();

    // regression tests for crashes
    void crash1();

private:
    QmlEngine engine;
};

#define VERIFY_ERRORS(errorfile) \
    if (!errorfile) { \
        QVERIFY(!component.isError()); \
        QVERIFY(component.errors().isEmpty()); \
    } else { \
        QFile file(errorfile); \
        QVERIFY(file.open(QIODevice::ReadOnly)); \
        QByteArray data = file.readAll(); \
        QList<QByteArray> expected = data.split('\n'); \
        expected.removeAll(QByteArray("")); \
        QList<QmlError> errors = component.errors(); \
        QList<QByteArray> actual; \
        for (int ii = 0; ii < errors.count(); ++ii) { \
            const QmlError &error = errors.at(ii); \
            QByteArray errorStr = QByteArray::number(error.line()) + ":" +  \
                                  QByteArray::number(error.column()) + ":" + \
                                  error.description().toUtf8(); \
            actual << errorStr; \
        } \
        if (qgetenv("DEBUG") != "") \
            qWarning() << expected << actual;  \
        QCOMPARE(expected, actual); \
    }

#define TEST_FILE(filename) \
    QUrl::fromLocalFile(QApplication::applicationDirPath() + "/" + filename)

void tst_qmlparser::errors_data()
{
    QTest::addColumn<QString>("file");
    QTest::addColumn<QString>("errorFile");
    QTest::addColumn<bool>("create");

    QTest::newRow("nonExistantProperty.1") << "nonexistantProperty.1.txt" << "nonexistantProperty.1.errors.txt" << true;
    QTest::newRow("nonExistantProperty.2") << "nonexistantProperty.2.txt" << "nonexistantProperty.2.errors.txt" << true;
    QTest::newRow("nonExistantProperty.3") << "nonexistantProperty.3.txt" << "nonexistantProperty.3.errors.txt" << true;
    QTest::newRow("nonExistantProperty.4") << "nonexistantProperty.4.txt" << "nonexistantProperty.4.errors.txt" << true;
    QTest::newRow("nonExistantProperty.5") << "nonexistantProperty.5.txt" << "nonexistantProperty.5.errors.txt" << false;
    QTest::newRow("nonExistantProperty.6") << "nonexistantProperty.6.txt" << "nonexistantProperty.6.errors.txt" << true;


    QTest::newRow("wrongType (string for int)") << "wrongType.1.txt" << "wrongType.1.errors.txt" << false;
    QTest::newRow("wrongType (int for bool)") << "wrongType.2.txt" << "wrongType.2.errors.txt" << false;
    QTest::newRow("wrongType (bad rect)") << "wrongType.3.txt" << "wrongType.3.errors.txt" << false;


    QTest::newRow("nonExistantProperty.1") << "readOnly.1.txt" << "readOnly.1.errors.txt" << false;
    QTest::newRow("nonExistantProperty.2") << "readOnly.2.txt" << "readOnly.2.errors.txt" << true;


    QTest::newRow("unsupportedProperty") << "unsupportedProperty.txt" << "unsupportedProperty.errors.txt" << true;
    QTest::newRow("nullDotProperty") << "nullDotProperty.txt" << "nullDotProperty.errors.txt" << true;
    QTest::newRow("fakeDotProperty") << "fakeDotProperty.txt" << "fakeDotProperty.errors.txt" << true;
    QTest::newRow("duplicateIDs") << "duplicateIDs.txt" << "duplicateIDs.errors.txt" << false;
    QTest::newRow("invalidID") << "invalidID.txt" << "invalidID.errors.txt" << false;
    QTest::newRow("unregisteredObject") << "unregisteredObject.txt" << "unregisteredObject.errors.txt" << false;
    QTest::newRow("empty") << "empty.txt" << "empty.errors.txt" << false;
    QTest::newRow("missingObject") << "missingObject.txt" << "missingObject.errors.txt" << false;
}

void tst_qmlparser::errors()
{
    QFETCH(QString, file);
    QFETCH(QString, errorFile);
    QFETCH(bool, create);

    QmlComponent component(&engine, TEST_FILE(file));

    if(create) {
        QObject *object = component.create();
        QVERIFY(object == 0);
    }

    VERIFY_ERRORS(errorFile.toLatin1().constData());
}

void tst_qmlparser::simpleObject()
{
    QmlComponent component(&engine, TEST_FILE("simpleObject.txt"));
    VERIFY_ERRORS(0);
    QObject *object = component.create();
    QVERIFY(object != 0);
}

void tst_qmlparser::simpleContainer()
{
    QmlComponent component(&engine, TEST_FILE("simpleContainer.txt"));
    MyContainer *container= qobject_cast<MyContainer*>(component.create());
    QVERIFY(container != 0);
    QCOMPARE(container->children()->count(),2);
}

void tst_qmlparser::interfaceProperty()
{
    QmlComponent component(&engine, TEST_FILE("interfaceProperty.txt"));
    MyQmlObject *object = qobject_cast<MyQmlObject*>(component.create());
    QVERIFY(object != 0);
    QVERIFY(object->interface());
    QVERIFY(object->interface()->id == 913);
}

void tst_qmlparser::interfaceQmlList()
{
    QmlComponent component(&engine, TEST_FILE("interfaceQmlList.txt"));
    MyContainer *container= qobject_cast<MyContainer*>(component.create());
    QVERIFY(container != 0);
    QVERIFY(container->qmllistAccessor().count() == 2);
    for(int ii = 0; ii < 2; ++ii)
        QVERIFY(container->qmllistAccessor().at(ii)->id == 913);
}

void tst_qmlparser::interfaceQList()
{
    QmlComponent component(&engine, TEST_FILE("interfaceQList.txt"));
    MyContainer *container= qobject_cast<MyContainer*>(component.create());
    QVERIFY(container != 0);
    QVERIFY(container->qlistInterfaces()->count() == 2);
    for(int ii = 0; ii < 2; ++ii)
        QVERIFY(container->qlistInterfaces()->at(ii)->id == 913);
}

void tst_qmlparser::assignObjectToSignal()
{
    QmlComponent component(&engine, TEST_FILE("assignObjectToSignal.txt"));
    MyQmlObject *object = qobject_cast<MyQmlObject *>(component.create());
    QVERIFY(object != 0);
    QTest::ignoreMessage(QtWarningMsg, "MyQmlObject::basicSlot");
    emit object->basicSignal();
}

void tst_qmlparser::crash1()
{
    QmlComponent component(&engine, "Component {}");
}

QTEST_MAIN(tst_qmlparser)

#include "tst_qmlparser.moc"
