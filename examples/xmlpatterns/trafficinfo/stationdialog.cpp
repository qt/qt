/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "stationdialog.h"
#include "ui_stationdialog.h"

#include <QtCore/QAbstractListModel>

class StationModel : public QAbstractListModel
{
    public:
        enum Role
        {
            StationIdRole = Qt::UserRole + 1,
            StationNameRole
        };

        StationModel(QObject *parent = 0)
            : QAbstractListModel(parent)
        {
        }

        void setStations(const StationInformation::List &list)
        {
            m_stations = list;
            layoutChanged();
        }

        virtual int rowCount(const QModelIndex &parent = QModelIndex()) const
        {
            if (!parent.isValid())
                return m_stations.count();
            else
                return 0;
        }

        virtual int columnCount(const QModelIndex &parent = QModelIndex()) const
        {
            if (!parent.isValid())
                return 1;
            else
                return 0;
        }

        virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const
        {
            if (!index.isValid())
                return QVariant();

            if (index.column() > 1 || index.row() >= m_stations.count())
                return QVariant();

            const StationInformation info = m_stations.at(index.row());
            if (role == Qt::DisplayRole || role == StationNameRole)
                return info.name();
            else if (role == StationIdRole)
                return info.id();

            return QVariant();
        }

    private:
        StationInformation::List m_stations;
};

StationDialog::StationDialog(const QString &name, const QStringList &lineNumbers, QWidget *parent)
    : QDialog(parent)
{
    m_ui.setupUi(this);

    connect(m_ui.m_searchButton, SIGNAL(clicked()), this, SLOT(searchStations()));

    m_ui.m_searchButton->setDefault(true);
    m_ui.m_input->setText(name);

    m_model = new StationModel(this);
    m_ui.m_view->setModel(m_model);

    for (int i = 0; i < lineNumbers.count(); ++i) {
        if (i == 0)
            m_ui.m_line1->setText(lineNumbers.at(i));
        else if (i == 1)
            m_ui.m_line2->setText(lineNumbers.at(i));
        else if (i == 2)
            m_ui.m_line3->setText(lineNumbers.at(i));
        else if (i == 3)
            m_ui.m_line4->setText(lineNumbers.at(i));
    }

    QMetaObject::invokeMethod(this, SLOT(searchStations()), Qt::QueuedConnection);
}

StationInformation StationDialog::selectedStation() const
{
    const QModelIndex index = m_ui.m_view->currentIndex();

    if (!index.isValid())
        return StationInformation();

    return StationInformation(index.data(StationModel::StationIdRole).toString(),
                              index.data(StationModel::StationNameRole).toString());
}

QStringList StationDialog::lineNumbers() const
{
    QStringList lines;

    if (!m_ui.m_line1->text().simplified().isEmpty())
        lines.append(m_ui.m_line1->text().simplified());
    if (!m_ui.m_line2->text().simplified().isEmpty())
        lines.append(m_ui.m_line2->text().simplified());
    if (!m_ui.m_line3->text().simplified().isEmpty())
        lines.append(m_ui.m_line3->text().simplified());
    if (!m_ui.m_line4->text().simplified().isEmpty())
        lines.append(m_ui.m_line4->text().simplified());

    return lines;
}

void StationDialog::searchStations()
{
    m_model->setStations(StationQuery::query(m_ui.m_input->text()));
    m_ui.m_view->keyboardSearch(m_ui.m_input->text());
}
