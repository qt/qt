/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QtTest/QtTest>

#include <QtMediaServices/qvideodevicecontrol.h>
#include <QtMediaServices/qmediacontrol.h>
#include <QtMediaServices/qmediaservice.h>

#include <QtGui/qapplication.h>
#include <QtGui/qstyle.h>

class QtTestMediaService;


class tst_QMediaService : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void control_iid();
    void control();
};


class QtTestMediaControlA : public QMediaControl
{
    Q_OBJECT
};


#define QtTestMediaControlA_iid "com.nokia.QtTestMediaControlA"
QT_BEGIN_NAMESPACE
Q_MEDIA_DECLARE_CONTROL(QtTestMediaControlA, QtTestMediaControlA_iid)
QT_END_NAMESPACE


class QtTestMediaControlB : public QMediaControl
{
    Q_OBJECT
};

#define QtTestMediaControlB_iid "com.nokia.QtTestMediaControlB"
QT_BEGIN_NAMESPACE
Q_MEDIA_DECLARE_CONTROL(QtTestMediaControlB, QtTestMediaControlB_iid)
QT_END_NAMESPACE


class QtTestMediaControlC : public QMediaControl
{
    Q_OBJECT
};

#define QtTestMediaControlC_iid "com.nokia.QtTestMediaControlC"
QT_BEGIN_NAMESPACE
Q_MEDIA_DECLARE_CONTROL(QtTestMediaControlC, QtTestMediaControlA_iid) // Yes A.
QT_END_NAMESPACE

class QtTestMediaControlD : public QMediaControl
{
    Q_OBJECT
};

#define QtTestMediaControlD_iid "com.nokia.QtTestMediaControlD"
QT_BEGIN_NAMESPACE
Q_MEDIA_DECLARE_CONTROL(QtTestMediaControlD, QtTestMediaControlD_iid)
QT_END_NAMESPACE

class QtTestMediaControlE : public QMediaControl
{
    Q_OBJECT
};

struct QtTestDevice
{
    QtTestDevice() {}
    QtTestDevice(const QString &name, const QString &description, const QIcon &icon)
        : name(name), description(description), icon(icon)
    {
    }

    QString name;
    QString description;
    QIcon icon;
};

class QtTestVideoDeviceControl : public QVideoDeviceControl
{
public:
    QtTestVideoDeviceControl(QObject *parent = 0)
        : QVideoDeviceControl(parent)
        , m_selectedDevice(-1)
        , m_defaultDevice(-1)
    {
    }

    int deviceCount() const { return devices.count(); }

    QString deviceName(int index) const { return devices.value(index).name; }
    QString deviceDescription(int index) const { return devices.value(index).description; }
    QIcon deviceIcon(int index) const { return devices.value(index).icon; }

    int defaultDevice() const { return m_defaultDevice; }
    void setDefaultDevice(int index) { m_defaultDevice = index; }

    int selectedDevice() const { return m_selectedDevice; }
    void setSelectedDevice(int index)
    {
        emit selectedDeviceChanged(m_selectedDevice = index);
        emit selectedDeviceChanged(devices.value(index).name);
    }

    QList<QtTestDevice> devices;

private:
    int m_selectedDevice;
    int m_defaultDevice;
};

class QtTestMediaService : public QMediaService
{
    Q_OBJECT
public:
    QtTestMediaService()
        : QMediaService(0)
        , hasDeviceControls(false)
    {
    }

    QMediaControl* control(const char *name) const
    {
        if (strcmp(name, QtTestMediaControlA_iid) == 0)
            return const_cast<QtTestMediaControlA *>(&controlA);
        else if (strcmp(name, QtTestMediaControlB_iid) == 0)
            return const_cast<QtTestMediaControlB *>(&controlB);
        else if (strcmp(name, QtTestMediaControlC_iid) == 0)
            return const_cast<QtTestMediaControlC *>(&controlC);
        else if (hasDeviceControls && strcmp(name, QVideoDeviceControl_iid) == 0)
            return const_cast<QtTestVideoDeviceControl *>(&videoDeviceControl);
        else
            return 0;
    }

    using QMediaService::control;

    QtTestMediaControlA controlA;
    QtTestMediaControlB controlB;
    QtTestMediaControlC controlC;
    QtTestVideoDeviceControl videoDeviceControl;
    bool hasDeviceControls;
};

void tst_QMediaService::initTestCase()
{    
}

void tst_QMediaService::control_iid()
{
    const char *nullString = 0;

    // Default implementation.
    QCOMPARE(qmediacontrol_iid<QtTestMediaControlE *>(), nullString);

    // Partial template.
    QVERIFY(qstrcmp(qmediacontrol_iid<QtTestMediaControlA *>(), QtTestMediaControlA_iid) == 0);
}

void tst_QMediaService::control()
{
    QtTestMediaService service;

    QCOMPARE(service.control<QtTestMediaControlA *>(), &service.controlA);
    QCOMPARE(service.control<QtTestMediaControlB *>(), &service.controlB);
    QVERIFY(!service.control<QtTestMediaControlC *>());  // Faulty implementation returns A.
    QVERIFY(!service.control<QtTestMediaControlD *>());  // No control of that type.
}

QTEST_MAIN(tst_QMediaService)

#include "tst_qmediaservice.moc"
