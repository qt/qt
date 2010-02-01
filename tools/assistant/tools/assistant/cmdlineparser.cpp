/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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

#include <QtCore/QFileInfo>
#include <QtGui/QMessageBox>

#include "cmdlineparser.h"

QT_BEGIN_NAMESPACE

const QString CmdLineParser::m_helpMessage = QLatin1String(
        "Usage: assistant [Options]\n\n"
        "-collectionFile file       Uses the specified collection\n"
        "                           file instead of the default one\n"
        "-showUrl url               Shows the document with the\n"
        "                           url.\n"
        "-enableRemoteControl       Enables Assistant to be\n"
        "                           remotely controlled.\n"
        "-show widget               Shows the specified dockwidget\n"
        "                           which can be \"contents\", \"index\",\n"
        "                           \"bookmarks\" or \"search\".\n"
        "-activate widget           Activates the specified dockwidget\n"
        "                           which can be \"contents\", \"index\",\n"
        "                           \"bookmarks\" or \"search\".\n"
        "-hide widget               Hides the specified dockwidget\n"
        "                           which can be \"contents\", \"index\"\n"
        "                           \"bookmarks\" or \"search\".\n"
        "-register helpFile         Registers the specified help file\n"
        "                           (.qch) in the given collection\n"
        "                           file.\n"
        "-unregister helpFile       Unregisters the specified help file\n"
        "                           (.qch) from the give collection\n"
        "                           file.\n"
        "-setCurrentFilter filter   Set the filter as the active filter.\n"
        "-remove-search-index       Removes the full text search index.\n"
        "-quiet                     Does not display any error or\n"
        "                           status message.\n"
        "-help                      Displays this help.\n"
        );


CmdLineParser::CmdLineParser(const QStringList &arguments)
    : m_pos(0),
    m_enableRemoteControl(false),
    m_contents(Untouched),
    m_index(Untouched),
    m_bookmarks(Untouched),
    m_search(Untouched),
    m_register(None),
    m_removeSearchIndex(false),
    m_quiet(false)
{
    for (int i = 1; i < arguments.count(); ++i) {
        const QString &arg = arguments.at(i);
        if (arg.toLower() == "-quiet")
            m_quiet = true;
        else
            m_arguments.append(arg);
    }
}

CmdLineParser::Result CmdLineParser::parse()
{
    bool showHelp = false;

    while (m_error.isEmpty() && hasMoreArgs()) {
        const QString &arg = nextArg().toLower();
        if (arg == QLatin1String("-collectionfile"))
            handleCollectionFileOption();
        else if (arg == QLatin1String("-showurl"))
            handleShowUrlOption();
        else if (arg == QLatin1String("-enableremotecontrol"))
            m_enableRemoteControl = true;
        else if (arg == QLatin1String("-show"))
            handleShowOption();
        else if (arg == QLatin1String("-hide"))
            handleHideOption();
        else if (arg == QLatin1String("-activate"))
            handleActivateOption();
        else if (arg == QLatin1String("-register"))
            handleRegisterOption();
        else if (arg == QLatin1String("-unregister"))
            handleUnregisterOption();
        else if (arg == QLatin1String("-setcurrentfilter"))
            handleSetCurrentFilterOption();
        else if (arg == QLatin1String("-remove-search-index"))
            m_removeSearchIndex = true;
        else if (arg == QLatin1String("-help"))
            showHelp = true;
        else
            m_error = tr("Unknown option: %1").arg(arg);
    }

    if (!m_error.isEmpty()) {
        showMessage(m_error + QLatin1String("\n\n\n") + m_helpMessage, true);
        return Error;
    } else if (showHelp) {
        showMessage(m_helpMessage, false);
        return Help;
    }
    return Ok;
}

bool CmdLineParser::hasMoreArgs() const
{
    return m_pos < m_arguments.count();
}

const QString &CmdLineParser::nextArg()
{
    Q_ASSERT(hasMoreArgs());
    return m_arguments.at(m_pos++);
}

void CmdLineParser::handleCollectionFileOption()
{
    if (hasMoreArgs()) {
        const QString &fileName = nextArg();
        m_collectionFile = getFileName(fileName);
        if (m_collectionFile.isEmpty())
            m_error = tr("The collection file '%1' does not exist!").
                          arg(fileName);
    } else {
        m_error = tr("Missing collection file!");
    }
}

void CmdLineParser::handleShowUrlOption()
{
    if (hasMoreArgs()) {
        const QString &urlString = nextArg();
        QUrl url(urlString);
        if (url.isValid()) {
            m_url = url;
        } else
            m_error = tr("Invalid URL '%1'!").arg(urlString);
    } else {
        m_error = tr("Missing URL!");
    }
}

void CmdLineParser::handleShowOption()
{
    handleShowOrHideOrActivateOption(Show);
}

void CmdLineParser::handleHideOption()
{
    handleShowOrHideOrActivateOption(Hide);
}

void CmdLineParser::handleActivateOption()
{
    handleShowOrHideOrActivateOption(Activate);
}

void CmdLineParser::handleShowOrHideOrActivateOption(ShowState state)
{
    if (hasMoreArgs()) {
        const QString &widget = nextArg().toLower();
        if (widget == QLatin1String("contents"))
            m_contents = state;
        else if (widget == QLatin1String("index"))
            m_index = state;
        else if (widget == QLatin1String("bookmarks"))
            m_bookmarks = state;
        else if (widget == QLatin1String("search"))
            m_search = state;
        else
            m_error = tr("Unknown widget: %1").arg(widget);
    } else {
        m_error = tr("Missing widget!");
    }
}

void CmdLineParser::handleRegisterOption()
{
    handleRegisterOrUnregisterOption(Register);
}

void CmdLineParser::handleUnregisterOption()
{
    handleRegisterOrUnregisterOption(Unregister);
}

void CmdLineParser::handleRegisterOrUnregisterOption(RegisterState state)
{
    if (hasMoreArgs()) {
        const QString &fileName = nextArg();
        m_helpFile = getFileName(fileName);
        if (m_helpFile.isEmpty())
            m_error = tr("The Qt help file '%1' does not exist!").arg(fileName);
        else
            m_register = state;
    } else {
        m_error = tr("Missing help file!");
    }
}

void CmdLineParser::handleSetCurrentFilterOption()
{
    if (hasMoreArgs())
        m_currentFilter = nextArg();
    else
        m_error = tr("Missing filter argument!");
}

QString CmdLineParser::getFileName(const QString &fileName)
{
    QFileInfo fi(fileName);
    if (!fi.exists())
        return QString();
    return fi.absoluteFilePath();
}

void CmdLineParser::showMessage(const QString &msg, bool error)
{
    if (m_quiet)
        return;
#ifdef Q_OS_WIN
    QString s = QLatin1String("<pre>") + msg + QLatin1String("</pre>");
    if (error)
        QMessageBox::critical(0, QObject::tr("Qt Assistant"), s);
    else
        QMessageBox::information(0, QObject::tr("Qt Assistant"), s);
#else
    fprintf(error ? stderr : stdout, "%s\n", qPrintable(msg));
#endif
}

void CmdLineParser::setCollectionFile(const QString &file)
{
    m_collectionFile = file;
}

QString CmdLineParser::collectionFile() const
{
    return m_collectionFile;
}

QUrl CmdLineParser::url() const
{
    return m_url;
}

bool CmdLineParser::enableRemoteControl() const
{
    return m_enableRemoteControl;
}

CmdLineParser::ShowState CmdLineParser::contents() const
{
    return m_contents;
}

CmdLineParser::ShowState CmdLineParser::index() const
{
    return m_index;
}

CmdLineParser::ShowState CmdLineParser::bookmarks() const
{
    return m_bookmarks;
}

CmdLineParser::ShowState CmdLineParser::search() const
{
    return m_search;
}

QString CmdLineParser::currentFilter() const
{
    return m_currentFilter;
}

bool CmdLineParser::removeSearchIndex() const
{
    return m_removeSearchIndex;
}

CmdLineParser::RegisterState CmdLineParser::registerRequest() const
{
    return m_register;
}

QString CmdLineParser::helpFile() const
{
    return m_helpFile;
}

QT_END_NAMESPACE
