#include "FormatSeville.h"

#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QString>

using namespace sp;

FormatSeville::FormatSeville(SunTemporal* sunTemporal):
    m_sunTemporal(sunTemporal)
{

}

void FormatSeville::setLocation(Location& location)
{
    m_sunTemporal->calculator()->setLocation(location);
}

bool FormatSeville::read(QString fileName, const ParamsSeville& params)
{
    try {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            throw QString("File not opened: ") + fileName;

        QTextStream fin(&file);
        readData(fin, params);

        m_message.clear();
        return true;
    }
    catch (const QString& message) {
        m_message = QString("Runtime error in FormatTMY::read\n") + message;
        qDebug() << m_message;
        return false;
    }
}

bool FormatSeville::write(QString fileName, const ParamsSeville& params)
{
    try {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            throw QString("File not opened: ") + fileName;

        QTextStream fout(&file);

        SunCalculator* sc = m_sunTemporal->calculator();
        fout << QString("Source,Latitude,Longitude,Time Zone\n");
        fout << QString("TMY3,%1,%2,%3\n")
            .arg(sc->location().latitude()/sp::degree, 0, 'f', 6)
            .arg(sc->location().longitude()/sp::degree, 0, 'f', 6)
            .arg(sc->location().offsetUTC()/3600., 0, 'f', 3);

        QString temp("Year,Month,Day,Hour,Minute,");
        if (params.seconds)
            temp += "Seconds,";
        if (params.writeSunpos)
            temp += "Azimuth[deg],Elevation[deg],";
        temp += "DNI[W/m2]\n";
        fout << temp;

        const QVector<TimeStamp>& timeStamps = m_sunTemporal->timeStamps();
        const QVector<double>& data = m_sunTemporal->data();
        for (int n = 1; n < timeStamps.size(); ++n) {
            QDateTime dt = timeStamps[n].t;
            QString temp;

            QDate d = dt.date();
            temp += QString::number(d.year()) + ",";
            temp += QString::number(d.month()) + ",";
            temp += QString::number(d.day()) + ",";

            QTime t = dt.time();
            temp += QString::number(t.hour()) + ",";
            temp += QString::number(t.minute()) + ",";
            if (params.seconds)
                temp += QString::number(t.second()) + ",";

            if (params.writeSunpos) {
                Horizontal hc = sc->findHorizontal(timeStamps[n].s);
                temp += QString::number(hc.azimuth()/degree, 'f', 3) + ",";
                temp += QString::number(hc.elevation()/degree, 'f', 3) + ",";
            }

            temp += QString::number(data[n - 1],'f', 3) + "\n";

            fout << temp;
        }

        m_message.clear();
        return true;
    }
    catch (const QString& message) {
        m_message = QString("Runtime error in FormatTMY::write\n") + message;
        qDebug() << m_message;
        return false;
    }
}

void FormatSeville::readData(QTextStream& fin, const ParamsSeville& params)
{
    // line 3
    QString line = fin.readLine();
    if (line.isNull()) throw QString("line 1 is missing");
    QStringList list = line.split(',');

    int iDay = -1;
    int iHour = -1;
    int iMinute = -1;
    int iSecond = -1;
    int iDNI = -1;

    for (int i = 0; i < list.size(); ++i) {
        if (list[i].contains("Day", Qt::CaseInsensitive))
            iDay = i;
        else if (list[i].contains("Hour", Qt::CaseInsensitive))
            iHour = i;
        else if (list[i].contains("Min", Qt::CaseInsensitive))
            iMinute = i;
        else if (list[i].contains("Second", Qt::CaseInsensitive))
            iSecond = i;
        else if (list[i].contains("Hbn", Qt::CaseInsensitive))
            iDNI = i;
    }

    int year = 2016; // leap year
    int day = 0;
    int hour = 0;
    int minute = 0;
    int second = 0;
    double DNI = 0.;

    bool ok;
    int offsetUTC = m_sunTemporal->calculator()->location().offsetUTC();
    QVector<QDateTime> ts;
    ts << QDateTime();
    QVector<double> ds;

    while (true) {
        QString line = fin.readLine();
        if (line.isNull()) break;
        QStringList list = line.split(',');

        day = list[iDay].toInt(&ok);
        if (!ok) throw QString("toInt ") + line;

        hour = list[iHour].toInt(&ok);
        if (!ok) throw QString("toInt ") + line;

        if (iMinute > 0) {
            minute = list[iMinute].toInt(&ok);
            if (!ok) throw QString("toInt ") + line;
        }
        if (iSecond > 0) {
            second = list[iSecond].toInt(&ok);
            if (!ok) throw QString("toInt ") + line;
        }
        DNI = list[iDNI].toDouble(&ok);
        if (!ok) throw QString("toDouble ") + line;

        // fix seville
//        if (QTime(hour, minute, second).msecsSinceStartOfDay() == 0) continue;

        QDateTime dt(QDate(year, 1, 1).addDays(day - 1), QTime(hour, minute, second), Qt::OffsetFromUTC, offsetUTC);
        if (params.offset != 0) dt = dt.addSecs(params.offset);

        ts << dt;
        ds << DNI;
    }

    int tStep = ts[1].msecsTo(ts[2]);
    ts[0] = ts[1].addMSecs(-tStep);
//    qDebug() << ts[1];
//    qDebug() << ts[0];

    TimeSampler timeSampler(m_sunTemporal);
    timeSampler.sample(ts);

    m_sunTemporal->setData(ds);
}
