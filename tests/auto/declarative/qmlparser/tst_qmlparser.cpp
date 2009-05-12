#include <qtest.h>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <QtDeclarative/qmlpropertyvaluesource.h>
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

class MyQmlObject : public QObject, public MyInterface, public QmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue)
    Q_PROPERTY(QString readOnlyString READ readOnlyString)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled)
    Q_PROPERTY(QRect rect READ rect WRITE setRect)
    Q_PROPERTY(QMatrix matrix READ matrix WRITE setMatrix)  //assumed to be unsupported by QML
    Q_PROPERTY(MyInterface *interface READ interface WRITE setInterface)
    Q_INTERFACES(MyInterface QmlParserStatus)
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

    static QObject *qmlAttachedProperties(QObject *other) {
        MyQmlObject *rv = new MyQmlObject;
        rv->setParent(other);
        return rv;
    }
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

class MyTypeObject : public QObject
{
    Q_OBJECT
    Q_ENUMS(MyEnum)
    Q_FLAGS(MyFlags)

    Q_PROPERTY(QString id READ id WRITE setId);
    Q_PROPERTY(QObject *objectProperty READ objectProperty WRITE setObjectProperty);
    Q_PROPERTY(QmlComponent *componentProperty READ componentProperty WRITE setComponentProperty);
    Q_PROPERTY(MyFlags flagProperty READ flagProperty WRITE setFlagProperty);
    Q_PROPERTY(MyEnum enumProperty READ enumProperty WRITE setEnumProperty);
    Q_PROPERTY(QString stringProperty READ stringProperty WRITE setStringProperty);
    Q_PROPERTY(uint uintProperty READ uintProperty WRITE setUintProperty);
    Q_PROPERTY(int intProperty READ intProperty WRITE setIntProperty);
    Q_PROPERTY(qreal realProperty READ realProperty WRITE setRealProperty);
    Q_PROPERTY(double doubleProperty READ doubleProperty WRITE setDoubleProperty);
    Q_PROPERTY(QColor colorProperty READ colorProperty WRITE setColorProperty);
    Q_PROPERTY(QDate dateProperty READ dateProperty WRITE setDateProperty);
    Q_PROPERTY(QTime timeProperty READ timeProperty WRITE setTimeProperty);
    Q_PROPERTY(QDateTime dateTimeProperty READ dateTimeProperty WRITE setDateTimeProperty);
    Q_PROPERTY(QPoint pointProperty READ pointProperty WRITE setPointProperty);
    Q_PROPERTY(QPointF pointFProperty READ pointFProperty WRITE setPointFProperty);
    Q_PROPERTY(QSize sizeProperty READ sizeProperty WRITE setSizeProperty);
    Q_PROPERTY(QSizeF sizeFProperty READ sizeFProperty WRITE setSizeFProperty);
    Q_PROPERTY(QRect rectProperty READ rectProperty WRITE setRectProperty);
    Q_PROPERTY(QRectF rectFProperty READ rectFProperty WRITE setRectFProperty);
    Q_PROPERTY(bool boolProperty READ boolProperty WRITE setBoolProperty);
    Q_PROPERTY(QVariant variantProperty READ variantProperty WRITE setVariantProperty);

public:
    MyTypeObject()
        : objectPropertyValue(0), componentPropertyValue(0) {}

    QString idValue;
    QString id() const {
        return idValue;
    }
    void setId(const QString &v) {
        idValue = v;
    }

    QObject *objectPropertyValue;
    QObject *objectProperty() const {
        return objectPropertyValue;
    }
    void setObjectProperty(QObject *v) {
        objectPropertyValue = v;
    }

    QmlComponent *componentPropertyValue;
    QmlComponent *componentProperty() const {
        return componentPropertyValue;
    }
    void setComponentProperty(QmlComponent *v) {
        componentPropertyValue = v;
    }

    enum MyFlag { FlagVal1 = 0x01, FlagVal2 = 0x02, FlagVal3 = 0x04 };
    Q_DECLARE_FLAGS(MyFlags, MyFlag)
    MyFlags flagPropertyValue;
    MyFlags flagProperty() const {
        return flagPropertyValue;
    }
    void setFlagProperty(MyFlags v) {
        flagPropertyValue = v;
    }

    enum MyEnum { EnumVal1, EnumVal2 };
    MyEnum enumPropertyValue;
    MyEnum enumProperty() const {
        return enumPropertyValue;
    }
    void setEnumProperty(MyEnum v) {
        enumPropertyValue = v;
    }

    QString stringPropertyValue;
    QString stringProperty() const {
       return stringPropertyValue;
    }
    void setStringProperty(const QString &v) {
        stringPropertyValue = v;
    }

    uint uintPropertyValue;
    uint uintProperty() const {
       return uintPropertyValue;
    }
    void setUintProperty(const uint &v) {
        uintPropertyValue = v;
    }

    int intPropertyValue;
    int intProperty() const {
       return intPropertyValue;
    }
    void setIntProperty(const int &v) {
        intPropertyValue = v;
    }

    qreal realPropertyValue;
    qreal realProperty() const {
       return realPropertyValue;
    }
    void setRealProperty(const qreal &v) {
        realPropertyValue = v;
    }

    double doublePropertyValue;
    double doubleProperty() const {
       return doublePropertyValue;
    }
    void setDoubleProperty(const double &v) {
        doublePropertyValue = v;
    }

    QColor colorPropertyValue;
    QColor colorProperty() const {
       return colorPropertyValue;
    }
    void setColorProperty(const QColor &v) {
        colorPropertyValue = v;
    }

    QDate datePropertyValue;
    QDate dateProperty() const {
       return datePropertyValue;
    }
    void setDateProperty(const QDate &v) {
        datePropertyValue = v;
    }

    QTime timePropertyValue;
    QTime timeProperty() const {
       return timePropertyValue;
    }
    void setTimeProperty(const QTime &v) {
        timePropertyValue = v;
    }

    QDateTime dateTimePropertyValue;
    QDateTime dateTimeProperty() const {
       return dateTimePropertyValue;
    }
    void setDateTimeProperty(const QDateTime &v) {
        dateTimePropertyValue = v;
    }

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

    bool boolPropertyValue;
    bool boolProperty() const {
       return boolPropertyValue;
    }
    void setBoolProperty(const bool &v) {
        boolPropertyValue = v;
    }

    QVariant variantPropertyValue;
    QVariant variantProperty() const {
       return variantPropertyValue;
    }
    void setVariantProperty(const QVariant &v) {
        variantPropertyValue = v;
    }
};
Q_DECLARE_OPERATORS_FOR_FLAGS(MyTypeObject::MyFlags)

QML_DECLARE_TYPE(MyTypeObject);
QML_DEFINE_TYPE(MyTypeObject,MyTypeObject);

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

class MyPropertyValueSource : public QmlPropertyValueSource
{
    Q_OBJECT
public:
    MyPropertyValueSource()
        : QmlPropertyValueSource(0) {}

    QmlMetaProperty prop;
    virtual void setTarget(const QmlMetaProperty &p)
    {
        prop = p;
    }
};
QML_DECLARE_TYPE(MyPropertyValueSource);
QML_DEFINE_TYPE(MyPropertyValueSource,MyPropertyValueSource);

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
    void assignQmlComponent();
    void assignBasicTypes();
    void customParserTypes();
    void rootAsQmlComponent();
    void inlineQmlComponents();
    void idProperty();
    void assignSignal();
    void dynamicProperties();
    void dynamicSignalsAndSlots();
    void simpleBindings();
    void autoComponentCreation();
    void propertyValueSource();
    void attachedProperties();

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

    QTest::newRow("wrongType (invalid enum)") << "wrongType.4.txt" << "wrongType.4.errors.txt" << false;
    QTest::newRow("wrongType (int for uint)") << "wrongType.5.txt" << "wrongType.5.errors.txt" << false;
    QTest::newRow("wrongType (string for real)") << "wrongType.6.txt" << "wrongType.6.errors.txt" << false;
    QTest::newRow("wrongType (int for color)") << "wrongType.7.txt" << "wrongType.7.errors.txt" << false;
    QTest::newRow("wrongType (int for date)") << "wrongType.8.txt" << "wrongType.8.errors.txt" << false;
    QTest::newRow("wrongType (int for time)") << "wrongType.9.txt" << "wrongType.9.errors.txt" << false;
    QTest::newRow("wrongType (int for datetime)") << "wrongType.10.txt" << "wrongType.10.errors.txt" << false;
    QTest::newRow("wrongType (string for point)") << "wrongType.11.txt" << "wrongType.11.errors.txt" << false;
    QTest::newRow("wrongType (color for size)") << "wrongType.12.txt" << "wrongType.12.errors.txt" << false;

    QTest::newRow("readOnly.1") << "readOnly.1.txt" << "readOnly.1.errors.txt" << false;
    QTest::newRow("readOnly.2") << "readOnly.2.txt" << "readOnly.2.errors.txt" << true;

    QTest::newRow("listAssignment.1") << "listAssignment.1.txt" << "listAssignment.1.errors.txt" << false;
    QTest::newRow("listAssignment.2") << "listAssignment.2.txt" << "listAssignment.2.errors.txt" << false;
    QTest::newRow("listAssignment.3") << "listAssignment.3.txt" << "listAssignment.3.errors.txt" << false;

    QTest::newRow("invalidID.1") << "invalidID.txt" << "invalidID.errors.txt" << false;
    QTest::newRow("invalidID.2") << "invalidID.2.txt" << "invalidID.2.errors.txt" << false;
    QTest::newRow("invalidID.3") << "invalidID.3.txt" << "invalidID.3.errors.txt" << false;
    QTest::newRow("invalidID.4") << "invalidID.4.txt" << "invalidID.4.errors.txt" << false;

    QTest::newRow("unsupportedProperty") << "unsupportedProperty.txt" << "unsupportedProperty.errors.txt" << true;
    QTest::newRow("nullDotProperty") << "nullDotProperty.txt" << "nullDotProperty.errors.txt" << true;
    QTest::newRow("fakeDotProperty") << "fakeDotProperty.txt" << "fakeDotProperty.errors.txt" << true;
    QTest::newRow("duplicateIDs") << "duplicateIDs.txt" << "duplicateIDs.errors.txt" << false;
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


// Test is an external component can be loaded and assigned (to a qlist)
void tst_qmlparser::assignQmlComponent()
{
    QmlComponent component(&engine, TEST_FILE("assignQmlComponent.txt"));
    MyContainer *object = qobject_cast<MyContainer *>(component.create());
    QVERIFY(object != 0);
    QVERIFY(object->children()->count() == 1);
    QObject *child = object->children()->at(0);
    QCOMPARE(child->property("x"), QVariant(10));
    QCOMPARE(child->property("y"), QVariant(11));
}

// Test literal assignment to all the basic types 
void tst_qmlparser::assignBasicTypes()
{
    QmlComponent component(&engine, TEST_FILE("assignBasicTypes.txt"));
    MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
    QVERIFY(object != 0);
    QCOMPARE(object->flagProperty(), MyTypeObject::FlagVal1 | MyTypeObject::FlagVal3);
    QCOMPARE(object->enumProperty(), MyTypeObject::EnumVal2);
    QCOMPARE(object->stringProperty(), QString("Hello World!"));
    QCOMPARE(object->uintProperty(), uint(10));
    QCOMPARE(object->intProperty(), -19);
    QCOMPARE((float)object->realProperty(), float(23.2));
    QCOMPARE((float)object->doubleProperty(), float(-19.7));
    QCOMPARE(object->colorProperty(), QColor("red"));
    QCOMPARE(object->dateProperty(), QDate(1982, 11, 25));
    QCOMPARE(object->timeProperty(), QTime(11, 11, 32));
    QCOMPARE(object->dateTimeProperty(), QDateTime(QDate(2009, 5, 12), QTime(13, 22, 1)));
    QCOMPARE(object->pointProperty(), QPoint(99,13));
    QCOMPARE(object->pointFProperty(), QPointF((float)-10.1, (float)12.3));
    QCOMPARE(object->sizeProperty(), QSize(99, 13));
    QCOMPARE(object->sizeFProperty(), QSizeF((float)0.1, (float)0.2));
    QCOMPARE(object->rectProperty(), QRect(9, 7, 100, 200));
    QCOMPARE(object->rectFProperty(), QRectF((float)1000.1, (float)-10.9, (float)400, (float)90.99));
    QCOMPARE(object->boolProperty(), true);
    QCOMPARE(object->variantProperty(), QVariant("Hello World!"));
    QVERIFY(object->objectProperty() != 0);
    MyTypeObject *child = qobject_cast<MyTypeObject *>(object->objectProperty());
    QVERIFY(child != 0);
    QCOMPARE(child->intProperty(), 8);
}

// Tests that custom parser tyeps can be instantiated
void tst_qmlparser::customParserTypes()
{
    QmlComponent component(&engine, TEST_FILE("customParserTypes.txt"));
    QObject *object = component.create();
    QVERIFY(object != 0);
    QVERIFY(object->property("count") == QVariant(2));
}

// Tests that the root item can be a custom component
void tst_qmlparser::rootAsQmlComponent()
{
    QmlComponent component(&engine, TEST_FILE("rootAsQmlComponent.txt"));
    MyContainer *object = qobject_cast<MyContainer *>(component.create());
    QVERIFY(object != 0);
    QCOMPARE(object->property("x"), QVariant(11));
    QCOMPARE(object->children()->count(), 2);
}

// Tests that components can be specified inline
void tst_qmlparser::inlineQmlComponents()
{
    QmlComponent component(&engine, TEST_FILE("inlineQmlComponents.txt"));
    MyContainer *object = qobject_cast<MyContainer *>(component.create());
    QVERIFY(object != 0);
    QCOMPARE(object->children()->count(), 1);
    QmlComponent *comp = qobject_cast<QmlComponent *>(object->children()->at(0));
    QVERIFY(comp != 0);
    MyQmlObject *compObject = qobject_cast<MyQmlObject *>(comp->create());
    QVERIFY(compObject != 0);
    QCOMPARE(compObject->value(), 11);
}

// Tests that types that have an id property have it set
void tst_qmlparser::idProperty()
{
    QmlComponent component(&engine, TEST_FILE("idProperty.txt"));
    MyContainer *object = qobject_cast<MyContainer *>(component.create());
    QVERIFY(object != 0);
    QCOMPARE(object->children()->count(), 1);
    MyTypeObject *child = 
        qobject_cast<MyTypeObject *>(object->children()->at(0));
    QVERIFY(child != 0);
    QCOMPARE(child->id(), QString("MyObjectId"));
    QCOMPARE(object->property("object"), QVariant::fromValue((QObject *)child));
}

// Tests that signals can be assigned to
void tst_qmlparser::assignSignal()
{
    QmlComponent component(&engine, TEST_FILE("assignSignal.txt"));
    MyQmlObject *object = qobject_cast<MyQmlObject *>(component.create());
    QVERIFY(object != 0);
    QTest::ignoreMessage(QtWarningMsg, "MyQmlObject::basicSlot");
    emit object->basicSignal();
}

// Tests the creation and assignment of dynamic properties
void tst_qmlparser::dynamicProperties()
{
    QmlComponent component(&engine, TEST_FILE("dynamicProperties.txt"));
    QObject *object = component.create();
    QVERIFY(object != 0);
    QCOMPARE(object->property("intProperty"), QVariant(10));
    QCOMPARE(object->property("boolProperty"), QVariant(false));
    QCOMPARE(object->property("doubleProperty"), QVariant((float)-10.1));
    QCOMPARE(object->property("realProperty"), QVariant((float)-19.9));
    QCOMPARE(object->property("stringProperty"), QVariant("Hello World!"));
    QCOMPARE(object->property("colorProperty"), QVariant(QColor("red")));
    QCOMPARE(object->property("dateProperty"), QVariant(QDate(1945, 9, 2)));
    QCOMPARE(object->property("varProperty"), QVariant("Hello World!"));
    QCOMPARE(object->property("variantProperty"), QVariant(12));
}

// Tests the declaration of dynamic signals and slots
void tst_qmlparser::dynamicSignalsAndSlots()
{
    QmlComponent component(&engine, TEST_FILE("dynamicSignalsAndSlots.txt"));
    QObject *object = component.create();
    QVERIFY(object != 0);
    QVERIFY(object->metaObject()->indexOfMethod("signal1()") != -1);
    QVERIFY(object->metaObject()->indexOfMethod("signal2()") != -1);
    QVERIFY(object->metaObject()->indexOfMethod("slot1()") != -1);
    QVERIFY(object->metaObject()->indexOfMethod("slot2()") != -1);
}

void tst_qmlparser::simpleBindings()
{
    QmlComponent component(&engine, TEST_FILE("simpleBindings.txt"));
    QObject *object = component.create();
    QVERIFY(object != 0);
    QCOMPARE(object->property("value1"), QVariant(10));
    QCOMPARE(object->property("value2"), QVariant(10));
    QCOMPARE(object->property("value3"), QVariant(21));
    QCOMPARE(object->property("value4"), QVariant(10));
    QCOMPARE(object->property("objectProperty"), QVariant::fromValue(object));
}

void tst_qmlparser::autoComponentCreation()
{
    QmlComponent component(&engine, TEST_FILE("autoComponentCreation.txt"));
    MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
    QVERIFY(object != 0);
    QVERIFY(object->componentProperty() != 0);
    MyTypeObject *child = qobject_cast<MyTypeObject *>(object->componentProperty()->create());
    QVERIFY(child != 0);
    QCOMPARE(child->realProperty(), qreal(9));
}

void tst_qmlparser::propertyValueSource()
{
    QmlComponent component(&engine, TEST_FILE("propertyValueSource.txt"));
    MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
    QVERIFY(object != 0);
    QList<QmlPropertyValueSource *> valueSources = 
        object->findChildren<QmlPropertyValueSource *>();
    QCOMPARE(valueSources.count(), 1);
    MyPropertyValueSource *valueSource = 
        qobject_cast<MyPropertyValueSource *>(valueSources.at(0));
    QVERIFY(valueSource != 0);
    QCOMPARE(valueSource->prop.object(), object);
    QCOMPARE(valueSource->prop.name(), QString(QLatin1String("intProperty")));
}

void tst_qmlparser::attachedProperties()
{
    QmlComponent component(&engine, TEST_FILE("attachedProperties.txt"));
    QObject *object = component.create();
    QVERIFY(object != 0);
    QObject *attached = qmlAttachedPropertiesObject<MyQmlObject>(object);
    QVERIFY(attached != 0);
    QCOMPARE(attached->property("value"), QVariant(10));
}

void tst_qmlparser::crash1()
{
    QmlComponent component(&engine, "Component {}");
}

QTEST_MAIN(tst_qmlparser)

#include "tst_qmlparser.moc"
