#include "qjcalendar.h"

QJCalendar::QJCalendar() {}

int QJCalendar::subDate(QString dateStr, int year, int month, int day)
{
    int monthDays[12] = {0, 31, 62, 93, 124, 155, 186, 216, 246, 276, 306, 336};

    QStringList spilitedDate = dateStr.split("/");
    int originalYear = spilitedDate[0].toInt();
    int originalMonth = spilitedDate[1].toInt();
    int originalDay = spilitedDate[2].toInt();

    if (originalYear < 1 || year < 1) {
        qWarning() << "Invalid input: Years must be greater than or equal to 1.";
        return -1;
    }

    int leapYears1 = countLeapYears(1, originalYear);
    int leapYears2 = countLeapYears(1, year);

    int sumA = ((originalYear - 1) * 365) + leapYears1 + monthDays[originalMonth - 1] + originalDay;
    int sumB = ((year - 1) * 365) + leapYears2 + monthDays[month - 1] + day;

    return sumB - sumA;
}

bool QJCalendar::isLeap(int year)
{
    // following alghorithm determines the leap year accurately, for the years between 1178 to 1634.
    if (year % 33 == 1 or year % 33 == 5 or year % 33 == 9 or year % 33 == 13 or year % 33 == 17
        or year % 33 == 22 or year % 33 == 26 or year % 33 == 30) {
        return true;
    }

    return false;
}

bool QJCalendar::isValid(int jalaliYear, int jalaliMonth, int jalaliDay)
{
    if (jalaliMonth <= 6 && jalaliDay > 0 && jalaliDay <= 31) {
        return true;
    }

    if ((jalaliMonth > 6 && jalaliMonth <= 11) && (jalaliDay > 0 && jalaliDay <= 30)) {
        return true;
    }

    if (jalaliMonth == 12) {
        if ((jalaliDay > 0 && jalaliDay <= 30) || (isLeap(jalaliYear) && jalaliDay == 30))
            return true;
    }

    return false;
}

bool QJCalendar::isValid(QString jalaliDate)
{
    QStringList date = jalaliDate.split("/");
    if (date.size() != 3)
        return false;

    int y = date[0].toInt();
    int m = date[1].toInt();
    int d = date[2].toInt();

    return isValid(y, m, d);
}

QString QJCalendar::convertToJalali(int gregorianYear, int gregorianMonth, int gregorianDay)
{
    return convertToJalali(QDate(gregorianYear, gregorianMonth, gregorianDay));
}

QString QJCalendar::convertToJalali(QString dateStr)
{
    QStringList date = dateStr.split("-");
    if (date.size() != 3)
        return "";

    int y = date[0].toInt();
    int m = date[1].toInt();
    int d = date[2].toInt();

    return convertToJalali(QDate(y, m, d));
}

QString QJCalendar::convertToJalali(QDate date)
{
    if (date.isNull()) {
        qWarning() << "Invalid input: Gregorian date should be in YYYY-MM-DD format.";
        return "";
    }

    int gregorianYear = date.year();
    int gregorianMonth = date.month();
    int gregorianDay = date.day();

    long gregorianYear2 = (gregorianMonth > 2) ? (gregorianYear + 1) : gregorianYear;

    long gregorianDayOfMonth[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
    long days = 355666 + (365 * gregorianYear) + ((int) ((gregorianYear2 + 3) / 4))
                - ((int) ((gregorianYear2 + 99) / 100)) + ((int) ((gregorianYear2 + 399) / 400))
                + gregorianDay + gregorianDayOfMonth[gregorianMonth - 1];

    // Calculating Jalali year
    long jalaliYear = -1595 + (33 * ((int) (days / 12053)));
    days %= 12053;

    jalaliYear += 4 * ((int) (days / 1461));
    days %= 1461;

    if (days > 365) {
        jalaliYear += (int) ((days - 1) / 365);
        days = (days - 1) % 365;
    }

    // Calculating Jalali month and day
    int jalaliMonth, jalaliDay;
    if (days < 186) {
        jalaliMonth = 1 + (int) (days / 31);
        jalaliDay = 1 + (days % 31);
    } else {
        jalaliMonth = 7 + (int) ((days - 186) / 30);
        jalaliDay = 1 + ((days - 186) % 30);
    }

    return QString("%1/%2/%3").arg(jalaliYear).arg(jalaliMonth).arg(jalaliDay);
}

QString QJCalendar::convertToGregorianStr(QString dateStr)
{
    QDate gd = convertToGregorian(dateStr);
    QString gDateStr;
    QTextStream out(&gDateStr);
    out << gd.year() << "-" << gd.month() << "-" << gd.day();

    return gDateStr;
}

QString QJCalendar::convertToGregorianStr(int jalaliYear, int jalaliMonth, int jalaliDay)
{
    QDate gd = convertToGregorian(jalaliYear, jalaliMonth, jalaliDay);
    QString gDateStr;
    QTextStream out(&gDateStr);
    out << gd.year() << "-" << gd.month() << "-" << gd.day();

    return gDateStr;
}

QDate QJCalendar::convertToGregorian(int jalaliYear, int jalaliMonth, int jalaliDay)
{
    return convertToGregorian(QString("%1/%2/%3").arg(jalaliYear).arg(jalaliMonth).arg(jalaliDay));
}

QDate QJCalendar::convertToGregorian(QString dateStr)
{
    if (dateStr.isEmpty() || !dateStr.contains("/")) {
        qWarning() << "Invalid input: Jalali date should be in YYYY/MM/DD format.";
        return QDate();
    }

    QStringList date = dateStr.split("/");
    int jalaliYear = date[0].toInt();
    int jalaliMonth = date[1].toInt();
    int jalaliDay = date[2].toInt();

    jalaliYear += 1595;

    // Calculating days since the reference date
    long days = -355668 + (365 * jalaliYear) + (((int) (jalaliYear / 33)) * 8)
                + ((int) (((jalaliYear % 33) + 3) / 4)) + jalaliDay
                + ((jalaliMonth < 7) ? (jalaliMonth - 1) * 31 : ((jalaliMonth - 7) * 30) + 186);

    long gregorianYear = 400 * ((int) (days / 146097));

    days %= 146097;
    if (days > 36524) {
        gregorianYear += 100 * ((int) (--days / 36524));
        days %= 36524;
        if (days >= 365)
            days++;
    }

    gregorianYear += 4 * ((int) (days / 1461));
    days %= 1461;

    if (days > 365) {
        gregorianYear += (int) ((days - 1) / 365);
        days = (days - 1) % 365;
    }

    long gregorianDay = days + 1;
    long gregorianMonth;

    // Mapping Jalali days to Gregorian days
    long feb = ((gregorianYear % 4 == 0 && gregorianYear % 100 != 0) || (gregorianYear % 400 == 0))
                   ? 29
                   : 28;

    long daysCount[13] = {0, 31, feb, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    for (gregorianMonth = 0; gregorianMonth < 13 && gregorianDay > daysCount[gregorianMonth];
         gregorianMonth++)
        gregorianDay -= daysCount[gregorianMonth];

    return QDate(gregorianYear, gregorianMonth, gregorianDay);
}

int QJCalendar::countLeapYears(int startYear, int endYear)
{
    int leapYears = 0;
    for (int i = startYear; i < endYear; ++i) {
        if (isLeap(i))
            leapYears++;
    }
    return leapYears;
}
