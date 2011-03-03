/*
 * Copyright (c) 2011 Nokia Corporation.
 */

#ifndef CONTACTSDIALOG_H
#define CONTACTSDIALOG_H

#include <QtGui/QDialog>
#include <QListWidget>
#include <QPointer>

// QtMobility API headers
// Contacts
#include <QContactManager>
#include <QContactPhoneNumber>
#include <QContactSortOrder>
#include <QContact>
#include <QContactName>

// QtMobility namespace
QTM_USE_NAMESPACE

class ContactsDialog: public QDialog
{
Q_OBJECT

public:
    ContactsDialog(QWidget *parent = 0);
    ~ContactsDialog();

public slots:
    void itemClicked(QListWidgetItem *item);
    void selectContact();

signals:
    void contactSelected(QString phoneNumber);

private:
    void createContactManager();
    void searchContact();

private:
    QPointer<QContactManager> m_contactManager;
    QListWidget* m_listWidget;
};

#endif // CONTACTSDIALOG_H
