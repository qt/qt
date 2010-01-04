/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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

#ifndef QNETWORKSESSIONENGINE_P_H
#define QNETWORKSESSIONENGINE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <qmobilityglobal.h>
#include <QtCore/qobject.h>
#include <QtCore/qglobal.h>
#include <QtCore/qlist.h>
#include <QtCore/qstring.h>

QTM_BEGIN_NAMESPACE

class QNetworkConfigurationPrivate;
class QNetworkSessionEngine : public QObject
{
    Q_OBJECT

public:
    enum ConnectionError {
        InterfaceLookupError = 0,
        ConnectError,
        OperationNotSupported,
        DisconnectionError,
    };

    QNetworkSessionEngine(QObject *parent = 0);
    virtual ~QNetworkSessionEngine();

    virtual QList<QNetworkConfigurationPrivate *> getConfigurations(bool *ok = 0) = 0;
    virtual QString getInterfaceFromId(const QString &id) = 0;
    virtual bool hasIdentifier(const QString &id) = 0;

    //virtual QString bearerName(const QString &id) = 0;

    virtual void connectToId(const QString &id) = 0;
    virtual void disconnectFromId(const QString &id) = 0;

    virtual void requestUpdate() = 0;

Q_SIGNALS:
    void configurationsChanged();
    void connectionError(const QString &id, QNetworkSessionEngine::ConnectionError error);
};

QTM_END_NAMESPACE

#endif
