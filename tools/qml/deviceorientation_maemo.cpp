/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

#include "deviceorientation.h"
#include <stdio.h>
#include <stdlib.h>

class MaemoOrientation : public DeviceOrientation
{
    Q_OBJECT
public:
    MaemoOrientation()
        : DeviceOrientation(),m_current(Portrait), m_lastSeen(Portrait), m_lastSeenCount(0)
    {
        m_current = get();
        if (m_current == UnknownOrientation) 
            m_current = Portrait;

        startTimer(100);
    }

    Orientation orientation() const {
        return m_current;
    }

    void setOrientation(Orientation) { }

protected:
    virtual void timerEvent(QTimerEvent *)
    {
        Orientation c = get();

        if (c == m_lastSeen) {
            m_lastSeenCount++;
        } else {
            m_lastSeenCount = 0;
            m_lastSeen = c;
        }

        if (m_lastSeen != UnknownOrientation && m_lastSeen != m_current && m_lastSeenCount > 4) {
            m_current = m_lastSeen;
            emit orientationChanged();
            printf("%d\n", m_current);
        }
    }

signals:
    void changed();

private:
    Orientation m_current;
    Orientation m_lastSeen;
    int m_lastSeenCount;

    Orientation get()
    {
        Orientation o = UnknownOrientation;

        int ax, ay, az;

        read(&ax, &ay, &az);

        if (abs(az) > 850) {
            o = UnknownOrientation;
        } else if (ax < -750) {
            o = Portrait;
        } else if (ay < -750) {
            o = Landscape;
        }

        return o;
    }

    int read(int *ax,int *ay,int *az)
    {
        static const char *accel_filename = "/sys/class/i2c-adapter/i2c-3/3-001d/coord";

        FILE *fd;
        int rs;
        fd = fopen(accel_filename, "r");
        if(fd==NULL){ printf("liqaccel, cannot open for reading\n"); return -1;}
        rs=fscanf((FILE*) fd,"%i %i %i",ax,ay,az);
        fclose(fd);
        if(rs != 3){ printf("liqaccel, cannot read information\n"); return -2;}
        return 0;
    }
};


DeviceOrientation* DeviceOrientation::instance()
{
    static MaemoOrientation *o = 0;
    if (!o)
        o = new MaemoOrientation;
    return o;
}

#include "deviceorientation_maemo.moc"
