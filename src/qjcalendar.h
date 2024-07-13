#ifndef QJCALENDAR_H
#define QJCALENDAR_H

#include <QDate>
#include <QString>

class QJCalendar
{
public:
    QJCalendar();
    Q_INVOKABLE int subDate(QString dateStr, int year, int month, int day);
    Q_INVOKABLE bool isLeap(int year);

    // gregorian to jalali
    Q_INVOKABLE QString convertToJalali(int gregorianYear, int gregorianMonth, int gregorianDay);
    Q_INVOKABLE static QString convertToJalali(QDate date);

    // jalali to gregorian
    Q_INVOKABLE QString convertToGregorianStr(int jalaliYear, int jalaliMonth, int jalaliDay);
    Q_INVOKABLE static QDate convertToGregorian(QString dateStr);

private:
    int countLeapYears(int startYear, int endYear);
};

#endif // QJCALENDAR_H
