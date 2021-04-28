#include "Dialog.h"
#include "ui_Dialog.h"

#include <SunPathLib/calculators/SunCalculatorMB.h>
#include <SunPathLib/calculators/Location.h>




Dialog::Dialog(QWidget* parent):
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    QDate date = ui->dateEdit->date();
    ui->calendarWidget->setSelectedDate(date);
}

Dialog::~Dialog()
{
    delete ui;
}

int parseOffsetUTC(QString text, bool* ok = 0)
{
    QStringList list = text.split(":");
    int hours = list[0].toInt(ok);
    int minutes = list[1].toInt(ok);
    if (hours < 0) minutes *= -1;
    return hours*3600 + minutes*60;
}

void Dialog::on_pushCompute_clicked()
{
    using namespace sp;

    SunCalculatorMB sunMB;

    double latitude = ui->spinLatitude->value();
    double longitude = ui->spinLongitude->value();
    int offset = parseOffsetUTC(ui->editUTC->text());
    Location location("User", latitude*degree, longitude*degree, offset);
    sunMB.setLocation(location);

    QDate date = ui->dateEdit->date();
    QTime time = ui->timeEdit->time();
    QDateTime dt = sunMB.getLocalTime(date, time);
    Horizontal hc = sunMB.findHorizontalV(dt);

    QString temp;
    temp = QString::number(hc.azimuth()/degree, 'f', 4);
    ui->editAzimuth->setText(temp);
    temp = QString::number(hc.elevation()/degree, 'f', 4);
    ui->editElevation->setText(temp);
}
