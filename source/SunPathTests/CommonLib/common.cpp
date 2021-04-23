#include "common.h"

QTextStream cout(stdout);
Location sLocationNicosia("Nicosia", 35.1588*degree, 33.4214*degree, 2*3600);
Location sLocationProteas("Proteas", 34.707*degree, 33.261*degree, 2*3600);
QString sTimeFormat("d MMMM yyyy, h:mm:ss");


QString formatSunPos(const Horizontal& coords)
{
    return QString("Azimuth: %1 deg, Elevation: %2 deg").arg(
        formatF3(coords.azimuth()/degree),
        formatF3(coords.elevation()/degree)
    );
}

QString formatSunPos(QDateTime dt, const Horizontal& coords)
{
    return QString("%1 (%2)").arg(
        formatTime(dt),
        formatSunPos(coords)
    );
}

//QString formatNode(SkyNode& node, SunCalculator& calculator)
//{
//    calculator.setVector(node.v);
//    return QString("%1, Function: %2").arg(
//        formatSunPos(calculator.findHorizontal()),
//        formatG6(node.f)
//    );
//}
