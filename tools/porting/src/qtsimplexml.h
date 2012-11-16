/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the qt3to4 porting application of the Qt Toolkit.
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

#ifndef QTSIMPLEXML_H
#define QTSIMPLEXML_H

#include <QString>
#include <QMultiMap>
#include <QMap>

QT_BEGIN_NAMESPACE

class QDomDocument;
class QDomElement;
class QDomNode;
class QIODevice;

class QtSimpleXml
{
public:
    QtSimpleXml(const QString &name = QString());

    QString name() const;
    QString text() const;
    int numChildren() const;
    bool isValid() const;

    const QtSimpleXml &operator [](int index) const;
    QtSimpleXml &operator [](int index);
    QtSimpleXml &operator [](const QString &key);
    QtSimpleXml &operator =(const QString &text);

    void setAttribute(const QString &key, const QString &value);
    QString attribute(const QString &key);

    bool setContent(const QString &content);
    bool setContent(QIODevice *device);
    QString errorString() const;

    QDomDocument toDomDocument() const;
    QDomElement toDomElement(QDomDocument *doc) const;
private:
    void parse(QDomNode node);

    QtSimpleXml *parent;

    QMultiMap<QString, QtSimpleXml *> children;
    QMap<QString, QString> attr;

    QString s;
    QString n;
    bool valid;

    QString errorStr;
};

QT_END_NAMESPACE

#endif
