/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
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
#include "../../../shared/util.h"
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <QtCore/qdir.h>
#include <QtCore/qfile.h>
#include <QtCore/qabstractitemmodel.h>
#include <QDebug>

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

// From qdeclarastivefolderlistmodel.h
const int FileNameRole = Qt::UserRole+1;
const int FilePathRole = Qt::UserRole+2;
enum SortField { Unsorted, Name, Time, Size, Type };

class tst_qdeclarativefolderlistmodel : public QObject
{
    Q_OBJECT
public:
    tst_qdeclarativefolderlistmodel() : removeStart(0), removeEnd(0) {}

public slots:
    void removed(const QModelIndex &, int start, int end) {
        removeStart = start;
        removeEnd = end;
    }

private slots:
    void basicProperties();
    void refresh();

private:
    void checkNoErrors(const QDeclarativeComponent& component);
    QDeclarativeEngine engine;

    int removeStart;
    int removeEnd;
};

void tst_qdeclarativefolderlistmodel::checkNoErrors(const QDeclarativeComponent& component)
{
    // Wait until the component is ready
    QTRY_VERIFY(component.isReady() || component.isError());

    if (component.isError()) {
        QList<QDeclarativeError> errors = component.errors();
        for (int ii = 0; ii < errors.count(); ++ii) {
            const QDeclarativeError &error = errors.at(ii);
            QByteArray errorStr = QByteArray::number(error.line()) + ":" +
                                  QByteArray::number(error.column()) + ":" +
                                  error.description().toUtf8();
            qWarning() << errorStr;
        }
    }
    QVERIFY(!component.isError());
}

void tst_qdeclarativefolderlistmodel::basicProperties()
{
    QDeclarativeComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/basic.qml"));
    checkNoErrors(component);

    QAbstractListModel *flm = qobject_cast<QAbstractListModel*>(component.create());
    QVERIFY(flm != 0);

    flm->setProperty("folder",QUrl::fromLocalFile(SRCDIR "/data"));
    QTRY_COMPARE(flm->property("count").toInt(),2); // wait for refresh
    QCOMPARE(flm->property("folder").toUrl(), QUrl::fromLocalFile(SRCDIR "/data"));
    QCOMPARE(flm->property("parentFolder").toUrl(), QUrl::fromLocalFile(SRCDIR));
    QCOMPARE(flm->property("sortField").toInt(), int(Name));
    QCOMPARE(flm->property("nameFilters").toStringList(), QStringList() << "*.qml");
    QCOMPARE(flm->property("sortReversed").toBool(), false);
    QCOMPARE(flm->property("showDirs").toBool(), true);
    QCOMPARE(flm->property("showDotAndDotDot").toBool(), false);
    QCOMPARE(flm->property("showOnlyReadable").toBool(), false);
    QCOMPARE(flm->data(flm->index(0),FileNameRole).toString(), QLatin1String("basic.qml"));
    QCOMPARE(flm->data(flm->index(1),FileNameRole).toString(), QLatin1String("dummy.qml"));    
    
    flm->setProperty("folder",QUrl::fromLocalFile(""));
    QCOMPARE(flm->property("folder").toUrl(), QUrl::fromLocalFile(""));
}

void tst_qdeclarativefolderlistmodel::refresh()
{
    QDeclarativeComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/basic.qml"));
    checkNoErrors(component);

    QAbstractListModel *flm = qobject_cast<QAbstractListModel*>(component.create());
    QVERIFY(flm != 0);

    flm->setProperty("folder",QUrl::fromLocalFile(SRCDIR "/data"));
    QTRY_COMPARE(flm->property("count").toInt(),2); // wait for refresh

    int count = flm->rowCount();

    connect(flm, SIGNAL(rowsRemoved(const QModelIndex&,int,int)),
            this, SLOT(removed(const QModelIndex&,int,int)));

    flm->setProperty("sortReversed", true);

    QCOMPARE(removeStart, 0);
    QCOMPARE(removeEnd, count-1);
}

QTEST_MAIN(tst_qdeclarativefolderlistmodel)

#include "tst_qdeclarativefolderlistmodel.moc"
