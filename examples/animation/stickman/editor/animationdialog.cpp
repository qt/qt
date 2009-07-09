/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "animationdialog.h"
#include "stickman.h"
#include "animation.h"
#include "node.h"

#include <QHBoxLayout>
#include <QStackedWidget>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QFileDialog>

AnimationDialog::AnimationDialog(StickMan *stickman, QWidget *parent)
    : QDialog(parent), m_animation(0), m_stickman(stickman)
{
    initUi();    
}

AnimationDialog::~AnimationDialog()
{
    delete m_animation;
}

void AnimationDialog::initUi() 
{
    setWindowTitle("Animation");
    setEnabled(false);

    // Second page
    m_currentFrame = new QSpinBox();
    m_totalFrames = new QSpinBox();
    m_name = new QLineEdit();

    connect(m_currentFrame, SIGNAL(valueChanged(int)), this, SLOT(currentFrameChanged(int)));
    connect(m_totalFrames, SIGNAL(valueChanged(int)), this, SLOT(totalFramesChanged(int)));
    connect(m_name, SIGNAL(textChanged(QString)), this, SLOT(setCurrentAnimationName(QString)));

    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->addWidget(new QLabel("Name:"), 0, 0, 1, 2);
    gridLayout->addWidget(m_name, 0, 2, 1, 2);
    gridLayout->addWidget(new QLabel("Frame:"), 1, 0);    
    gridLayout->addWidget(m_currentFrame, 1, 1);
    gridLayout->addWidget(new QLabel("of total # of frames: "), 1, 2);
    gridLayout->addWidget(m_totalFrames, 1, 3);
}

void AnimationDialog::initFromAnimation()
{
    m_currentFrame->setRange(0, m_animation->totalFrames()-1);
    m_currentFrame->setValue(m_animation->currentFrame());

    m_totalFrames->setRange(1, 1000);
    m_totalFrames->setValue(m_animation->totalFrames());

    m_name->setText(m_animation->name());
}

void AnimationDialog::saveAnimation()
{
    saveCurrentFrame();

    QString fileName = QFileDialog::getSaveFileName(this, "Save animation");

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly))
        m_animation->save(&file);    
}

void AnimationDialog::loadAnimation()
{
    if (maybeSave() != QMessageBox::Cancel) {
        QString fileName = QFileDialog::getOpenFileName(this, "Open animation");

        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly)) {        
            if (m_animation == 0)
                newAnimation();

            m_animation->load(&file);
            stickManFromCurrentFrame();
            initFromAnimation();
        }
    }
}

QMessageBox::StandardButton AnimationDialog::maybeSave() 
{
    if (m_animation == 0)
        return QMessageBox::No;

    QMessageBox::StandardButton button = QMessageBox::question(this, "Save?", "Do you want to save your changes?", 
                                                               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    if (button == QMessageBox::Save)
        saveAnimation();   

    return button;                 
}

void AnimationDialog::newAnimation()
{    
    if (maybeSave() != QMessageBox::Cancel) {
        setEnabled(true);
        delete m_animation;
        m_animation = new Animation();
        initFromAnimation();        
    }
}

// Gets the data from the stickman and stores it in current frame
void AnimationDialog::saveCurrentFrame()
{
    int count = m_stickman->nodeCount();
    m_animation->setNodeCount(count);
    for (int i=0; i<count; ++i) {
        QGraphicsItem *node = m_stickman->node(i);
        m_animation->setNodePos(i, node->pos());
    }
}

// Gets the data from the current frame and sets the stickman
void AnimationDialog::stickManFromCurrentFrame()
{
    int count = m_animation->nodeCount();
    for (int i=0;i<count;++i) {
        QGraphicsItem *node = m_stickman->node(i);
        node->setPos(m_animation->nodePos(i));
    }
}

void AnimationDialog::currentFrameChanged(int currentFrame)
{
    saveCurrentFrame();
    qDebug("currentFrame: %d", currentFrame);
    m_animation->setCurrentFrame(currentFrame);    
    stickManFromCurrentFrame();
    initFromAnimation();
}
 
void AnimationDialog::totalFramesChanged(int totalFrames)
{
    m_animation->setTotalFrames(totalFrames);    
    stickManFromCurrentFrame();
    initFromAnimation();
}

void AnimationDialog::setCurrentAnimationName(const QString &name)
{
    m_animation->setName(name);
}
