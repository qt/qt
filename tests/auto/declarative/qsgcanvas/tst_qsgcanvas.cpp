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
#include <QDebug>

#include "qsgitem.h"
#include "qsgcanvas.h"
#include "../../../shared/util.h"

class ConstantUpdateItem : public QSGItem
{
Q_OBJECT
public:
    ConstantUpdateItem(QSGItem *parent = 0) : QSGItem(parent), iterations(0) {setFlag(ItemHasContents);}

    int iterations;
protected:
    Node* updatePaintNode(Node *, UpdatePaintNodeData *){
        iterations++;
        update();
        return 0;
    }
};

class tst_qsgcanvas : public QObject
{
    Q_OBJECT
public:
    tst_qsgcanvas();

private slots:
    void initTestCase();
    void cleanupTestCase();

    void constantUpdates();
};

tst_qsgcanvas::tst_qsgcanvas()
{
}

void tst_qsgcanvas::initTestCase()
{
}

void tst_qsgcanvas::cleanupTestCase()
{
}

//If the item calls update inside updatePaintNode, it should schedule another update
void tst_qsgcanvas::constantUpdates()
{
    QSGCanvas canvas;
    ConstantUpdateItem item(canvas.rootItem());
    canvas.show();
    QTRY_VERIFY(item.iterations > 60);
}

QTEST_MAIN(tst_qsgcanvas)

#include "tst_qsgcanvas.moc"
