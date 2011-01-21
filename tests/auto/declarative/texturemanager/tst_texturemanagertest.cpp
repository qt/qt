/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt scene graph research project.
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

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>

#include <QtDeclarative/qsgcontext.h>
#include <QtDeclarative/qsgtexturemanager.h>

class TextureManagerTest : public QObject
{
    Q_OBJECT

public:
    TextureManagerTest();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void upload();
    void uploadSameImageTwice();

    void requestUpload();
    void requestUploadSameImageTwice();
    void requestUploadSameImageTwiceWithDelay();

    void requestUploadAfterSyncUpload();
    void uploadAfterRequestUpload();

    void gracefullyRunOutOfMemory();
    void gracefullyFailOnTooLarge();

    void deletingTextures();

    void maxTextureSize();

public slots:
    void textureStatusChanged(int newStatus) {
        status = newStatus;
    }

private:
    QSGContext *context;
    QGLWidget *glWidget;
    QSGTextureManager *tm;

    int status;
};


TextureManagerTest::TextureManagerTest()
{
}


void TextureManagerTest::initTestCase()
{
    glWidget = new QGLWidget();
    glWidget->resize(300, 200);
    glWidget->show();

    context = new QSGContext();
    context->initialize(const_cast<QGLContext *>(glWidget->context()));

    tm = context->textureManager();
}


void TextureManagerTest::cleanupTestCase()
{
    delete context;
    delete glWidget;
}


/*!
   Verify that uploads work and are synchronous and that
   size of returned texture must be at least image dims.
 */
void TextureManagerTest::upload()
{
    QImage image(100, 100, QImage::Format_ARGB32_Premultiplied);
    QSGTextureRef t = tm->upload(image);

    QVERIFY(!t.isNull());
    QVERIFY(t.isReady());
    QVERIFY(t->textureId() > 0);

    QVERIFY(t->textureSize().width() >= image.width());
    QVERIFY(t->textureSize().height() >= image.height());
}


/*!
   Verify that two requests of the same image returns the same texture.
 */
void TextureManagerTest::uploadSameImageTwice()
{
    QImage image(100, 100, QImage::Format_ARGB32_Premultiplied);
    QSGTextureRef t = tm->upload(image);

    QImage shallowCopy = image;
    QSGTextureRef t2 = tm->upload(shallowCopy);
    QCOMPARE(t.texture(), t2.texture());
}


/*!
   Requests an image to be uploaded and verifies that it actually works
 */
void TextureManagerTest::requestUpload()
{
    QImage image(100, 100, QImage::Format_ARGB32_Premultiplied);
    QSGTextureRef t = tm->requestUpload(image, 0, 0);

    QVERIFY(!t.isNull());

    QVERIFY(t->status() == QSGTexture::Ready || t->status() == QSGTexture::Loading);
    QTime time;
    time.start();
    while (t->status() == QSGTexture::Loading && time.elapsed() < 10000) {
        QTest::qWait(50);
        QApplication::processEvents();
    }

    QVERIFY(t->status() == QSGTexture::Ready);
    QVERIFY(t->textureId() > 0);
    QVERIFY(t->textureSize().width() >= image.width());
    QVERIFY(t->textureSize().height() >= image.height());
}


/*!
    Requests the same image to be uploaded twice. The second time,
    the image should be directly available. This is to verify that
    we are actually caching the uploaded requested images.
 */
void TextureManagerTest::requestUploadSameImageTwiceWithDelay()
{
    QImage image(100, 100, QImage::Format_ARGB32_Premultiplied);
    QSGTextureRef t = tm->requestUpload(image, 0, 0);
    QVERIFY(!t.isNull());

    QTime time;
    time.start();
    while (t->status() == QSGTexture::Loading && time.elapsed() < 10000) {
        QTest::qWait(50);
        QApplication::processEvents();
    }
    QVERIFY(t.isReady());

    QSGTextureRef t2 = tm->requestUpload(image, 0, 0);
    QVERIFY(t.texture() == t2.texture());
    QVERIFY(t2.isReady());
}



/*!
    Test that we don't fail horribly when allocating large amounts of
    texture memory. Since some drivers page graphics memory out to disk
    and thus never run out, cap the test at 128Mb to not run forever
 */
void TextureManagerTest::gracefullyRunOutOfMemory()
{
    QImage image(512, 512, QImage::Format_ARGB32_Premultiplied);
    QList<QSGTextureRef> refs;

    int i = 128;
    while (--i) {
        QSGTextureRef t = tm->upload(image);
        refs << t;
        if (t.isNull())
            break;
    }

    QVERIFY(true);
}


/*!
   Test that we don't crash when we try to allocate a texture that is
   too large for GL.
 */
void TextureManagerTest::gracefullyFailOnTooLarge()
{
    QImage image(32000, 2, QImage::Format_ARGB32_Premultiplied);
    QSGTextureRef t = tm->upload(image);
    QVERIFY(t.isNull());
}


void TextureManagerTest::maxTextureSize()
{
    int size = tm->maxTextureSize();

    if (size < 1024) {
        qWarning("Texture limit is only %d", size);
    }

    // 64 is the lowest allowed by the spec...
    // Any lower number would indicate an error
    QVERIFY(size >=64);
}



/*!
   Verify that two consequitive requests return the same
   texture instance so we don't upload images twice...
 */
void TextureManagerTest::requestUploadSameImageTwice()
{
    QImage image(100, 100, QImage::Format_ARGB32_Premultiplied);
    QSGTextureRef t1 = tm->requestUpload(image, 0, 0);
    QSGTextureRef t2 = tm->requestUpload(image, 0, 0);

    QVERIFY(t1.texture() == t2.texture());
}



/*!
   Verify that an async upload of a sync upload returns a ready
   texture that is the same as the sync one.
 */
void TextureManagerTest::requestUploadAfterSyncUpload()
{
    QImage image(100, 100, QImage::Format_ARGB32_Premultiplied);
    QSGTextureRef t1 = tm->upload(image);
    QSGTextureRef t2 = tm->requestUpload(image, 0, 0);

    QVERIFY(t1.texture() == t2.texture());
    QVERIFY(t2.isReady());
}



/*!
   Verify that a sync upload after an async upload returns
   a ready texture and that the async texture then also is
   ready...
 */
void TextureManagerTest::uploadAfterRequestUpload()
{
    status = QSGTexture::Null;

    QImage image(100, 100, QImage::Format_ARGB32_Premultiplied);

    QSGTextureRef async = tm->requestUpload(image, this, SLOT(textureStatusChanged(int)));
    QSGTextureRef sync = tm->upload(image);

    QVERIFY(sync.isReady());
    QVERIFY(async.isReady());

    QTest::qWait(100); // If the signal is emitted asynchronously, we need to wait for it...

    QCOMPARE(status, int(QSGTexture::Ready));

    QVERIFY(async.texture() == sync.texture());
}



void TextureManagerTest::deletingTextures()
{
    QImage image(100, 100, QImage::Format_ARGB32_Premultiplied);

    // Verify that scheduling several textures which are then removed
    // does not cause problems for a following requests.
    // - Async when async is deleted
    for (int i=0; i<100; ++i)
        QSGTextureRef async2 = tm->requestUpload(image.copy());
    QSGTextureRef async1 = tm->requestUpload(image);
    QTime time;
    time.start();
    while (!async1.isReady() && time.elapsed() < 10000) {
        QTest::qWait(50);
        QApplication::processEvents();
    }
    QVERIFY(async1.isReady());


    // Delete the uploaded texture and force upload it again
    // - sync when async is deleted
    async1 = QSGTextureRef();
    QSGTextureRef sync = tm->upload(image);
    QVERIFY(sync.isReady());


    // Delete the sync texture and upload again
    // - sync when sync is deleted
    sync = QSGTextureRef();
    sync = tm->upload(image);


    // Delete the sync texture and upload it async
    // - async when sync is deleted
    sync = QSGTextureRef();
    async1 = tm->requestUpload(image);
    time.start();
    while (!async1.isReady() && time.elapsed() < 10000) {
        QTest::qWait(50);
        QApplication::processEvents();
    }
    QVERIFY(async1.isReady());
}



QTEST_MAIN(TextureManagerTest);

#include "tst_texturemanagertest.moc"
