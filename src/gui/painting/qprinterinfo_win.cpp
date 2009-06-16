/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qprinterinfo.h"

#include <qstringlist.h>

#include <windows.h>

QT_BEGIN_NAMESPACE

#ifndef QT_NO_PRINTER

extern QPrinter::PaperSize mapDevmodePaperSize(int s);

class QPrinterInfoPrivate
{
Q_DECLARE_PUBLIC(QPrinterInfo)
public:
    ~QPrinterInfoPrivate();
    QPrinterInfoPrivate();
    QPrinterInfoPrivate(const QString& name);

private:
    QString                     m_name;
    bool                        m_default;
    bool                        m_isNull;

    QPrinterInfo*               q_ptr;
};

static QPrinterInfoPrivate nullQPrinterInfoPrivate;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

QList<QPrinterInfo> QPrinterInfo::availablePrinters()
{
    QList<QPrinterInfo> printers;
    LPBYTE buffer;
    DWORD needed = 0;
    DWORD returned = 0;

    QT_WA({
            if (!EnumPrintersW(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, NULL,
                               4, 0, 0, &needed, &returned))
            {
                buffer = new BYTE[needed];
                if (!EnumPrintersW(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS , NULL,
                                   4, buffer, needed, &needed, &returned))
                {
                    delete [] buffer;
                    return printers;
                }
                PPRINTER_INFO_4 infoList = reinterpret_cast<PPRINTER_INFO_4>(buffer);
                QPrinterInfo defPrn = defaultPrinter();
                for (uint i = 0; i < returned; ++i) {
                    printers.append(QPrinterInfo(QString::fromUtf16(reinterpret_cast<const ushort*>(infoList[i].pPrinterName))));
                    if (printers.at(i).printerName() == defPrn.printerName())
                        printers[i].d_ptr->m_default = true;
                }
                delete [] buffer;
            }
        }, {
            // In Windows 98 networked printers are served through the local connection
            if (!EnumPrintersA(PRINTER_ENUM_LOCAL, NULL, 5, 0, 0, &needed, &returned)) {
                buffer = new BYTE[needed];
                if (!EnumPrintersA(PRINTER_ENUM_LOCAL, NULL, 5, buffer, needed, &needed, &returned)) {
                    delete [] buffer;
                    return printers;
                }

                PPRINTER_INFO_5 infoList = reinterpret_cast<PPRINTER_INFO_5>(buffer);
                QPrinterInfo defPrn = defaultPrinter();
                for (uint i = 0; i < returned; ++i) {
                    printers.append(QPrinterInfo(QString::fromLocal8Bit(reinterpret_cast<const char*>(infoList[i].pPrinterName))));
                    if (printers.at(i).printerName() == defPrn.printerName())
                        printers[i].d_ptr->m_default = true;
                }
                delete [] buffer;
            }
        });

    return printers;
}

QPrinterInfo QPrinterInfo::defaultPrinter()
{
    QString noPrinters(QLatin1String("qt_no_printers"));
    QString output;
    QT_WA({
        ushort buffer[256];
        GetProfileStringW(L"windows", L"device",
                          reinterpret_cast<const wchar_t *>(noPrinters.utf16()),
                          reinterpret_cast<wchar_t *>(buffer), 256);
        output = QString::fromUtf16(buffer);
    }, {
        char buffer[256];
        GetProfileStringA("windows", "device", noPrinters.toLatin1(), buffer, 256);
        output = QString::fromLocal8Bit(buffer);
    });

    // Filter out the name of the printer, which should be everything
    // before a comma.
    bool noConfiguredPrinters = (output == noPrinters);
    QStringList info = output.split(QLatin1Char(','));
    QString printerName = noConfiguredPrinters ? QString() : info.at(0);

    QPrinterInfo prn(printerName);
    prn.d_ptr->m_default = true;
    if (noConfiguredPrinters)
        prn.d_ptr->m_isNull = true;
    return prn;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

QPrinterInfo::QPrinterInfo()
{
    d_ptr = &nullQPrinterInfoPrivate;
}

QPrinterInfo::QPrinterInfo(const QString& name)
{
    d_ptr = new QPrinterInfoPrivate(name);
    d_ptr->q_ptr = this;
}

QPrinterInfo::QPrinterInfo(const QPrinterInfo& src)
{
    d_ptr = &nullQPrinterInfoPrivate;
    *this = src;
}

QPrinterInfo::QPrinterInfo(const QPrinter& prn)
{
    d_ptr = &nullQPrinterInfoPrivate;
    QList<QPrinterInfo> list = availablePrinters();
    for (int c = 0; c < list.size(); ++c) {
        if (prn.printerName() == list[c].printerName()) {
            *this = list[c];
            return;
        }
    }

    *this = QPrinterInfo();
}

QPrinterInfo::~QPrinterInfo()
{
    if (d_ptr != &nullQPrinterInfoPrivate)
        delete d_ptr;
}

QPrinterInfo& QPrinterInfo::operator=(const QPrinterInfo& src)
{
    Q_ASSERT(d_ptr);
    if (d_ptr != &nullQPrinterInfoPrivate)
        delete d_ptr;
    d_ptr = new QPrinterInfoPrivate(*src.d_ptr);
    d_ptr->q_ptr = this;
    return *this;
}

QString QPrinterInfo::printerName() const
{
    const Q_D(QPrinterInfo);
    return d->m_name;
}

bool QPrinterInfo::isNull() const
{
    const Q_D(QPrinterInfo);
    return d->m_isNull;
}

bool QPrinterInfo::isDefault() const
{
    const Q_D(QPrinterInfo);
    return d->m_default;
}

QList<QPrinter::PaperSize> QPrinterInfo::supportedPaperSizes() const
{
    const Q_D(QPrinterInfo);
    DWORD size;
    WORD* papers;
    QList<QPrinter::PaperSize> paperList;

    QT_WA({
        size = DeviceCapabilitiesW(reinterpret_cast<const WCHAR*>(d->m_name.utf16()),
                NULL, DC_PAPERS, NULL, NULL);
        if ((int)size == -1)
            return paperList;
        papers = new WORD[size];
        size = DeviceCapabilitiesW(reinterpret_cast<const WCHAR*>(d->m_name.utf16()),
                NULL, DC_PAPERS, reinterpret_cast<WCHAR*>(papers), NULL);
    }, {
        size = DeviceCapabilitiesA(d->m_name.toLatin1().data(), NULL, DC_PAPERS, NULL, NULL);
        if ((int)size == -1)
            return paperList;
        papers = new WORD[size];
        size = DeviceCapabilitiesA(d->m_name.toLatin1().data(), NULL, DC_PAPERS,
                reinterpret_cast<char*>(papers), NULL);
    });

    for (int c = 0; c < (int)size; ++c) {
        paperList.append(mapDevmodePaperSize(papers[c]));
    }

    delete [] papers;

    return paperList;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

QPrinterInfoPrivate::QPrinterInfoPrivate() :
    m_default(false),
    m_isNull(true),
    q_ptr(NULL)
{
}

QPrinterInfoPrivate::QPrinterInfoPrivate(const QString& name) :
    m_name(name),
    m_default(false),
    m_isNull(false),
    q_ptr(NULL)
{
}

QPrinterInfoPrivate::~QPrinterInfoPrivate()
{
}

#endif // QT_NO_PRINTER

QT_END_NAMESPACE
