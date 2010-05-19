/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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
#include "tracer.h"

#include "contentwindow.h"
#include "centralwidget.h"
#include "helpenginewrapper.h"
#include "helpviewer.h"

#include <QtGui/QLayout>
#include <QtGui/QFocusEvent>
#include <QtGui/QMenu>

#include <QtHelp/QHelpContentWidget>

QT_BEGIN_NAMESPACE

ContentWindow::ContentWindow()
    : m_contentWidget(HelpEngineWrapper::instance().contentWidget())
    , m_expandDepth(-2)
{
    TRACE_OBJ
    m_contentWidget->viewport()->installEventFilter(this);
    m_contentWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(4);
    layout->addWidget(m_contentWidget);

    connect(m_contentWidget, SIGNAL(customContextMenuRequested(QPoint)), this,
        SLOT(showContextMenu(QPoint)));
    connect(m_contentWidget, SIGNAL(linkActivated(QUrl)), this,
        SIGNAL(linkActivated(QUrl)));

    QHelpContentModel *contentModel =
        qobject_cast<QHelpContentModel*>(m_contentWidget->model());
    connect(contentModel, SIGNAL(contentsCreated()), this, SLOT(expandTOC()));
}

ContentWindow::~ContentWindow()
{
    TRACE_OBJ
}

bool ContentWindow::syncToContent(const QUrl& url)
{
    TRACE_OBJ
    QModelIndex idx = m_contentWidget->indexOf(url);
    if (!idx.isValid())
        return false;
    m_contentWidget->setCurrentIndex(idx);
    return true;
}

void ContentWindow::expandTOC()
{
    TRACE_OBJ
    Q_ASSERT(m_expandDepth >= -2);
    if (m_expandDepth > -2) {
        expandToDepth(m_expandDepth);
        m_expandDepth = -2;
    }
}

void ContentWindow::expandToDepth(int depth)
{
    TRACE_OBJ
    Q_ASSERT(depth >= -2);
    m_expandDepth = depth;
    if (depth == -1)
        m_contentWidget->expandAll();
    else if (depth == 0)
        m_contentWidget->collapseAll();
    else
        m_contentWidget->expandToDepth(depth - 1);
}

void ContentWindow::focusInEvent(QFocusEvent *e)
{
    TRACE_OBJ
    if (e->reason() != Qt::MouseFocusReason)
        m_contentWidget->setFocus();
}

void ContentWindow::keyPressEvent(QKeyEvent *e)
{
    TRACE_OBJ
    if (e->key() == Qt::Key_Escape)
        emit escapePressed();
}

bool ContentWindow::eventFilter(QObject *o, QEvent *e)
{
    TRACE_OBJ
    if (m_contentWidget && o == m_contentWidget->viewport()
        && e->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *me = static_cast<QMouseEvent*>(e);
        const QModelIndex &index = m_contentWidget->indexAt(me->pos());
        if (!index.isValid())
            return QWidget::eventFilter(o, e);

        const Qt::MouseButtons button = me->button();
        QItemSelectionModel *sm = m_contentWidget->selectionModel();
        if (sm->isSelected(index)) {
            if ((button == Qt::LeftButton && (me->modifiers() & Qt::ControlModifier))
                || (button == Qt::MidButton)) {
                QHelpContentModel *contentModel =
                    qobject_cast<QHelpContentModel*>(m_contentWidget->model());
                if (contentModel) {
                    QHelpContentItem *itm = contentModel->contentItemAt(index);
                    if (itm && AbstractHelpViewer::canOpenPage(itm->url().path()))
                        CentralWidget::instance()->setSourceInNewTab(itm->url());
                }
            } else if (button == Qt::LeftButton) {
                itemClicked(index);
            }
        }
    }
    return QWidget::eventFilter(o, e);
}


void ContentWindow::showContextMenu(const QPoint &pos)
{
    TRACE_OBJ
    if (!m_contentWidget->indexAt(pos).isValid())
        return;

    QHelpContentModel *contentModel =
        qobject_cast<QHelpContentModel*>(m_contentWidget->model());
    QHelpContentItem *itm =
        contentModel->contentItemAt(m_contentWidget->currentIndex());

    QMenu menu;
    QAction *curTab = menu.addAction(tr("Open Link"));
    QAction *newTab = menu.addAction(tr("Open Link in New Tab"));
    if (!AbstractHelpViewer::canOpenPage(itm->url().path()))
        newTab->setEnabled(false);
    
    menu.move(m_contentWidget->mapToGlobal(pos));

    QAction *action = menu.exec();
    if (curTab == action)
        emit linkActivated(itm->url());
    else if (newTab == action)
        CentralWidget::instance()->setSourceInNewTab(itm->url());
}

void ContentWindow::itemClicked(const QModelIndex &index)
{
    TRACE_OBJ
    QHelpContentModel *contentModel =
        qobject_cast<QHelpContentModel*>(m_contentWidget->model());

    if (contentModel) {
        if (QHelpContentItem *itm = contentModel->contentItemAt(index)) {
            const QUrl &url = itm->url();
            if (url != CentralWidget::instance()->currentSource())
                emit linkActivated(url);
        }
    }
}

QT_END_NAMESPACE
