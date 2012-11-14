/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef DEMO_ITEM_H
#define DEMO_ITEM_H

#include <QtGui>

class DemoItemAnimation;
class Guide;

class SharedImage
{
public:
    SharedImage() : refCount(0), image(0), pixmap(0){};
    ~SharedImage()
    {
        delete image;
        delete pixmap;
    }

    int refCount;
    QImage *image;
    QPixmap *pixmap;
    QMatrix matrix;
    QRectF unscaledBoundingRect;
};

class DemoItem : public QGraphicsItem
{

public:
    DemoItem(QGraphicsScene *scene = 0, QGraphicsItem *parent = 0);
    virtual ~DemoItem();

    bool inTransition();
    virtual void animationStarted(int id = 0){ Q_UNUSED(id); };
    virtual void animationStopped(int id = 0){ Q_UNUSED(id); };
    virtual void prepare(){};
    void setRecursiveVisible(bool visible);
    void useSharedImage(const QString &hashKey);
    void setNeverVisible(bool never = true);
    static void setMatrix(const QMatrix &matrix);
    virtual QRectF boundingRect() const; // overridden
    void setPosUsingSheepDog(const QPointF &dest, const QRectF &sceneFence);

    qreal opacity;
    bool locked;
    DemoItemAnimation *currentAnimation;
    bool noSubPixeling;

    // Used if controlled by a guide:
    void useGuide(Guide *guide, float startFrame = 0);
    void guideAdvance(float distance);
    void guideMove(float moveSpeed);
    void setGuidedPos(const QPointF &position);
    QPointF getGuidedPos();
    float startFrame;
    float guideFrame;
    Guide *currGuide;

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option = 0, QWidget *widget = 0); // overridden
    virtual QImage *createImage(const QMatrix &) const { return 0; };
    virtual bool collidesWithItem(const QGraphicsItem *, Qt::ItemSelectionMode) const { return false; };
    bool prepared;

private:
    SharedImage *sharedImage;
    QString hashKey;
    bool neverVisible;
    bool validateImage();

    // Used if controlled by a guide:
    void switchGuide(Guide *guide);
    friend class Guide;
    QPointF guidedPos;

    // The next static hash is shared amongst all demo items, and
    // has the purpose of reusing images to save memory and time
    static QHash<QString, SharedImage *> sharedImageHash;
    static QMatrix matrix;
};

#endif // DEMO_ITEM_H

