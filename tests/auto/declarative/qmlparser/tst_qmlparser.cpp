#include <qtest.h>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <QtCore/qfile.h>
#include <QtCore/qdebug.h>
#include <QtCore/qfileinfo.h>
#include <QtCore/qdir.h>
#include "testtypes.h"

class tst_qmlparser : public QObject
{
    Q_OBJECT
public:
    tst_qmlparser() {
        QmlMetaType::registerCustomStringConverter(qMetaTypeId<MyCustomVariantType>(), myCustomVariantTypeConverter);
        QFileInfo fileInfo(__FILE__);
        engine.addImportPath(fileInfo.absoluteDir().filePath(QLatin1String("lib")));
    }

private slots:

    void errors_data();
    void errors();

    void simpleObject();
    void simpleContainer();
    void interfaceProperty();
    void interfaceQmlList();
    void interfaceQList();
    void assignObjectToSignal();
    void assignObjectToVariant();
    void assignLiteralSignalProperty();
    void assignQmlComponent();
    void assignBasicTypes();
    void assignTypeExtremes();
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
    void dynamicObjects();
    void customVariantTypes();
    void valueTypes();

    void imports_data();
    void imports();

    // regression tests for crashes
    void crash1();

private:
    QmlEngine engine;
};

#define VERIFY_ERRORS(errorfile) \
    if (!errorfile) { \
        if (qgetenv("DEBUG") != "" && !component.errors().isEmpty()) \
            qWarning() << "Unexpected Errors:" << component.errors(); \
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
        if (qgetenv("DEBUG") != "" && expected != actual) \
            qWarning() << "Expected:" << expected << "Actual:" << actual;  \
        QCOMPARE(expected, actual); \
    }

inline QUrl TEST_FILE(const QString &filename)
{
    QFileInfo fileInfo(__FILE__);
    return QUrl::fromLocalFile(fileInfo.absoluteDir().filePath(filename));
}

inline QUrl TEST_FILE(const char *filename)
{
    return TEST_FILE(QLatin1String(filename));
}

void tst_qmlparser::errors_data()
{
    QTest::addColumn<QString>("file");
    QTest::addColumn<QString>("errorFile");
    QTest::addColumn<bool>("create");

    QTest::newRow("nonexistantProperty.1") << "nonexistantProperty.1.qml" << "nonexistantProperty.1.errors.txt" << false;
    QTest::newRow("nonexistantProperty.2") << "nonexistantProperty.2.qml" << "nonexistantProperty.2.errors.txt" << false;
    QTest::newRow("nonexistantProperty.3") << "nonexistantProperty.3.qml" << "nonexistantProperty.3.errors.txt" << false;
    QTest::newRow("nonexistantProperty.4") << "nonexistantProperty.4.qml" << "nonexistantProperty.4.errors.txt" << false;
    QTest::newRow("nonexistantProperty.5") << "nonexistantProperty.5.qml" << "nonexistantProperty.5.errors.txt" << false;
    QTest::newRow("nonexistantProperty.6") << "nonexistantProperty.6.qml" << "nonexistantProperty.6.errors.txt" << false;

    QTest::newRow("wrongType (string for int)") << "wrongType.1.qml" << "wrongType.1.errors.txt" << false;
    QTest::newRow("wrongType (int for bool)") << "wrongType.2.qml" << "wrongType.2.errors.txt" << false;
    QTest::newRow("wrongType (bad rect)") << "wrongType.3.qml" << "wrongType.3.errors.txt" << false;

    QTest::newRow("wrongType (invalid enum)") << "wrongType.4.qml" << "wrongType.4.errors.txt" << false;
    QTest::newRow("wrongType (int for uint)") << "wrongType.5.qml" << "wrongType.5.errors.txt" << false;
    QTest::newRow("wrongType (string for real)") << "wrongType.6.qml" << "wrongType.6.errors.txt" << false;
    QTest::newRow("wrongType (int for color)") << "wrongType.7.qml" << "wrongType.7.errors.txt" << false;
    QTest::newRow("wrongType (int for date)") << "wrongType.8.qml" << "wrongType.8.errors.txt" << false;
    QTest::newRow("wrongType (int for time)") << "wrongType.9.qml" << "wrongType.9.errors.txt" << false;
    QTest::newRow("wrongType (int for datetime)") << "wrongType.10.qml" << "wrongType.10.errors.txt" << false;
    QTest::newRow("wrongType (string for point)") << "wrongType.11.qml" << "wrongType.11.errors.txt" << false;
    QTest::newRow("wrongType (color for size)") << "wrongType.12.qml" << "wrongType.12.errors.txt" << false;
    QTest::newRow("wrongType (number string for int)") << "wrongType.13.qml" << "wrongType.13.errors.txt" << false;
    QTest::newRow("wrongType (int for string)") << "wrongType.14.qml" << "wrongType.14.errors.txt" << false;

    QTest::newRow("readOnly.1") << "readOnly.1.qml" << "readOnly.1.errors.txt" << false;
    QTest::newRow("readOnly.2") << "readOnly.2.qml" << "readOnly.2.errors.txt" << false;

    QTest::newRow("listAssignment.1") << "listAssignment.1.qml" << "listAssignment.1.errors.txt" << false;
    QTest::newRow("listAssignment.2") << "listAssignment.2.qml" << "listAssignment.2.errors.txt" << false;
    QTest::newRow("listAssignment.3") << "listAssignment.3.qml" << "listAssignment.3.errors.txt" << false;

    QTest::newRow("invalidID.1") << "invalidID.qml" << "invalidID.errors.txt" << false;
    QTest::newRow("invalidID.2") << "invalidID.2.qml" << "invalidID.2.errors.txt" << false;
    QTest::newRow("invalidID.3") << "invalidID.3.qml" << "invalidID.3.errors.txt" << false;
    QTest::newRow("invalidID.4") << "invalidID.4.qml" << "invalidID.4.errors.txt" << false;
    QTest::newRow("invalidID.5") << "invalidID.5.qml" << "invalidID.5.errors.txt" << false;


    QTest::newRow("unsupportedProperty") << "unsupportedProperty.qml" << "unsupportedProperty.errors.txt" << false;
    QTest::newRow("nullDotProperty") << "nullDotProperty.qml" << "nullDotProperty.errors.txt" << true;
    QTest::newRow("fakeDotProperty") << "fakeDotProperty.qml" << "fakeDotProperty.errors.txt" << false;
    QTest::newRow("duplicateIDs") << "duplicateIDs.qml" << "duplicateIDs.errors.txt" << false;
    QTest::newRow("unregisteredObject") << "unregisteredObject.qml" << "unregisteredObject.errors.txt" << false;
    QTest::newRow("empty") << "empty.qml" << "empty.errors.txt" << false;
    QTest::newRow("missingObject") << "missingObject.qml" << "missingObject.errors.txt" << false;
    QTest::newRow("failingComponent") << "failingComponentTest.qml" << "failingComponent.errors.txt" << false;
    QTest::newRow("missingSignal") << "missingSignal.qml" << "missingSignal.errors.txt" << false;
    QTest::newRow("finalOverride") << "finalOverride.qml" << "finalOverride.errors.txt" << false;
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
    QmlComponent component(&engine, TEST_FILE("simpleObject.qml"));
    VERIFY_ERRORS(0);
    QObject *object = component.create();
    QVERIFY(object != 0);
}

void tst_qmlparser::simpleContainer()
{
    QmlComponent component(&engine, TEST_FILE("simpleContainer.qml"));
    VERIFY_ERRORS(0);
    MyContainer *container= qobject_cast<MyContainer*>(component.create());
    QVERIFY(container != 0);
    QCOMPARE(container->children()->count(),2);
}

void tst_qmlparser::interfaceProperty()
{
    QmlComponent component(&engine, TEST_FILE("interfaceProperty.qml"));
    VERIFY_ERRORS(0);
    MyQmlObject *object = qobject_cast<MyQmlObject*>(component.create());
    QVERIFY(object != 0);
    QVERIFY(object->interface());
    QVERIFY(object->interface()->id == 913);
}

void tst_qmlparser::interfaceQmlList()
{
    QmlComponent component(&engine, TEST_FILE("interfaceQmlList.qml"));
    VERIFY_ERRORS(0);
    MyContainer *container= qobject_cast<MyContainer*>(component.create());
    QVERIFY(container != 0);
    QVERIFY(container->qmllistAccessor().count() == 2);
    for(int ii = 0; ii < 2; ++ii)
        QVERIFY(container->qmllistAccessor().at(ii)->id == 913);
}

void tst_qmlparser::interfaceQList()
{
    QmlComponent component(&engine, TEST_FILE("interfaceQList.qml"));
    VERIFY_ERRORS(0);
    MyContainer *container= qobject_cast<MyContainer*>(component.create());
    QVERIFY(container != 0);
    QVERIFY(container->qlistInterfaces()->count() == 2);
    for(int ii = 0; ii < 2; ++ii)
        QVERIFY(container->qlistInterfaces()->at(ii)->id == 913);
}

void tst_qmlparser::assignObjectToSignal()
{
    QmlComponent component(&engine, TEST_FILE("assignObjectToSignal.qml"));
    VERIFY_ERRORS(0);
    MyQmlObject *object = qobject_cast<MyQmlObject *>(component.create());
    QVERIFY(object != 0);
    QTest::ignoreMessage(QtWarningMsg, "MyQmlObject::basicSlot");
    emit object->basicSignal();
}

void tst_qmlparser::assignObjectToVariant()
{
    QmlComponent component(&engine, TEST_FILE("assignObjectToVariant.qml"));
    VERIFY_ERRORS(0);
    QObject *object = component.create();
    QVERIFY(object != 0);
    QVariant v = object->property("a");
    QVERIFY(v.userType() == qMetaTypeId<QObject *>());
}

void tst_qmlparser::assignLiteralSignalProperty()
{
    QmlComponent component(&engine, TEST_FILE("assignLiteralSignalProperty.qml"));
    VERIFY_ERRORS(0);
    MyQmlObject *object = qobject_cast<MyQmlObject *>(component.create());
    QVERIFY(object != 0);
    QCOMPARE(object->onLiteralSignal(), 10);
}

// Test is an external component can be loaded and assigned (to a qlist)
void tst_qmlparser::assignQmlComponent()
{
    QmlComponent component(&engine, TEST_FILE("assignQmlComponent.qml"));
    VERIFY_ERRORS(0);
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
    QmlComponent component(&engine, TEST_FILE("assignBasicTypes.qml"));
    VERIFY_ERRORS(0);
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

// Test edge case type assignments
void tst_qmlparser::assignTypeExtremes()
{
    QmlComponent component(&engine, TEST_FILE("assignTypeExtremes.qml"));
    VERIFY_ERRORS(0);
    MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
    QVERIFY(object != 0);
    QCOMPARE(object->uintProperty(), 0xEE6B2800);
    QCOMPARE(object->intProperty(), -0x77359400);
}

// Tests that custom parser tyeps can be instantiated
void tst_qmlparser::customParserTypes()
{
    QmlComponent component(&engine, TEST_FILE("customParserTypes.qml"));
    VERIFY_ERRORS(0);
    QObject *object = component.create();
    QVERIFY(object != 0);
    QVERIFY(object->property("count") == QVariant(2));
}

// Tests that the root item can be a custom component
void tst_qmlparser::rootAsQmlComponent()
{
    QmlComponent component(&engine, TEST_FILE("rootAsQmlComponent.qml"));
    VERIFY_ERRORS(0);
    MyContainer *object = qobject_cast<MyContainer *>(component.create());
    QVERIFY(object != 0);
    QCOMPARE(object->property("x"), QVariant(11));
    QCOMPARE(object->children()->count(), 2);
}

// Tests that components can be specified inline
void tst_qmlparser::inlineQmlComponents()
{
    QmlComponent component(&engine, TEST_FILE("inlineQmlComponents.qml"));
    VERIFY_ERRORS(0);
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
    QmlComponent component(&engine, TEST_FILE("idProperty.qml"));
    VERIFY_ERRORS(0);
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
    QmlComponent component(&engine, TEST_FILE("assignSignal.qml"));
    VERIFY_ERRORS(0);
    MyQmlObject *object = qobject_cast<MyQmlObject *>(component.create());
    QVERIFY(object != 0);
    QTest::ignoreMessage(QtWarningMsg, "MyQmlObject::basicSlot");
    emit object->basicSignal();
}

// Tests the creation and assignment of dynamic properties
void tst_qmlparser::dynamicProperties()
{
    QmlComponent component(&engine, TEST_FILE("dynamicProperties.qml"));
    VERIFY_ERRORS(0);
    QObject *object = component.create();
    QVERIFY(object != 0);
    QCOMPARE(object->property("intProperty"), QVariant(10));
    QCOMPARE(object->property("boolProperty"), QVariant(false));
    QCOMPARE(object->property("doubleProperty"), QVariant(-10.1));
    QCOMPARE(object->property("realProperty"), QVariant((qreal)-19.9));
    QCOMPARE(object->property("stringProperty"), QVariant("Hello World!"));
    QCOMPARE(object->property("colorProperty"), QVariant(QColor("red")));
    QCOMPARE(object->property("dateProperty"), QVariant(QDate(1945, 9, 2)));
    QCOMPARE(object->property("varProperty"), QVariant("Hello World!"));
    QCOMPARE(object->property("variantProperty"), QVariant(12));
}

// Tests the declaration of dynamic signals and slots
void tst_qmlparser::dynamicSignalsAndSlots()
{
    QmlComponent component(&engine, TEST_FILE("dynamicSignalsAndSlots.qml"));
    VERIFY_ERRORS(0);
    QObject *object = component.create();
    QVERIFY(object != 0);
    QVERIFY(object->metaObject()->indexOfMethod("signal1()") != -1);
    QVERIFY(object->metaObject()->indexOfMethod("signal2()") != -1);
    QVERIFY(object->metaObject()->indexOfMethod("slot1()") != -1);
    QVERIFY(object->metaObject()->indexOfMethod("slot2()") != -1);
}

void tst_qmlparser::simpleBindings()
{
    QmlComponent component(&engine, TEST_FILE("simpleBindings.qml"));
    VERIFY_ERRORS(0);
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
    QmlComponent component(&engine, TEST_FILE("autoComponentCreation.qml"));
    VERIFY_ERRORS(0);
    MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
    QVERIFY(object != 0);
    QVERIFY(object->componentProperty() != 0);
    MyTypeObject *child = qobject_cast<MyTypeObject *>(object->componentProperty()->create());
    QVERIFY(child != 0);
    QCOMPARE(child->realProperty(), qreal(9));
}

void tst_qmlparser::propertyValueSource()
{
    QmlComponent component(&engine, TEST_FILE("propertyValueSource.qml"));
    VERIFY_ERRORS(0);
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
    QmlComponent component(&engine, TEST_FILE("attachedProperties.qml"));
    VERIFY_ERRORS(0);
    QObject *object = component.create();
    QVERIFY(object != 0);
    QObject *attached = qmlAttachedPropertiesObject<MyQmlObject>(object);
    QVERIFY(attached != 0);
    QCOMPARE(attached->property("value"), QVariant(10));
}

// Tests non-static object properties
void tst_qmlparser::dynamicObjects()
{
    QmlComponent component(&engine, TEST_FILE("dynamicObject.1.qml"));
    VERIFY_ERRORS(0);
    QObject *object = component.create();
    QVERIFY(object != 0);
}

// Tests the registration of custom variant string converters
void tst_qmlparser::customVariantTypes()
{
    QmlComponent component(&engine, TEST_FILE("customVariantTypes.qml"));
    VERIFY_ERRORS(0);
    MyQmlObject *object = qobject_cast<MyQmlObject*>(component.create());
    QVERIFY(object != 0);
    QCOMPARE(object->customType().a, 10);
}

void tst_qmlparser::valueTypes()
{
    QmlComponent component(&engine, TEST_FILE("valueTypes.qml"));
    VERIFY_ERRORS(0);
    MyTypeObject *object = qobject_cast<MyTypeObject*>(component.create());
    QVERIFY(object != 0);

    QCOMPARE(object->rectProperty(), QRect(10, 11, 12, 13));
    QCOMPARE(object->rectProperty2(), QRect(10, 11, 12, 13));
    QCOMPARE(object->intProperty(), 10);
    object->doAction();
    QCOMPARE(object->rectProperty(), QRect(12, 11, 14, 13));
    QCOMPARE(object->rectProperty2(), QRect(12, 11, 14, 13));
    QCOMPARE(object->intProperty(), 12);

    QmlMetaProperty p = QmlMetaProperty::createProperty(object, "rectProperty.x");
    QCOMPARE(p.read(), QVariant(12));
    p.write(13);
    QCOMPARE(p.read(), QVariant(13));

    quint32 r = p.save();
    QmlMetaProperty p2;
    p2.restore(r, object);
    QCOMPARE(p2.read(), QVariant(13));
}

class TestType : public QObject {
    Q_OBJECT
public:
    TestType(QObject *p=0) : QObject(p) {}
};

class TestType2 : public QObject {
    Q_OBJECT
public:
    TestType2(QObject *p=0) : QObject(p) {}
};

QML_DECLARE_TYPE(TestType)
QML_DECLARE_TYPE(TestType2)

QML_DEFINE_TYPE(com.nokia.Test, 1, 0, 3, Test, TestType)
QML_DEFINE_TYPE(com.nokia.Test, 1, 5, 7, Test, TestType)
QML_DEFINE_TYPE(com.nokia.Test, 1, 8, 9, Test, TestType2)
QML_DEFINE_TYPE(com.nokia.Test, 1, 12, 13, Test, TestType2)
QML_DEFINE_TYPE(com.nokia.Test, 1, 9, 11, OldTest, TestType)

void tst_qmlparser::imports_data()
{
    QTest::addColumn<QString>("qml");
    QTest::addColumn<QString>("type");

    // import built-ins
    QTest::newRow("missing import")
        << "Test {}"
        << "";
    QTest::newRow("not in version 0.0")
        << "import com.nokia.Test 0.0\n"
           "Test {}"
        << "";
    QTest::newRow("in version 1.0")
        << "import com.nokia.Test 1.0\n"
           "Test {}"
        << "TestType";
    QTest::newRow("qualified wrong")
        << "import com.nokia.Test 1.0 as T\n"
           "Test {}"
        << "";
    QTest::newRow("qualified right")
        << "import com.nokia.Test 1.0 as T\n"
           "T.Test {}"
        << "TestType";
    QTest::newRow("qualified right but not in version 0.0")
        << "import com.nokia.Test 0.0 as T\n"
           "T.Test {}"
        << "";
    QTest::newRow("in version 1.1")
        << "import com.nokia.Test 1.1\n"
           "Test {}"
        << "TestType";
    QTest::newRow("in version 1.3")
        << "import com.nokia.Test 1.3\n"
           "Test {}"
        << "TestType";
    QTest::newRow("not in version 1.4")
        << "import com.nokia.Test 1.4\n"
           "Test {}"
        << "";
    QTest::newRow("in version 1.5")
        << "import com.nokia.Test 1.5\n"
           "Test {}"
        << "TestType";
    QTest::newRow("changed in version 1.8")
        << "import com.nokia.Test 1.8\n"
           "Test {}"
        << "TestType2";
    QTest::newRow("not in version 1.10")
        << "import com.nokia.Test 1.10\n"
           "Test {}"
        << "";
    QTest::newRow("back in version 1.12")
        << "import com.nokia.Test 1.12\n"
           "Test {}"
        << "TestType2";
    QTest::newRow("old in version 1.9")
        << "import com.nokia.Test 1.9\n"
           "OldTest {}"
        << "TestType";
    QTest::newRow("old in version 1.11")
        << "import com.nokia.Test 1.11\n"
           "OldTest {}"
        << "TestType";
    QTest::newRow("no old in version 1.12")
        << "import com.nokia.Test 1.12\n"
           "OldTest {}"
        << "";
    QTest::newRow("multiversion 1")
        << "import com.nokia.Test 1.11\n"
           "import com.nokia.Test 1.12\n"
           "Test {}"
        << "TestType2";
    QTest::newRow("multiversion 2")
        << "import com.nokia.Test 1.11\n"
           "import com.nokia.Test 1.12\n"
           "OldTest {}"
        << "TestType";
    QTest::newRow("qualified multiversion 3")
        << "import com.nokia.Test 1.0 as T0\n"
           "import com.nokia.Test 1.8 as T8\n"
           "T0.Test {}"
        << "TestType";
    QTest::newRow("qualified multiversion 4")
        << "import com.nokia.Test 1.0 as T0\n"
           "import com.nokia.Test 1.8 as T8\n"
           "T8.Test {}"
        << "TestType2";
    QTest::newRow("qualified multiversion 5")
        << "import com.nokia.Test 1.0 as T0\n"
           "import com.nokia.Test 1.10 as T10\n"
           "T10.Test {}"
        << "";

    // import locals
    QTest::newRow("local import")
        << "import \"subdir\"\n"
           "Test {}"
        << "QFxRect";
    QTest::newRow("local import as")
        << "import \"subdir\" as T\n"
           "T.Test {}"
        << "QFxRect";
    QTest::newRow("wrong local import as")
        << "import \"subdir\" as T\n"
           "Test {}"
        << "";
    QTest::newRow("library precedence over local import")
        << "import \"subdir\"\n"
           "import com.nokia.Test 1.0\n"
           "Test {}"
        << "TestType";

    // import installed
    QTest::newRow("installed import")
        << "import com.nokia.installedtest 1.0\n"
           "InstalledTest {}"
        << "QFxRect";
    QTest::newRow("installed import")
        << "import com.nokia.installedtest 1.4\n"
           "InstalledTest {}"
        << "QFxText";
}

void tst_qmlparser::imports()
{
    QFETCH(QString, qml);
    QFETCH(QString, type);

    QmlComponent component(&engine, qml.toUtf8(), TEST_FILE("empty.qml")); // just a file for relative local imports

    if (type.isEmpty()) {
        QVERIFY(component.isError());
    } else {
        VERIFY_ERRORS(0);
        QObject *object = component.create();
        QVERIFY(object != 0);
        QCOMPARE(QString(object->metaObject()->className()), type);
    }
}

void tst_qmlparser::crash1()
{
    QmlComponent component(&engine, "Component {}");
}

QTEST_MAIN(tst_qmlparser)

#include "tst_qmlparser.moc"
