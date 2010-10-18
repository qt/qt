/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <qtest.h>
#include <QDeclarativeEngine>
#include <QDeclarativeComponent>
#include <private/qdeclarativeimage_p.h>

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

class tst_qmlgraphicsimage : public QObject
{
    Q_OBJECT
public:
    tst_qmlgraphicsimage() {}

private slots:
    void qmlgraphicsimage();
    void qmlgraphicsimage_file();
    void qmlgraphicsimage_url();

private:
    QDeclarativeEngine engine;
};

void tst_qmlgraphicsimage::qmlgraphicsimage()
{
    int x = 0;
    QUrl url(SRCDIR "/image.png");
    QBENCHMARK {
        QUrl url2("http://localhost/image" + QString::number(x++) + ".png");
        QDeclarativeImage *image = new QDeclarativeImage;
        QDeclarativeEngine::setContextForObject(image, engine.rootContext());
        delete image;
    }
}

void tst_qmlgraphicsimage::qmlgraphicsimage_file()
{
    int x = 0;
    QUrl url(SRCDIR "/image.png");
    //get rid of initialization effects
    {
        QDeclarativeImage *image = new QDeclarativeImage;
        QDeclarativeEngine::setContextForObject(image, engine.rootContext());
        image->setSource(url);
    }
    QBENCHMARK {
        QUrl url2("http://localhost/image" + QString::number(x++) + ".png");
        QDeclarativeImage *image = new QDeclarativeImage;
        QDeclarativeEngine::setContextForObject(image, engine.rootContext());
        image->setSource(url);
        delete image;
    }
}

void tst_qmlgraphicsimage::qmlgraphicsimage_url()
{
    int x = 0;
    QUrl url(SRCDIR "/image.png");
    QBENCHMARK {
        QUrl url2("http://localhost/image" + QString::number(x++) + ".png");
        QDeclarativeImage *image = new QDeclarativeImage;
        QDeclarativeEngine::setContextForObject(image, engine.rootContext());
        image->setSource(url2);
        delete image;
    }
}

QTEST_MAIN(tst_qmlgraphicsimage)

#include "tst_qdeclarativeimage.moc"
