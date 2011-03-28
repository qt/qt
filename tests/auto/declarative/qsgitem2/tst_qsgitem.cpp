/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include <qtest.h>
#include <QtTest/QSignalSpy>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <QtDeclarative/qdeclarativecontext.h>
#include <QtDeclarative/qsgview.h>
#include <private/qsgrectangle_p.h>
#include <private/qsgitem_p.h>
#include "../../../shared/util.h"

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

class tst_QSGItem : public QObject

{
    Q_OBJECT
public:
    tst_QSGItem();

private slots:
    void initTestCase();
    void keys();
    void keysProcessingOrder();
    void keyNavigation();
    void keyNavigation_RightToLeft();
    void keyNavigation_skipNotVisible();
    void keyNavigation_implicitSetting();
    void layoutMirroring();
    void layoutMirroringIllegalParent();
    void smooth();
    void clip();
    void mapCoordinates();
    void mapCoordinates_data();
    void propertyChanges();
    void transforms();
    void transforms_data();
    void childrenRect();
    void childrenRectBug();
    void childrenRectBug2();
    void childrenRectBug3();

    void childrenProperty();
    void resourcesProperty();
    void mouseFocus();

    void transformCrash();
    void implicitSize();
    void qtbug_16871();
private:
    QDeclarativeEngine engine;
};

template<typename T>
T *findItem(QSGItem *parent, const QString &objectName)
{
    if (!parent)
        return 0;

    const QMetaObject &mo = T::staticMetaObject;
    //qDebug() << parent->QSGItem::children().count() << "children";
    for (int i = 0; i < parent->childItems().count(); ++i) {
        QSGItem *item = qobject_cast<QSGItem*>(parent->childItems().at(i));
        if(!item)
            continue;
        //qDebug() << "try" << item;
        if (mo.cast(item) && (objectName.isEmpty() || item->objectName() == objectName))
            return static_cast<T*>(item);
        item = findItem<T>(item, objectName);
        if (item)
            return static_cast<T*>(item);
    }

    return 0;
}

class KeysTestObject : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool processLast READ processLast NOTIFY processLastChanged)

public:
    KeysTestObject() : mKey(0), mModifiers(0), mForwardedKey(0), mLast(false) {}

    void reset() {
        mKey = 0;
        mText = QString();
        mModifiers = 0;
        mForwardedKey = 0;
    }

    bool processLast() const { return mLast; }
    void setProcessLast(bool b) {
        if (b != mLast) {
            mLast = b;
            emit processLastChanged();
        }
    }

public slots:
    void keyPress(int key, QString text, int modifiers) {
        mKey = key;
        mText = text;
        mModifiers = modifiers;
    }
    void keyRelease(int key, QString text, int modifiers) {
        mKey = key;
        mText = text;
        mModifiers = modifiers;
    }
    void forwardedKey(int key) {
        mForwardedKey = key;
    }

signals:
    void processLastChanged();

public:
    int mKey;
    QString mText;
    int mModifiers;
    int mForwardedKey;
    bool mLast;

private:
};

class KeyTestItem : public QSGItem
{
    Q_OBJECT
public:
    KeyTestItem(QSGItem *parent=0) : QSGItem(parent), mKey(0) {}

protected:
    void keyPressEvent(QKeyEvent *e) {
        keyPressPreHandler(e);
        if (e->isAccepted())
            return;

        mKey = e->key();

        if (e->key() == Qt::Key_A)
            e->accept();
        else
            e->ignore();

        if (!e->isAccepted())
            QSGItem::keyPressEvent(e);
    }

    void keyReleaseEvent(QKeyEvent *e) {
        keyReleasePreHandler(e);

        if (e->isAccepted())
            return;

        if (e->key() == Qt::Key_B)
            e->accept();
        else
            e->ignore();

        if (!e->isAccepted())
            QSGItem::keyReleaseEvent(e);
    }

public:
    int mKey;
};

QML_DECLARE_TYPE(KeyTestItem);


tst_QSGItem::tst_QSGItem()
{
}

void tst_QSGItem::initTestCase()
{
    qmlRegisterType<KeyTestItem>("Test",1,0,"KeyTestItem");
}

void tst_QSGItem::keys()
{
    QSGView *canvas = new QSGView(0);
    canvas->setFixedSize(240,320);

    KeysTestObject *testObject = new KeysTestObject;
    canvas->rootContext()->setContextProperty("keysTestObject", testObject);

    canvas->rootContext()->setContextProperty("enableKeyHanding", QVariant(true));
    canvas->rootContext()->setContextProperty("forwardeeVisible", QVariant(true));

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/keystest.qml"));
    canvas->show();
    qApp->processEvents();

    QEvent wa(QEvent::WindowActivate);
    QApplication::sendEvent(canvas, &wa);
    QFocusEvent fe(QEvent::FocusIn);
    QApplication::sendEvent(canvas, &fe);

    QVERIFY(canvas->rootObject());
    QCOMPARE(canvas->rootObject()->property("isEnabled").toBool(), true);

    QKeyEvent key(QEvent::KeyPress, Qt::Key_A, Qt::NoModifier, "A", false, 1);
    QApplication::sendEvent(canvas, &key);
    QCOMPARE(testObject->mKey, int(Qt::Key_A));
    QCOMPARE(testObject->mForwardedKey, int(Qt::Key_A));
    QCOMPARE(testObject->mText, QLatin1String("A"));
    QVERIFY(testObject->mModifiers == Qt::NoModifier);
    QVERIFY(!key.isAccepted());

    testObject->reset();

    key = QKeyEvent(QEvent::KeyRelease, Qt::Key_A, Qt::ShiftModifier, "A", false, 1);
    QApplication::sendEvent(canvas, &key);
    QCOMPARE(testObject->mKey, int(Qt::Key_A));
    QCOMPARE(testObject->mForwardedKey, int(Qt::Key_A));
    QCOMPARE(testObject->mText, QLatin1String("A"));
    QVERIFY(testObject->mModifiers == Qt::ShiftModifier);
    QVERIFY(key.isAccepted());

    testObject->reset();

    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier, "", false, 1);
    QApplication::sendEvent(canvas, &key);
    QCOMPARE(testObject->mKey, int(Qt::Key_Return));
    QCOMPARE(testObject->mForwardedKey, int(Qt::Key_Return));
    QCOMPARE(testObject->mText, QLatin1String("Return"));
    QVERIFY(testObject->mModifiers == Qt::NoModifier);
    QVERIFY(key.isAccepted());

    testObject->reset();

    key = QKeyEvent(QEvent::KeyPress, Qt::Key_0, Qt::NoModifier, "0", false, 1);
    QApplication::sendEvent(canvas, &key);
    QCOMPARE(testObject->mKey, int(Qt::Key_0));
    QCOMPARE(testObject->mForwardedKey, int(Qt::Key_0));
    QCOMPARE(testObject->mText, QLatin1String("0"));
    QVERIFY(testObject->mModifiers == Qt::NoModifier);
    QVERIFY(key.isAccepted());

    testObject->reset();

    key = QKeyEvent(QEvent::KeyPress, Qt::Key_9, Qt::NoModifier, "9", false, 1);
    QApplication::sendEvent(canvas, &key);
    QCOMPARE(testObject->mKey, int(Qt::Key_9));
    QCOMPARE(testObject->mForwardedKey, int(Qt::Key_9));
    QCOMPARE(testObject->mText, QLatin1String("9"));
    QVERIFY(testObject->mModifiers == Qt::NoModifier);
    QVERIFY(!key.isAccepted());

    testObject->reset();

    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier, "", false, 1);
    QApplication::sendEvent(canvas, &key);
    QCOMPARE(testObject->mKey, int(Qt::Key_Tab));
    QCOMPARE(testObject->mForwardedKey, int(Qt::Key_Tab));
    QCOMPARE(testObject->mText, QLatin1String("Tab"));
    QVERIFY(testObject->mModifiers == Qt::NoModifier);
    QVERIFY(key.isAccepted());

    testObject->reset();

    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Backtab, Qt::NoModifier, "", false, 1);
    QApplication::sendEvent(canvas, &key);
    QCOMPARE(testObject->mKey, int(Qt::Key_Backtab));
    QCOMPARE(testObject->mForwardedKey, int(Qt::Key_Backtab));
    QCOMPARE(testObject->mText, QLatin1String("Backtab"));
    QVERIFY(testObject->mModifiers == Qt::NoModifier);
    QVERIFY(key.isAccepted());

    testObject->reset();

    canvas->rootContext()->setContextProperty("forwardeeVisible", QVariant(false));
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_A, Qt::NoModifier, "A", false, 1);
    QApplication::sendEvent(canvas, &key);
    QCOMPARE(testObject->mKey, int(Qt::Key_A));
    QCOMPARE(testObject->mForwardedKey, 0);
    QCOMPARE(testObject->mText, QLatin1String("A"));
    QVERIFY(testObject->mModifiers == Qt::NoModifier);
    QVERIFY(!key.isAccepted());

    testObject->reset();

    canvas->rootContext()->setContextProperty("enableKeyHanding", QVariant(false));
    QCOMPARE(canvas->rootObject()->property("isEnabled").toBool(), false);

    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier, "", false, 1);
    QApplication::sendEvent(canvas, &key);
    QCOMPARE(testObject->mKey, 0);
    QVERIFY(!key.isAccepted());

    canvas->rootContext()->setContextProperty("enableKeyHanding", QVariant(true));
    QCOMPARE(canvas->rootObject()->property("isEnabled").toBool(), true);

    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier, "", false, 1);
    QApplication::sendEvent(canvas, &key);
    QCOMPARE(testObject->mKey, int(Qt::Key_Return));
    QVERIFY(key.isAccepted());

    delete canvas;
    delete testObject;
}

void tst_QSGItem::keysProcessingOrder()
{
    QSGView *canvas = new QSGView(0);
    canvas->setFixedSize(240,320);

    KeysTestObject *testObject = new KeysTestObject;
    canvas->rootContext()->setContextProperty("keysTestObject", testObject);

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/keyspriority.qml"));
    canvas->show();
    qApp->processEvents();

    KeyTestItem *testItem = qobject_cast<KeyTestItem*>(canvas->rootObject());
    QVERIFY(testItem);

    QEvent wa(QEvent::WindowActivate);
    QApplication::sendEvent(canvas, &wa);
    QFocusEvent fe(QEvent::FocusIn);
    QApplication::sendEvent(canvas, &fe);

    QKeyEvent key(QEvent::KeyPress, Qt::Key_A, Qt::NoModifier, "A", false, 1);
    QApplication::sendEvent(canvas, &key);
    QCOMPARE(testObject->mKey, int(Qt::Key_A));
    QCOMPARE(testObject->mText, QLatin1String("A"));
    QVERIFY(testObject->mModifiers == Qt::NoModifier);
    QVERIFY(key.isAccepted());

    testObject->reset();

    testObject->setProcessLast(true);

    key = QKeyEvent(QEvent::KeyPress, Qt::Key_A, Qt::NoModifier, "A", false, 1);
    QApplication::sendEvent(canvas, &key);
    QCOMPARE(testObject->mKey, 0);
    QVERIFY(key.isAccepted());

    testObject->reset();

    key = QKeyEvent(QEvent::KeyPress, Qt::Key_B, Qt::NoModifier, "B", false, 1);
    QApplication::sendEvent(canvas, &key);
    QCOMPARE(testObject->mKey, int(Qt::Key_B));
    QCOMPARE(testObject->mText, QLatin1String("B"));
    QVERIFY(testObject->mModifiers == Qt::NoModifier);
    QVERIFY(!key.isAccepted());

    testObject->reset();

    key = QKeyEvent(QEvent::KeyRelease, Qt::Key_B, Qt::NoModifier, "B", false, 1);
    QApplication::sendEvent(canvas, &key);
    QCOMPARE(testObject->mKey, 0);
    QVERIFY(key.isAccepted());

    delete canvas;
    delete testObject;
}

QSGItemPrivate *childPrivate(QSGItem *rootItem, const char * itemString)
{
    QSGItem *item = findItem<QSGItem>(rootItem, QString(QLatin1String(itemString)));
    QSGItemPrivate* itemPrivate = QSGItemPrivate::get(item);
    return itemPrivate;
}

QVariant childProperty(QSGItem *rootItem, const char * itemString, const char * property)
{
    QSGItem *item = findItem<QSGItem>(rootItem, QString(QLatin1String(itemString)));
    return item->property(property);
}

bool anchorsMirrored(QSGItem *rootItem, const char * itemString)
{
    QSGItem *item = findItem<QSGItem>(rootItem, QString(QLatin1String(itemString)));
    QSGItemPrivate* itemPrivate = QSGItemPrivate::get(item);
    return itemPrivate->anchors()->mirrored();
}

void tst_QSGItem::layoutMirroring()
{
    QSGView *canvas = new QSGView(0);
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/layoutmirroring.qml"));
    canvas->show();

    QSGItem *rootItem = qobject_cast<QSGItem*>(canvas->rootObject());
    QVERIFY(rootItem);
    QSGItemPrivate *rootPrivate = QSGItemPrivate::get(rootItem);
    QVERIFY(rootPrivate);

    QCOMPARE(childPrivate(rootItem, "mirrored1")->effectiveLayoutMirror, true);
    QCOMPARE(childPrivate(rootItem, "mirrored2")->effectiveLayoutMirror, true);
    QCOMPARE(childPrivate(rootItem, "notMirrored1")->effectiveLayoutMirror, false);
    QCOMPARE(childPrivate(rootItem, "notMirrored2")->effectiveLayoutMirror, false);
    QCOMPARE(childPrivate(rootItem, "inheritedMirror1")->effectiveLayoutMirror, true);
    QCOMPARE(childPrivate(rootItem, "inheritedMirror2")->effectiveLayoutMirror, true);

    QCOMPARE(anchorsMirrored(rootItem, "mirrored1"), true);
    QCOMPARE(anchorsMirrored(rootItem, "mirrored2"), true);
    QCOMPARE(anchorsMirrored(rootItem, "notMirrored1"), false);
    QCOMPARE(anchorsMirrored(rootItem, "notMirrored2"), false);
    QCOMPARE(anchorsMirrored(rootItem, "inheritedMirror1"), true);
    QCOMPARE(anchorsMirrored(rootItem, "inheritedMirror2"), true);

    QCOMPARE(childPrivate(rootItem, "mirrored1")->inheritedLayoutMirror, true);
    QCOMPARE(childPrivate(rootItem, "mirrored2")->inheritedLayoutMirror, false);
    QCOMPARE(childPrivate(rootItem, "notMirrored1")->inheritedLayoutMirror, true);
    QCOMPARE(childPrivate(rootItem, "notMirrored2")->inheritedLayoutMirror, false);
    QCOMPARE(childPrivate(rootItem, "inheritedMirror1")->inheritedLayoutMirror, true);
    QCOMPARE(childPrivate(rootItem, "inheritedMirror2")->inheritedLayoutMirror, true);

    QCOMPARE(childPrivate(rootItem, "mirrored1")->isMirrorImplicit, false);
    QCOMPARE(childPrivate(rootItem, "mirrored2")->isMirrorImplicit, false);
    QCOMPARE(childPrivate(rootItem, "notMirrored1")->isMirrorImplicit, false);
    QCOMPARE(childPrivate(rootItem, "notMirrored2")->isMirrorImplicit, true);
    QCOMPARE(childPrivate(rootItem, "inheritedMirror1")->isMirrorImplicit, true);
    QCOMPARE(childPrivate(rootItem, "inheritedMirror2")->isMirrorImplicit, true);

    QCOMPARE(childPrivate(rootItem, "mirrored1")->inheritMirrorFromParent, true);
    QCOMPARE(childPrivate(rootItem, "mirrored2")->inheritMirrorFromParent, false);
    QCOMPARE(childPrivate(rootItem, "notMirrored1")->inheritMirrorFromParent, true);
    QCOMPARE(childPrivate(rootItem, "notMirrored2")->inheritMirrorFromParent, false);
    QCOMPARE(childPrivate(rootItem, "inheritedMirror1")->inheritMirrorFromParent, true);
    QCOMPARE(childPrivate(rootItem, "inheritedMirror2")->inheritMirrorFromParent, true);

    QCOMPARE(childPrivate(rootItem, "mirrored1")->inheritMirrorFromItem, true);
    QCOMPARE(childPrivate(rootItem, "mirrored2")->inheritMirrorFromItem, false);
    QCOMPARE(childPrivate(rootItem, "notMirrored1")->inheritMirrorFromItem, false);
    QCOMPARE(childPrivate(rootItem, "notMirrored2")->inheritMirrorFromItem, false);
    QCOMPARE(childPrivate(rootItem, "inheritedMirror1")->inheritMirrorFromItem, false);
    QCOMPARE(childPrivate(rootItem, "inheritedMirror2")->inheritMirrorFromItem, false);

    // load dynamic content using Loader that needs to inherit mirroring
    rootItem->setProperty("state", "newContent");
    QCOMPARE(childPrivate(rootItem, "notMirrored3")->effectiveLayoutMirror, false);
    QCOMPARE(childPrivate(rootItem, "inheritedMirror3")->effectiveLayoutMirror, true);

    QCOMPARE(childPrivate(rootItem, "notMirrored3")->inheritedLayoutMirror, true);
    QCOMPARE(childPrivate(rootItem, "inheritedMirror3")->inheritedLayoutMirror, true);

    QCOMPARE(childPrivate(rootItem, "notMirrored3")->isMirrorImplicit, false);
    QCOMPARE(childPrivate(rootItem, "inheritedMirror3")->isMirrorImplicit, true);

    QCOMPARE(childPrivate(rootItem, "notMirrored3")->inheritMirrorFromParent, true);
    QCOMPARE(childPrivate(rootItem, "inheritedMirror3")->inheritMirrorFromParent, true);

    QCOMPARE(childPrivate(rootItem, "notMirrored3")->inheritMirrorFromItem, false);
    QCOMPARE(childPrivate(rootItem, "notMirrored3")->inheritMirrorFromItem, false);

    // disable inheritance
    rootItem->setProperty("childrenInherit", false);

    QCOMPARE(childPrivate(rootItem, "inheritedMirror1")->effectiveLayoutMirror, false);
    QCOMPARE(childPrivate(rootItem, "inheritedMirror2")->effectiveLayoutMirror, false);
    QCOMPARE(childPrivate(rootItem, "mirrored1")->effectiveLayoutMirror, true);
    QCOMPARE(childPrivate(rootItem, "notMirrored1")->effectiveLayoutMirror, false);

    QCOMPARE(childPrivate(rootItem, "inheritedMirror1")->inheritedLayoutMirror, false);
    QCOMPARE(childPrivate(rootItem, "inheritedMirror2")->inheritedLayoutMirror, false);
    QCOMPARE(childPrivate(rootItem, "mirrored1")->inheritedLayoutMirror, false);
    QCOMPARE(childPrivate(rootItem, "notMirrored1")->inheritedLayoutMirror, false);

    // re-enable inheritance
    rootItem->setProperty("childrenInherit", true);

    QCOMPARE(childPrivate(rootItem, "inheritedMirror1")->effectiveLayoutMirror, true);
    QCOMPARE(childPrivate(rootItem, "inheritedMirror2")->effectiveLayoutMirror, true);
    QCOMPARE(childPrivate(rootItem, "mirrored1")->effectiveLayoutMirror, true);
    QCOMPARE(childPrivate(rootItem, "notMirrored1")->effectiveLayoutMirror, false);

    QCOMPARE(childPrivate(rootItem, "inheritedMirror1")->inheritedLayoutMirror, true);
    QCOMPARE(childPrivate(rootItem, "inheritedMirror2")->inheritedLayoutMirror, true);
    QCOMPARE(childPrivate(rootItem, "mirrored1")->inheritedLayoutMirror, true);
    QCOMPARE(childPrivate(rootItem, "notMirrored1")->inheritedLayoutMirror, true);
    
    //
    // dynamic parenting
    //
    QSGItem *parentItem1 = new QSGItem();
    QSGItemPrivate::get(parentItem1)->effectiveLayoutMirror = true; // LayoutMirroring.enabled: true
    QSGItemPrivate::get(parentItem1)->isMirrorImplicit = false;
    QSGItemPrivate::get(parentItem1)->inheritMirrorFromItem = true; // LayoutMirroring.childrenInherit: true
    QSGItemPrivate::get(parentItem1)->resolveLayoutMirror();

    // inherit in constructor
    QSGItem *childItem1 = new QSGItem(parentItem1);
    QCOMPARE(QSGItemPrivate::get(childItem1)->effectiveLayoutMirror, true);
    QCOMPARE(QSGItemPrivate::get(childItem1)->inheritMirrorFromParent, true);

    // inherit through a parent change
    QSGItem *childItem2 = new QSGItem();
    QCOMPARE(QSGItemPrivate::get(childItem2)->effectiveLayoutMirror, false);
    QCOMPARE(QSGItemPrivate::get(childItem2)->inheritMirrorFromParent, false);
    childItem2->setParentItem(parentItem1);
    QCOMPARE(QSGItemPrivate::get(childItem2)->effectiveLayoutMirror, true);
    QCOMPARE(QSGItemPrivate::get(childItem2)->inheritMirrorFromParent, true);

    // stop inherting through a parent change
    QSGItem *parentItem2 = new QSGItem();
    QSGItemPrivate::get(parentItem2)->effectiveLayoutMirror = true; // LayoutMirroring.enabled: true
    QSGItemPrivate::get(parentItem2)->resolveLayoutMirror();
    childItem2->setParentItem(parentItem2);
    QCOMPARE(QSGItemPrivate::get(childItem2)->effectiveLayoutMirror, false);
    QCOMPARE(QSGItemPrivate::get(childItem2)->inheritMirrorFromParent, false);
    
    delete parentItem1;
    delete parentItem2;
}

void tst_QSGItem::layoutMirroringIllegalParent()
{
    QDeclarativeComponent component(&engine);
    component.setData("import QtQuick 2.0; QtObject { LayoutMirroring.enabled: true; LayoutMirroring.childrenInherit: true }", QUrl::fromLocalFile(""));
    QTest::ignoreMessage(QtWarningMsg, "file::1:21: QML QtObject: LayoutDirection attached property only works with Items");
    QObject *object = component.create();
    QVERIFY(object != 0);
}

void tst_QSGItem::keyNavigation()
{
    QSGView *canvas = new QSGView(0);
    canvas->setFixedSize(240,320);

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/keynavigationtest.qml"));
    canvas->show();
    qApp->processEvents();

    QEvent wa(QEvent::WindowActivate);
    QApplication::sendEvent(canvas, &wa);
    QFocusEvent fe(QEvent::FocusIn);
    QApplication::sendEvent(canvas, &fe);

    QSGItem *item = findItem<QSGItem>(canvas->rootObject(), "item1");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    QVariant result;
    QVERIFY(QMetaObject::invokeMethod(canvas->rootObject(), "verify",
            Q_RETURN_ARG(QVariant, result)));
    QVERIFY(result.toBool());

    // right
    QKeyEvent key(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier, "", false, 1);
    QApplication::sendEvent(canvas, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QSGItem>(canvas->rootObject(), "item2");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // down
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier, "", false, 1);
    QApplication::sendEvent(canvas, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QSGItem>(canvas->rootObject(), "item4");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // left
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier, "", false, 1);
    QApplication::sendEvent(canvas, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QSGItem>(canvas->rootObject(), "item3");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // up
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier, "", false, 1);
    QApplication::sendEvent(canvas, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QSGItem>(canvas->rootObject(), "item1");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // tab
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier, "", false, 1);
    QApplication::sendEvent(canvas, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QSGItem>(canvas->rootObject(), "item2");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // backtab
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Backtab, Qt::NoModifier, "", false, 1);
    QApplication::sendEvent(canvas, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QSGItem>(canvas->rootObject(), "item1");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    delete canvas;
}

void tst_QSGItem::keyNavigation_RightToLeft()
{
    QSGView *canvas = new QSGView(0);
    canvas->setFixedSize(240,320);

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/keynavigationtest.qml"));
    canvas->show();
    qApp->processEvents();

    QSGItem *rootItem = qobject_cast<QSGItem*>(canvas->rootObject());
    QVERIFY(rootItem);
    QSGItemPrivate* rootItemPrivate = QSGItemPrivate::get(rootItem);

    rootItemPrivate->effectiveLayoutMirror = true; // LayoutMirroring.mirror: true
    rootItemPrivate->isMirrorImplicit = false;
    rootItemPrivate->inheritMirrorFromItem = true; // LayoutMirroring.inherit: true
    rootItemPrivate->resolveLayoutMirror();

    QEvent wa(QEvent::WindowActivate);
    QApplication::sendEvent(canvas, &wa);
    QFocusEvent fe(QEvent::FocusIn);
    QApplication::sendEvent(canvas, &fe);

    QSGItem *item = findItem<QSGItem>(canvas->rootObject(), "item1");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    QVariant result;
    QVERIFY(QMetaObject::invokeMethod(canvas->rootObject(), "verify",
            Q_RETURN_ARG(QVariant, result)));
    QVERIFY(result.toBool());

    // right
    QKeyEvent key(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier, "", false, 1);
    QApplication::sendEvent(canvas, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QSGItem>(canvas->rootObject(), "item2");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // left
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier, "", false, 1);
    QApplication::sendEvent(canvas, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QSGItem>(canvas->rootObject(), "item1");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    delete canvas;
}

void tst_QSGItem::keyNavigation_skipNotVisible()
{
    QSGView *canvas = new QSGView(0);
    canvas->setFixedSize(240,320);

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/keynavigationtest.qml"));
    canvas->show();
    qApp->processEvents();

    QEvent wa(QEvent::WindowActivate);
    QApplication::sendEvent(canvas, &wa);
    QFocusEvent fe(QEvent::FocusIn);
    QApplication::sendEvent(canvas, &fe);

    QSGItem *item = findItem<QSGItem>(canvas->rootObject(), "item1");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // Set item 2 to not visible
    item = findItem<QSGItem>(canvas->rootObject(), "item2");
    QVERIFY(item);
    item->setVisible(false);
    QVERIFY(!item->isVisible());

    // right
    QKeyEvent key(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier, "", false, 1);
    QApplication::sendEvent(canvas, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QSGItem>(canvas->rootObject(), "item1");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // tab
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier, "", false, 1);
    QApplication::sendEvent(canvas, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QSGItem>(canvas->rootObject(), "item3");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // backtab
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Backtab, Qt::NoModifier, "", false, 1);
    QApplication::sendEvent(canvas, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QSGItem>(canvas->rootObject(), "item1");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    //Set item 3 to not visible
    item = findItem<QSGItem>(canvas->rootObject(), "item3");
    QVERIFY(item);
    item->setVisible(false);
    QVERIFY(!item->isVisible());

    // tab
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier, "", false, 1);
    QApplication::sendEvent(canvas, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QSGItem>(canvas->rootObject(), "item4");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // backtab
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Backtab, Qt::NoModifier, "", false, 1);
    QApplication::sendEvent(canvas, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QSGItem>(canvas->rootObject(), "item1");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    delete canvas;
}

void tst_QSGItem::keyNavigation_implicitSetting()
{
    QSGView *canvas = new QSGView(0);
    canvas->setFixedSize(240,320);

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/keynavigationtest_implicit.qml"));
    canvas->show();
    qApp->processEvents();

    QEvent wa(QEvent::WindowActivate);
    QApplication::sendEvent(canvas, &wa);
    QFocusEvent fe(QEvent::FocusIn);
    QApplication::sendEvent(canvas, &fe);

    QSGItem *item = findItem<QSGItem>(canvas->rootObject(), "item1");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    QVariant result;
    QVERIFY(QMetaObject::invokeMethod(canvas->rootObject(), "verify",
            Q_RETURN_ARG(QVariant, result)));
    QVERIFY(result.toBool());

    // right
    QKeyEvent key(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier, "", false, 1);
    QApplication::sendEvent(canvas, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QSGItem>(canvas->rootObject(), "item2");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // back to item1
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier, "", false, 1);
    QApplication::sendEvent(canvas, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QSGItem>(canvas->rootObject(), "item1");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // down
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier, "", false, 1);
    QApplication::sendEvent(canvas, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QSGItem>(canvas->rootObject(), "item3");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // move to item4
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier, "", false, 1);
    QApplication::sendEvent(canvas, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QSGItem>(canvas->rootObject(), "item4");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // left
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier, "", false, 1);
    QApplication::sendEvent(canvas, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QSGItem>(canvas->rootObject(), "item3");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // back to item4
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier, "", false, 1);
    QApplication::sendEvent(canvas, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QSGItem>(canvas->rootObject(), "item4");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // up
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier, "", false, 1);
    QApplication::sendEvent(canvas, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QSGItem>(canvas->rootObject(), "item2");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // back to item4
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier, "", false, 1);
    QApplication::sendEvent(canvas, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QSGItem>(canvas->rootObject(), "item4");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // tab
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier, "", false, 1);
    QApplication::sendEvent(canvas, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QSGItem>(canvas->rootObject(), "item1");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // back to item4
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Backtab, Qt::NoModifier, "", false, 1);
    QApplication::sendEvent(canvas, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QSGItem>(canvas->rootObject(), "item4");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // backtab
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Backtab, Qt::NoModifier, "", false, 1);
    QApplication::sendEvent(canvas, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QSGItem>(canvas->rootObject(), "item3");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    delete canvas;
}

void tst_QSGItem::smooth()
{
    QDeclarativeComponent component(&engine);
    component.setData("import QtQuick 2.0; Item { smooth: false; }", QUrl::fromLocalFile(""));
    QSGItem *item = qobject_cast<QSGItem*>(component.create());
    QSignalSpy spy(item, SIGNAL(smoothChanged(bool)));

    QVERIFY(item);
    QVERIFY(!item->smooth());

    item->setSmooth(true);
    QVERIFY(item->smooth());
    QCOMPARE(spy.count(),1);
    QList<QVariant> arguments = spy.first();
    QVERIFY(arguments.count() == 1);
    QVERIFY(arguments.at(0).toBool() == true);

    item->setSmooth(true);
    QCOMPARE(spy.count(),1);

    item->setSmooth(false);
    QVERIFY(!item->smooth());
    QCOMPARE(spy.count(),2);
    item->setSmooth(false);
    QCOMPARE(spy.count(),2);

    delete item;
}

void tst_QSGItem::clip()
{
    QDeclarativeComponent component(&engine);
    component.setData("import QtQuick 2.0\nItem { clip: false\n }", QUrl::fromLocalFile(""));
    QSGItem *item = qobject_cast<QSGItem*>(component.create());
    QSignalSpy spy(item, SIGNAL(clipChanged(bool)));

    QVERIFY(item);
    QVERIFY(!item->clip());

    item->setClip(true);
    QVERIFY(item->clip());

    QList<QVariant> arguments = spy.first();
    QVERIFY(arguments.count() == 1);
    QVERIFY(arguments.at(0).toBool() == true);

    QCOMPARE(spy.count(),1);
    item->setClip(true);
    QCOMPARE(spy.count(),1);

    item->setClip(false);
    QVERIFY(!item->clip());
    QCOMPARE(spy.count(),2);
    item->setClip(false);
    QCOMPARE(spy.count(),2);

    delete item;
}

void tst_QSGItem::mapCoordinates()
{
    QFETCH(int, x);
    QFETCH(int, y);

    QSGView *canvas = new QSGView(0);
    canvas->setFixedSize(300, 300);
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/mapCoordinates.qml"));
    canvas->show();
    qApp->processEvents();

    QSGItem *root = qobject_cast<QSGItem*>(canvas->rootObject());
    QVERIFY(root != 0);
    QSGItem *a = findItem<QSGItem>(canvas->rootObject(), "itemA");
    QVERIFY(a != 0);
    QSGItem *b = findItem<QSGItem>(canvas->rootObject(), "itemB");
    QVERIFY(b != 0);

    QVariant result;

    QVERIFY(QMetaObject::invokeMethod(root, "mapAToB",
            Q_RETURN_ARG(QVariant, result), Q_ARG(QVariant, x), Q_ARG(QVariant, y)));
    QCOMPARE(result.value<QPointF>(), qobject_cast<QSGItem*>(a)->mapToItem(b, QPointF(x, y)));

    QVERIFY(QMetaObject::invokeMethod(root, "mapAFromB",
            Q_RETURN_ARG(QVariant, result), Q_ARG(QVariant, x), Q_ARG(QVariant, y)));
    QCOMPARE(result.value<QPointF>(), qobject_cast<QSGItem*>(a)->mapFromItem(b, QPointF(x, y)));

    QVERIFY(QMetaObject::invokeMethod(root, "mapAToNull",
            Q_RETURN_ARG(QVariant, result), Q_ARG(QVariant, x), Q_ARG(QVariant, y)));
    QCOMPARE(result.value<QPointF>(), qobject_cast<QSGItem*>(a)->mapToScene(QPointF(x, y)));

    QVERIFY(QMetaObject::invokeMethod(root, "mapAFromNull",
            Q_RETURN_ARG(QVariant, result), Q_ARG(QVariant, x), Q_ARG(QVariant, y)));
    QCOMPARE(result.value<QPointF>(), qobject_cast<QSGItem*>(a)->mapFromScene(QPointF(x, y)));

    QString warning1 = QUrl::fromLocalFile(SRCDIR "/data/mapCoordinates.qml").toString() + ":7:5: QML Item: mapToItem() given argument \"1122\" which is neither null nor an Item";
    QString warning2 = QUrl::fromLocalFile(SRCDIR "/data/mapCoordinates.qml").toString() + ":7:5: QML Item: mapFromItem() given argument \"1122\" which is neither null nor an Item";

    QTest::ignoreMessage(QtWarningMsg, qPrintable(warning1));
    QVERIFY(QMetaObject::invokeMethod(root, "checkMapAToInvalid",
            Q_RETURN_ARG(QVariant, result), Q_ARG(QVariant, x), Q_ARG(QVariant, y)));
    QVERIFY(result.toBool());

    QTest::ignoreMessage(QtWarningMsg, qPrintable(warning2));
    QVERIFY(QMetaObject::invokeMethod(root, "checkMapAFromInvalid",
            Q_RETURN_ARG(QVariant, result), Q_ARG(QVariant, x), Q_ARG(QVariant, y)));
    QVERIFY(result.toBool());

    delete canvas;
}

void tst_QSGItem::mapCoordinates_data()
{
    QTest::addColumn<int>("x");
    QTest::addColumn<int>("y");

    for (int i=-20; i<=20; i+=10)
        QTest::newRow(QTest::toString(i)) << i << i;
}

void tst_QSGItem::transforms_data()
{
    QTest::addColumn<QByteArray>("qml");
    QTest::addColumn<QMatrix>("matrix");
    QTest::newRow("translate") << QByteArray("Translate { x: 10; y: 20 }")
        << QMatrix(1,0,0,1,10,20);
    QTest::newRow("rotation") << QByteArray("Rotation { angle: 90 }")
        << QMatrix(0,1,-1,0,0,0);
    QTest::newRow("scale") << QByteArray("Scale { xScale: 1.5; yScale: -2  }")
        << QMatrix(1.5,0,0,-2,0,0);
    QTest::newRow("sequence") << QByteArray("[ Translate { x: 10; y: 20 }, Scale { xScale: 1.5; yScale: -2  } ]")
        << QMatrix(1,0,0,1,10,20) * QMatrix(1.5,0,0,-2,0,0);
}

void tst_QSGItem::transforms()
{
    QFAIL("This test has not been ported yet");
    /*QFETCH(QByteArray, qml);
    QFETCH(QMatrix, matrix);
    QDeclarativeComponent component(&engine);
    component.setData("import QtQuick 2.0\nItem { transform: "+qml+"}", QUrl::fromLocalFile(""));
    QSGItem *item = qobject_cast<QSGItem*>(component.create());
    QVERIFY(item);
    QCOMPARE(item->sceneMatrix(), matrix);*/
}

void tst_QSGItem::childrenProperty()
{
    QDeclarativeComponent component(&engine, SRCDIR "/data/childrenProperty.qml");
    
    QObject *o = component.create();
    QVERIFY(o != 0);

    QCOMPARE(o->property("test1").toBool(), true);
    QCOMPARE(o->property("test2").toBool(), true);
    QCOMPARE(o->property("test3").toBool(), true);
    QCOMPARE(o->property("test4").toBool(), true);
    QCOMPARE(o->property("test5").toBool(), true);
    delete o;
}

void tst_QSGItem::resourcesProperty()
{
    QDeclarativeComponent component(&engine, SRCDIR "/data/resourcesProperty.qml");
    
    QObject *o = component.create();
    QVERIFY(o != 0);

    QCOMPARE(o->property("test1").toBool(), true);
    QCOMPARE(o->property("test2").toBool(), true);
    QCOMPARE(o->property("test3").toBool(), true);
    QCOMPARE(o->property("test4").toBool(), true);
    QCOMPARE(o->property("test5").toBool(), true);
    delete o;
}

void tst_QSGItem::mouseFocus()
{
    QSGView *canvas = new QSGView(0);
    QVERIFY(canvas);
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/mouseFocus.qml"));
    canvas->show();
    QVERIFY(canvas->rootObject());
    QApplication::setActiveWindow(canvas);
    QTest::qWaitForWindowShown(canvas);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(canvas));

    QSGItem *item = findItem<QSGItem>(canvas->rootObject(), "declarativeItem");
    QVERIFY(item);
    QSignalSpy focusSpy(item, SIGNAL(activeFocusChanged(bool)));

    QTest::mouseClick(canvas, Qt::LeftButton, 0, item->mapToScene(QPointF(0,0)).toPoint());
    QApplication::processEvents();
    QCOMPARE(focusSpy.count(), 1);
    QVERIFY(item->hasActiveFocus());

    // make sure focusable graphics widget underneath does not steal focus
    QTest::mouseClick(canvas, Qt::LeftButton, 0, item->mapToScene(QPointF(0,0)).toPoint());
    QApplication::processEvents();
    QCOMPARE(focusSpy.count(), 1);
    QVERIFY(item->hasActiveFocus());

    item->setFocus(false);
    QVERIFY(!item->hasActiveFocus());
    QCOMPARE(focusSpy.count(), 2);
    item->setFocus(true);
    QCOMPARE(focusSpy.count(), 3);

    delete canvas;
}

void tst_QSGItem::propertyChanges()
{
    QSGView *canvas = new QSGView(0);
    canvas->setFixedSize(240,320);
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/propertychanges.qml"));
    canvas->show();

    QEvent wa(QEvent::WindowActivate);
    QApplication::sendEvent(canvas, &wa);
    QFocusEvent fe(QEvent::FocusIn);
    QApplication::sendEvent(canvas, &fe);

    QSGItem *item = findItem<QSGItem>(canvas->rootObject(), "item");
    QSGItem *parentItem = findItem<QSGItem>(canvas->rootObject(), "parentItem");

    QVERIFY(item);
    QVERIFY(parentItem);

    QSignalSpy parentSpy(item, SIGNAL(parentChanged(QSGItem *)));
    QSignalSpy widthSpy(item, SIGNAL(widthChanged()));
    QSignalSpy heightSpy(item, SIGNAL(heightChanged()));
    QSignalSpy baselineOffsetSpy(item, SIGNAL(baselineOffsetChanged(qreal)));
    QSignalSpy childrenRectSpy(parentItem, SIGNAL(childrenRectChanged(QRectF)));
    QSignalSpy focusSpy(item, SIGNAL(focusChanged(bool)));
    QSignalSpy wantsFocusSpy(parentItem, SIGNAL(activeFocusChanged(bool)));
    QSignalSpy childrenChangedSpy(parentItem, SIGNAL(childrenChanged()));
    QSignalSpy xSpy(item, SIGNAL(xChanged()));
    QSignalSpy ySpy(item, SIGNAL(yChanged()));

    item->setParentItem(parentItem);
    item->setWidth(100.0);
    item->setHeight(200.0);
    item->setFocus(true);
    item->setBaselineOffset(10.0);

    QCOMPARE(item->parentItem(), parentItem);
    QCOMPARE(parentSpy.count(),1);
    QList<QVariant> parentArguments = parentSpy.first();
    QVERIFY(parentArguments.count() == 1);
    QCOMPARE(item->parentItem(), qvariant_cast<QSGItem *>(parentArguments.at(0)));
    QCOMPARE(childrenChangedSpy.count(),1);

    item->setParentItem(parentItem);
    QCOMPARE(childrenChangedSpy.count(),1);

    QCOMPARE(item->width(), 100.0);
    QCOMPARE(widthSpy.count(),1);

    QCOMPARE(item->height(), 200.0);
    QCOMPARE(heightSpy.count(),1);

    QCOMPARE(item->baselineOffset(), 10.0);
    QCOMPARE(baselineOffsetSpy.count(),1);
    QList<QVariant> baselineOffsetArguments = baselineOffsetSpy.first();
    QVERIFY(baselineOffsetArguments.count() == 1);
    QCOMPARE(item->baselineOffset(), baselineOffsetArguments.at(0).toReal());

    QCOMPARE(parentItem->childrenRect(), QRectF(0.0,0.0,100.0,200.0));
    QCOMPARE(childrenRectSpy.count(),2);
    QList<QVariant> childrenRectArguments = childrenRectSpy.at(1);
    QVERIFY(childrenRectArguments.count() == 1);
    QCOMPARE(parentItem->childrenRect(), childrenRectArguments.at(0).toRectF());

    QCOMPARE(item->hasActiveFocus(), true);
    QCOMPARE(focusSpy.count(),1);
    QList<QVariant> focusArguments = focusSpy.first();
    QVERIFY(focusArguments.count() == 1);
    QCOMPARE(focusArguments.at(0).toBool(), true);

    QCOMPARE(parentItem->hasActiveFocus(), false);
    QCOMPARE(parentItem->hasFocus(), false);
    QCOMPARE(wantsFocusSpy.count(),0);

    item->setX(10.0);
    QCOMPARE(item->x(), 10.0);
    QCOMPARE(xSpy.count(), 1);

    item->setY(10.0);
    QCOMPARE(item->y(), 10.0);
    QCOMPARE(ySpy.count(), 1);

    delete canvas;
}

void tst_QSGItem::childrenRect()
{
    QSGView *canvas = new QSGView(0);
    canvas->setFixedSize(240,320);
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/childrenRect.qml"));
    canvas->show();

    QSGItem *o = canvas->rootObject();
    QSGItem *item = o->findChild<QSGItem*>("testItem");
    QCOMPARE(item->width(), qreal(0));
    QCOMPARE(item->height(), qreal(0));

    o->setProperty("childCount", 1);
    QCOMPARE(item->width(), qreal(10));
    QCOMPARE(item->height(), qreal(20));

    o->setProperty("childCount", 5);
    QCOMPARE(item->width(), qreal(50));
    QCOMPARE(item->height(), qreal(100));

    o->setProperty("childCount", 0);
    QCOMPARE(item->width(), qreal(0));
    QCOMPARE(item->height(), qreal(0));

    delete o;
    delete canvas;
}

// QTBUG-11383
void tst_QSGItem::childrenRectBug()
{
    QSGView *canvas = new QSGView(0);
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/childrenRectBug.qml"));
    canvas->show();

    QSGItem *o = canvas->rootObject();
    QSGItem *item = o->findChild<QSGItem*>("theItem");
    QCOMPARE(item->width(), qreal(200));
    QCOMPARE(item->height(), qreal(100));
    QCOMPARE(item->x(), qreal(100));

    delete canvas;
}

// QTBUG-11465
void tst_QSGItem::childrenRectBug2()
{
    QSGView *canvas = new QSGView(0);
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/childrenRectBug2.qml"));
    canvas->show();

    QSGRectangle *rect = qobject_cast<QSGRectangle*>(canvas->rootObject());
    QVERIFY(rect);
    QSGItem *item = rect->findChild<QSGItem*>("theItem");
    QCOMPARE(item->width(), qreal(100));
    QCOMPARE(item->height(), qreal(110));
    QCOMPARE(item->x(), qreal(130));

    QSGItemPrivate *rectPrivate = QSGItemPrivate::get(rect);
    rectPrivate->setState("row");
    QCOMPARE(item->width(), qreal(210));
    QCOMPARE(item->height(), qreal(50));
    QCOMPARE(item->x(), qreal(75));

    delete canvas;
}

// QTBUG-12722
void tst_QSGItem::childrenRectBug3()
{
    QSGView *canvas = new QSGView(0);
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/childrenRectBug3.qml"));
    canvas->show();

    //don't crash on delete
    delete canvas;
}

// QTBUG-13893
void tst_QSGItem::transformCrash()
{
    QSGView *canvas = new QSGView(0);
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/transformCrash.qml"));
    canvas->show();

    delete canvas;
}

void tst_QSGItem::implicitSize()
{
    QSGView *canvas = new QSGView(0);
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/implicitsize.qml"));
    canvas->show();

    QSGItem *item = qobject_cast<QSGItem*>(canvas->rootObject());
    QVERIFY(item);
    QCOMPARE(item->width(), qreal(80));
    QCOMPARE(item->height(), qreal(60));

    QCOMPARE(item->implicitWidth(), qreal(200));
    QCOMPARE(item->implicitHeight(), qreal(100));

    QMetaObject::invokeMethod(item, "resetSize");

    QCOMPARE(item->width(), qreal(200));
    QCOMPARE(item->height(), qreal(100));

    QMetaObject::invokeMethod(item, "changeImplicit");

    QCOMPARE(item->implicitWidth(), qreal(150));
    QCOMPARE(item->implicitHeight(), qreal(80));
    QCOMPARE(item->width(), qreal(150));
    QCOMPARE(item->height(), qreal(80));

    delete canvas;
}

void tst_QSGItem::qtbug_16871()
{
    QDeclarativeComponent component(&engine, SRCDIR "/data/qtbug_16871.qml");
    QObject *o = component.create();
    QVERIFY(o != 0);
    delete o;
}

QTEST_MAIN(tst_QSGItem)

#include "tst_qsgitem.moc"
