#include "jdatepickerwidget.h"

#include <QApplication>
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QScreen>
#include <QSpinBox>

#include "../qjcalendar.h"

JDatePickerWidget::JDatePickerWidget(QWidget *parent)
    : QWidget{parent}
{
    QGridLayout *gridLayout = new QGridLayout(this);
    setWindowTitle(tr("Jalali Date Picker"));
    calendar = new QJCalendar;

    QStringList weekNames = {"شنبه", "یکشنبه", "دوشنبه", "سه شنبه", "چهارشنبه", "پنجشنبه", "جمعه"};

    setLayoutDirection(Qt::RightToLeft);

    for (int i = 0; i < 7; i++) {
        QLabel *weekNameLabel = new QLabel(this);

        weekNameLabel->setAlignment(Qt::AlignCenter);
        weekNameLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        weekNameLabel->setText(weekNames[i]);
        gridLayout->addWidget(weekNameLabel, 1, i, 1, 1);

        if (i == 6) {
            weekNameLabel->setObjectName("weekend");
        }
    }

    for (int i = 0; i < 42; i++) {
        days[i] = new QPushButton(this);
        days[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        days[i]->setFlat(true);
        days[i]->setText("");
        days[i]->hide();
        days[i]->setCheckable(true);
        days[i]->setAutoExclusive(true);
        connect(days[i], &QPushButton::clicked, this, &JDatePickerWidget::pickDay);
        gridLayout->addWidget(days[i], qCeil(i / 7) + 2, i % 7, 1, 1);
    }

    year = new QSpinBox(this);
    year->setMinimum(1000);
    year->setMaximum(1600);
    gridLayout->addWidget(year, 0, 4, 1, 2);

    months = new QComboBox(this);
    months->setLayoutDirection(Qt::RightToLeft);
    gridLayout->addWidget(months, 0, 1, 1, 2);

    months->clear();
    months->insertItems(0,
                        {"فروردین",
                         "اردیبهشت",
                         "خرداد",
                         "تیر",
                         "اَمرداد",
                         "شهریور",
                         "مهر",
                         "آبان",
                         "آذر",
                         "دی",
                         "بهمن",
                         "اسفند"});

    QStringList currentJalaliDate = calendar->convertToJalali(QDate::currentDate()).split("/");

    this->year->setValue(currentJalaliDate[0].toInt());
    this->months->setCurrentIndex(currentJalaliDate[1].toInt() - 1);

    curChanged();
    int m = calendar->subDate("1300/1/6", year->value(), months->currentIndex() + 1, 1) % 7;
    days[currentJalaliDate[2].toInt() + m - 1]->setChecked(true);

    connect(months, &QComboBox::currentIndexChanged, this, &JDatePickerWidget::curChanged);
    connect(year, &QSpinBox::valueChanged, this, &JDatePickerWidget::curChanged);
    resize(460, 340);
}

// reposition the widget if it renders outside screen boundaries
void JDatePickerWidget::moveToScreen()
{
    QRect desktopRect = QGuiApplication::primaryScreen()->availableGeometry();
    QRect windowRect = this->geometry();

    if (!desktopRect.contains(windowRect)) {
        int newX = qBound(desktopRect.left(),
                          windowRect.left(),
                          desktopRect.right() - windowRect.width());
        int newY = qBound(desktopRect.top(),
                          windowRect.top(),
                          desktopRect.bottom() - windowRect.height());

        this->move(newX, newY);
    }
}

void JDatePickerWidget::curChanged()
{
    int daysCount[12] = {31, 31, 31, 31, 31, 31, 30, 30, 30, 30, 30, 30};
    for (int i = 0; i < 42; i++) {
        days[i]->setText("");
        days[i]->hide();
    }

    int m = calendar->subDate("1300/1/6", year->value(), months->currentIndex() + 1, 1) % 7;
    if (m < 0)
        m = m + 7;

    for (int i = 1; i < daysCount[months->currentIndex()] + 1; i++) {
        days[i + m - 1]->show();
        days[i + m - 1]->setText(QString::number(i));

        if (months->currentIndex() == 11) {
            if (!calendar->isLeap(year->value()))
                days[30 + m - 1]->hide();
        }
    }
}

void JDatePickerWidget::pickDay()
{
    QString day;
    for (int i = 0; i < 42; i++) {
        if (days[i]->isChecked()) {
            day = days[i]->text();
            curDate = QString::number(year->value()) + "/"
                      + QString::number(months->currentIndex() + 1) + "/" + day;
            emit this->pickDate(curDate);
            break;
        }
    }
}
