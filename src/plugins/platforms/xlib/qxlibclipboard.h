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

#ifndef QTESTLITECLIPBOARD_H
#define QTESTLITECLIPBOARD_H

#include <QPlatformClipboard>
#include "qxlibstatic.h"

class QXlibScreen;
class QXlibClipboard : public QPlatformClipboard
{
public:
    QXlibClipboard(QXlibScreen *screen);

    QMimeData *mimeData(QClipboard::Mode mode);
    void setMimeData(QMimeData *data, QClipboard::Mode mode);

    bool supportsMode(QClipboard::Mode mode) const;

    QXlibScreen *screen() const;

    Window requestor() const;
    void setRequestor(Window window);

    Window owner() const;

    void handleSelectionRequest(XEvent *event);

    bool clipboardReadProperty(Window win, Atom property, bool deleteProperty, QByteArray *buffer, int *size, Atom *type, int *format) const;
    QByteArray clipboardReadIncrementalProperty(Window win, Atom property, int nbytes, bool nullterm);

    QByteArray getDataInFormat(Atom modeAtom, Atom fmtatom);

private:
    void setOwner(Window window);

    Atom sendTargetsSelection(QMimeData *d, Window window, Atom property);
    Atom sendSelection(QMimeData *d, Atom target, Window window, Atom property);

    QXlibScreen *m_screen;

    QMimeData *m_xClipboard;
    QMimeData *m_clientClipboard;

    QMimeData *m_xSelection;
    QMimeData *m_clientSelection;

    Window m_requestor;
    Window m_owner;

    static const int clipboard_timeout;

};

#endif // QTESTLITECLIPBOARD_H
