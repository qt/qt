/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#ifndef S60MEDIARECOGNIZER_H_
#define S60MEDIARECOGNIZER_H_

#include <QtCore/qobject.h>

#include <apgcli.h>
#include <f32file.h>

QT_BEGIN_NAMESPACE

class QUrl;

class S60MediaRecognizer : public QObject
{
    Q_OBJECT

public:
    enum MediaType {
        Audio,
        Video,
        Url,
        NotSupported = -1
    };

    S60MediaRecognizer(QObject *parent = 0);
    ~S60MediaRecognizer();

    S60MediaRecognizer::MediaType mediaType(const QUrl &url);
    S60MediaRecognizer::MediaType identifyMediaType(const QString& fileName);

protected:
    TPtrC QString2TPtrC( const QString& string );

private:
    RApaLsSession m_recognizer;
    RFile m_file;
    RFs m_fileServer;
};

QT_END_NAMESPACE

#endif /* S60MEDIARECOGNIZER_H_ */
