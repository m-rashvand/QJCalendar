#ifndef JDATEPICKERWIDGET_H
#define JDATEPICKERWIDGET_H

#include <QWidget>

class QJCalendar;
class QPushButton;
class QSpinBox;
class QComboBox;

class JDatePickerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit JDatePickerWidget(QWidget *parent = nullptr);
    void moveToScreen();

private:
    QJCalendar *calendar;
    QPushButton *days[42];
    QSpinBox *year;
    QComboBox *months;
    QString curDate;

signals:
    void pickDate(QString day);

private slots:
    void curChanged();
    void pickDay();
};

#endif // JDATEPICKERWIDGET_H
