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

#ifndef Q3GARRAY_H
#define Q3GARRAY_H

#include <Qt3Support/q3shared.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

class Q_COMPAT_EXPORT Q3GArray					// generic array
{
    friend class QBuffer;
public:
    // do not use this, even though this is public
    struct array_data : public Q3Shared {	// shared array
	array_data():data(0),len(0)
#ifdef QT_QGARRAY_SPEED_OPTIM
		    ,maxl(0)
#endif
	    {}
	char *data;				// actual array data
	uint  len;
#ifdef QT_QGARRAY_SPEED_OPTIM
	uint maxl;
#endif
    };
    Q3GArray();
    enum Optimization { MemOptim, SpeedOptim };
protected:
    Q3GArray(int, int);			// dummy; does not alloc
    Q3GArray(int size);			// allocate 'size' bytes
    Q3GArray(const Q3GArray &a);		// shallow copy
    virtual ~Q3GArray();

    Q3GArray    &operator=(const Q3GArray &a) { return assign(a); }

    virtual void detach()	{ duplicate(*this); }

    // ### Qt 4.0: maybe provide two versions of data(), at(), etc.
    char       *data()	 const	{ return shd->data; }
    uint	nrefs()	 const	{ return shd->count; }
    uint	size()	 const	{ return shd->len; }
    bool	isEqual(const Q3GArray &a) const;

    bool	resize(uint newsize, Optimization optim);
    bool	resize(uint newsize);

    bool	fill(const char *d, int len, uint sz);

    Q3GArray    &assign(const Q3GArray &a);
    Q3GArray    &assign(const char *d, uint len);
    Q3GArray    &duplicate(const Q3GArray &a);
    Q3GArray    &duplicate(const char *d, uint len);
    void	store(const char *d, uint len);

    array_data *sharedBlock()	const		{ return shd; }
    void	setSharedBlock(array_data *p) { shd=(array_data*)p; }

    Q3GArray    &setRawData(const char *d, uint len);
    void	resetRawData(const char *d, uint len);

    int		find(const char *d, uint index, uint sz) const;
    int		contains(const char *d, uint sz) const;

    void	sort(uint sz);
    int		bsearch(const char *d, uint sz) const;

    char       *at(uint index) const;

    bool	setExpand(uint index, const char *d, uint sz);

protected:
    virtual array_data *newData();
    virtual void deleteData(array_data *p);

private:
    static void msg_index(uint);
    array_data *shd;
};


inline char *Q3GArray::at(uint index) const
{
#if defined(QT_CHECK_RANGE)
    if (index >= size()) {
	msg_index(index);
	index = 0;
    }
#endif
    return &shd->data[index];
}

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3GARRAY_H
