/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <QtDeclarative/qmlview.h>
#include <private/qmlgraphicsrectangle_p.h>
#include <private/qmlgraphicsimage_p.h>
#include <private/qmlgraphicsanimatedimageitem_p.h>

class tst_animatedimage : public QObject
{
    Q_OBJECT
public:
    tst_animatedimage() {}

private slots:
    void play();
    void pause();
    void setFrame();
    void frameCount();
};

void tst_animatedimage::play()
{
    QmlGraphicsAnimatedImageItem anim;
    anim.setSource(QUrl("file://" SRCDIR "/data/stickman.gif"));
    QVERIFY(anim.isPlaying());
}

void tst_animatedimage::pause()
{
    QmlGraphicsAnimatedImageItem anim;
    anim.setSource(QUrl("file://" SRCDIR "/data/stickman.gif"));
    anim.setPaused(true);
    QVERIFY(!anim.isPlaying());
}

void tst_animatedimage::setFrame()
{
    QmlGraphicsAnimatedImageItem anim;
    anim.setSource(QUrl("file://" SRCDIR "/data/stickman.gif"));
    anim.setPaused(true);
    QVERIFY(!anim.isPlaying());
    anim.setCurrentFrame(2);
    QCOMPARE(anim.currentFrame(), 2);
}

void tst_animatedimage::frameCount()
{
    QmlGraphicsAnimatedImageItem anim;
    anim.setSource(QUrl("file://" SRCDIR "/data/stickman.gif"));
    QCOMPARE(anim.frameCount(), 299);
}

QTEST_MAIN(tst_animatedimage)

#include "tst_animatedimage.moc"
