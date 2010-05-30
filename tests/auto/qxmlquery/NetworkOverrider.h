/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef NetworkOverrider_h
#define NetworkOverrider_h

#include <QtNetwork/QNetworkAccessManager>

/*!
 \class MessageSilencer
 \internal
 \since 4.5
 \brief A message handler for QXmlQuery that simply discards the messages, such
 that they aren't printed to \c stderr.
 */
class NetworkOverrider : public QNetworkAccessManager
{
public:
    NetworkOverrider(const QUrl &rewriteFrom,
                     const QUrl &rewriteTo);

    virtual QNetworkReply *createRequest(Operation op,
                                         const QNetworkRequest &req,
                                         QIODevice *outgoingData);

private:
    const QUrl m_rewriteFrom;
    const QUrl m_rewriteTo;
};

NetworkOverrider::NetworkOverrider(const QUrl &rewriteFrom,
                                   const QUrl &rewriteTo) : m_rewriteFrom(rewriteFrom)
                                                          , m_rewriteTo(rewriteTo)
{
    Q_ASSERT(m_rewriteFrom.isValid());
    Q_ASSERT(m_rewriteTo.isValid());
}

QNetworkReply *NetworkOverrider::createRequest(Operation op,
                                               const QNetworkRequest &req,
                                               QIODevice *outgoingData)
{
    QNetworkRequest newReq(req);

    if(req.url() == m_rewriteFrom)
        newReq.setUrl(m_rewriteTo);

    return QNetworkAccessManager::createRequest(op, newReq, outgoingData);
}
#endif
