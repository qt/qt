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

#include <qtest.h>

#include <QFile>
#include <QSvgRenderer>

//TESTED_FILES=

class tst_QSvgRenderer : public QObject
{
    Q_OBJECT

public:
    tst_QSvgRenderer();
    virtual ~tst_QSvgRenderer();

public slots:
    void init();
    void cleanup();

private slots:
    void construct();
    void load();
};

tst_QSvgRenderer::tst_QSvgRenderer()
{
}

tst_QSvgRenderer::~tst_QSvgRenderer()
{
}

void tst_QSvgRenderer::init()
{
}

void tst_QSvgRenderer::cleanup()
{
}

void tst_QSvgRenderer::construct()
{
    QBENCHMARK {
        QSvgRenderer renderer;
    }
}

void tst_QSvgRenderer::load()
{
    QFile file(":/data/tiger.svg");
    if (!file.open(QFile::ReadOnly))
        QFAIL("Can not open tiger.svg");
    QByteArray data = file.readAll();
    QSvgRenderer renderer;

    QBENCHMARK {
        renderer.load(data);
    }
}

QTEST_MAIN(tst_QSvgRenderer)
#include "tst_qsvgrenderer.moc"
