/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#ifndef Q3VALUEVECTOR_H
#define Q3VALUEVECTOR_H

#include <QtCore/qvector.h>

#ifndef QT_NO_STL
#include <vector>
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

template <typename T>
class Q3ValueVector : public QVector<T>
{
public:
    inline Q3ValueVector() : QVector<T>() {}
    inline Q3ValueVector(const Q3ValueVector<T>& v) : QVector<T>(v) {}
    inline Q3ValueVector(typename QVector<T>::size_type n,
                         const T& val = T()) : QVector<T>(n, val) {}

#ifndef QT_NO_STL
    inline Q3ValueVector(const std::vector<T>& v) : QVector<T>()
        { this->resize(v.size()); qCopy(v.begin(), v.end(), this->begin()); }
#endif

    Q3ValueVector<T>& operator= (const Q3ValueVector<T>& v)
        { QVector<T>::operator=(v); return *this; }

#ifndef QT_NO_STL
    Q3ValueVector<T>& operator= (const std::vector<T>& v)
    {
        this->clear();
        this->resize(v.size());
        qCopy(v.begin(), v.end(), this->begin());
        return *this;
    }
#endif

    void resize(int n, const T& val = T())
    {
        if (n < this->size())
            this->erase(this->begin() + n, this->end());
        else
            this->insert(this->end(), n - this->size(), val);
    }


    T& at(int i, bool* ok = 0)
    {
        this->detach();
        if (ok)
            *ok = (i >= 0 && i < this->size());
        return *(this->begin() + i);
    }

    const T&at(int i, bool* ok = 0) const
    {
        if (ok)
            *ok = (i >= 0 && i < this->size());
        return *(this->begin() + i);
    }
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3VALUEVECTOR_H
