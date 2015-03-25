/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
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
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QWAYLANDCLIPBOARD_H
#define QWAYLANDCLIPBOARD_H

#include <QtGui/QPlatformClipboard>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

class QWaylandDisplay;
class QWaylandSelection;
class QWaylandMimeData;
struct wl_selection_offer;

class QWaylandClipboardSignalEmitter : public QObject
{
    Q_OBJECT
public slots:
    void emitChanged();
};

class QWaylandClipboard : public QPlatformClipboard
{
public:
    QWaylandClipboard(QWaylandDisplay *display);
    ~QWaylandClipboard();

    QMimeData *mimeData(QClipboard::Mode mode = QClipboard::Clipboard);
    void setMimeData(QMimeData *data, QClipboard::Mode mode = QClipboard::Clipboard);
    bool supportsMode(QClipboard::Mode mode) const;

    void unregisterSelection(QWaylandSelection *selection);

    void createSelectionOffer(uint32_t id);

    QVariant retrieveData(const QString &mimeType, QVariant::Type type) const;

private:
    static void offer(void *data,
                      struct wl_selection_offer *selection_offer,
                      const char *type);
    static void keyboardFocus(void *data,
                              struct wl_selection_offer *selection_offer,
                              struct wl_input_device *input_device);
    static const struct wl_selection_offer_listener selectionOfferListener;

    static void syncCallback(void *data);
    static void forceRoundtrip(struct wl_display *display);

    QWaylandDisplay *mDisplay;
    QWaylandMimeData *mMimeDataIn;
    QList<QWaylandSelection *> mSelections;
    QStringList mOfferedMimeTypes;
    struct wl_selection_offer *mOffer;
    QWaylandClipboardSignalEmitter mEmitter;
};

#endif // QWAYLANDCLIPBOARD_H
