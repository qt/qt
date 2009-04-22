#ifndef TIMEPERIOD_H
#define TIMEPERIOD_H

#include <QDateTime>
#include <QTime>
#include <QDebug>

class TimePeriod
{
public:
    TimePeriod() : m_seconds(0), m_minutes(0), m_hours(0), m_days(0), m_months(0), m_years(0)
    {
    }

    TimePeriod &setSeconds(int seconds) { m_seconds = seconds; return *this; }
    int seconds() const { return m_seconds; }

    TimePeriod &setMinutes(int minutes) { m_minutes = minutes; return *this; }
    int minutes() const { return m_minutes; }

    TimePeriod &setHours(int hours) { m_hours = hours; return *this; }
    int hours() const { return m_hours; }

    TimePeriod &setDays(int days) { m_days = days; return *this; }
    int days() const { return m_days; }

    TimePeriod &setMonths(int months) { m_months = months; return *this; }
    int months() const { return m_months; }

    TimePeriod &setYears(int years) { m_years = years; return *this; }
    int years() const { return m_years; }

    operator QVariant() const 
    {
        QVariant v;
        qVariantSetValue<TimePeriod>(v, *this);
        return v;
    }

private:
    int m_seconds;
    int m_minutes;
    int m_hours;
    int m_days;
    int m_months;
    int m_years;
};

inline void operator+=(QDateTime &dateTime, const TimePeriod &timePeriod)
{
    dateTime = dateTime.addSecs(timePeriod.seconds());
    dateTime = dateTime.addSecs(timePeriod.minutes() * 60);
    dateTime = dateTime.addSecs(timePeriod.hours() * 3600);
    dateTime = dateTime.addDays(timePeriod.days());
    dateTime = dateTime.addMonths(timePeriod.months());
    dateTime = dateTime.addYears(timePeriod.years());
}

inline QDateTime operator+(const QDateTime &dateTime, const TimePeriod &timePeriod)
{
    QDateTime result(dateTime);
    result += timePeriod;

    return result;
}

inline void operator+=(QTime &time, const TimePeriod &timePeriod)
{
    time = time.addSecs(timePeriod.seconds());
    time = time.addSecs(timePeriod.minutes() * 60);
    time = time.addSecs(timePeriod.hours() * 3600);    
}

inline QTime operator+(const QTime &time, const TimePeriod &timePeriod)
{
    QTime result(time);
    result += timePeriod;

    return time;
}

Q_DECLARE_METATYPE(TimePeriod)

#endif \\ TIMEPERIOD_H
