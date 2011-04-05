/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#include "declarativeinputcontext.h"

#include "inputcontextmodule.h"
#include "inputcontextfilter.h"

#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

DeclarativeInputContext::DeclarativeInputContext(QObject *parent)
    : QInputContext(parent)
    , m_module(0)
{
}

DeclarativeInputContext::~DeclarativeInputContext()
{
}

bool DeclarativeInputContext::isComposing() const
{
    return m_module && !m_module->preeditText().isEmpty();
}

QString DeclarativeInputContext::identifierName()
{
    return QLatin1String("Qt.labs.inputcontext/1.0");
}

QString DeclarativeInputContext::language()
{
    return QString();
}

void DeclarativeInputContext::setFocusWidget(QWidget *widget)
{
    QInputContext::setFocusWidget(widget);

    if (m_module)
        m_module->setFocusWidget(widget);
}

void DeclarativeInputContext::mouseHandler(int x, QMouseEvent *event)
{
    if (!m_mouseHandlers.isEmpty()) {
        InputContextMouseEvent me(*event);
        foreach (InputContextMouseHandler *handler, m_mouseHandlers) {
            handler->processEvent(event->type(), x, &me);
            if (me.isAccepted()) {
                event->setAccepted(true);
                return;
            }
        }
    }
}

bool DeclarativeInputContext::filterMouseEvent(const QMouseEvent *event)
{
    if (!m_mouseFilters.isEmpty()) {
        InputContextMouseEvent me(*event);
        foreach (InputContextMouseFilter *filter, m_mouseFilters) {
            filter->processEvent(event->type(), &me);
            if (me.isAccepted())
                return true;
        }
    }

    return false;
}

bool DeclarativeInputContext::filterKeyEvent(const QKeyEvent *event)
{
    if (!m_keyFilters.isEmpty()) {
        InputContextKeyEvent ke(*event);
        foreach (InputContextKeyFilter *filter, m_keyFilters) {
            filter->processEvent(event->type(), &ke);
            if (ke.isAccepted())
                return true;
        }
    }
    return false;
}

bool DeclarativeInputContext::filterEvent(const QEvent *event)
{
    switch (event->type()) {
    case QEvent::RequestSoftwareInputPanel:
        if (m_module)
            m_module->setVisible(true);
        return true;
    case QEvent::CloseSoftwareInputPanel:
        if (m_module)
            m_module->setVisible(false);
        return true;
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseMove:
        return filterMouseEvent(static_cast<const QMouseEvent *>(event));
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
        return filterKeyEvent(static_cast<const QKeyEvent *>(event));
    default:
        return false;
    }
}

void DeclarativeInputContext::reset()
{
    if (m_module)
        m_module->commit();
}

void DeclarativeInputContext::update()
{
    if (m_module)
        m_module->update();
}

void DeclarativeInputContext::setModule(InputContextModule *module)
{
    m_module = module;
}

void DeclarativeInputContext::registerMouseHandler(InputContextMouseHandler *handler)
{
    connect(handler, SIGNAL(destroyed(QObject*)), this, SLOT(mouseHandlerDestroyed(QObject*)));
    m_mouseHandlers.append(handler);
}

void DeclarativeInputContext::registerMouseFilter(InputContextMouseFilter *filter)
{
    connect(filter, SIGNAL(destroyed(QObject*)), this, SLOT(mouseFilterDestroyed(QObject*)));
    m_mouseFilters.append(filter);
}

void DeclarativeInputContext::registerKeyFilter(InputContextKeyFilter *filter)
{
    connect(filter, SIGNAL(destroyed(QObject*)), this, SLOT(keyFilterDestroyed(QObject*)));
    m_keyFilters.append(filter);
}

void DeclarativeInputContext::mouseHandlerDestroyed(QObject *handler)
{
    m_mouseHandlers.removeAll(static_cast<InputContextMouseHandler *>(handler));
}

void DeclarativeInputContext::mouseFilterDestroyed(QObject *filter)
{
    m_mouseFilters.removeAll(static_cast<InputContextMouseFilter *>(filter));
}

void DeclarativeInputContext::keyFilterDestroyed(QObject *filter)
{
    m_keyFilters.removeAll(static_cast<InputContextKeyFilter *>(filter));
}

QT_END_NAMESPACE
