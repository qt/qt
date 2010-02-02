/*
 *
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */
#include "config.h"
#include "QWebPopup.h"
#include "PopupMenuStyle.h"

#include <QAbstractItemView>
#include <QApplication>
#include <QInputContext>
#include <QMouseEvent>
#include <QGraphicsProxyWidget>

namespace WebCore {

QWebPopup::QWebPopup(PopupMenuClient* client)
    : m_client(client)
    , m_popupVisible(false)
{
    Q_ASSERT(m_client);

#if !defined(Q_WS_S60) && !defined(Q_WS_MAEMO_5)
    setFont(m_client->menuStyle().font().font());
#endif
    connect(this, SIGNAL(activated(int)),
            SLOT(activeChanged(int)), Qt::QueuedConnection);
}


void QWebPopup::exec()
{
    // QCursor::pos() is not a great idea for a touch screen, but we don't need the coordinates
    // as comboboxes with Qt on Maemo 5 come up in their full width on the screen.
    // On the other platforms it's okay to use QCursor::pos().
#if defined(Q_WS_MAEMO_5)
    showPopup();
#else
    QMouseEvent event(QEvent::MouseButtonPress, QCursor::pos(), Qt::LeftButton,
                      Qt::LeftButton, Qt::NoModifier);
    QCoreApplication::sendEvent(this, &event);
#endif
}

void QWebPopup::showPopup()
{
    QComboBox::showPopup();
    m_popupVisible = true;
}

void QWebPopup::hidePopup()
{
    QWidget* activeFocus = QApplication::focusWidget();
    if (activeFocus && activeFocus == view()
        && activeFocus->testAttribute(Qt::WA_InputMethodEnabled)) {
        QInputContext* qic = activeFocus->inputContext();
        if (qic) {
            qic->reset();
            qic->setFocusWidget(0);
        }
    }

    QComboBox::hidePopup();
    
    if (QGraphicsProxyWidget* proxy = graphicsProxyWidget())
        proxy->setVisible(false);

    if (!m_popupVisible)
        return;

    m_popupVisible = false;
    m_client->popupDidHide();
}

void QWebPopup::activeChanged(int index)
{
    if (index < 0)
        return;

    m_client->valueChanged(index);
}

} // namespace WebCore

#include "moc_QWebPopup.cpp"
