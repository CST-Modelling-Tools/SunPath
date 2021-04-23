#pragma once

#include "CommonLib.h"

#include <QDebug>
#include <QTextStream>
COMMONLIB extern QTextStream cout;
using Qt::endl;

#include "SunPathLib/calculators/SunCalculator.h"
#include "SunPathLib/samplers/SkySampler.h"
#include "SunPathLib/data/SkyModelPI.h"

using namespace sp;

COMMONLIB extern Location sLocationNicosia;
COMMONLIB extern Location sLocationProteas;
COMMONLIB extern QString sTimeFormat;

inline QString formatF3(double x) {return QString::number(x, 'f', 3);}
inline QString formatF6(double x) {return QString::number(x, 'f', 6);}
inline QString formatG6(double x) {return QString::number(x, 'g', 6);}
inline QString formatTime(QDateTime dt) {return dt.toString(sTimeFormat);}

COMMONLIB QString formatSunPos(const Horizontal& coords);
COMMONLIB QString formatSunPos(QDateTime dt, const Horizontal& coords);
//COMMONLIB QString formatNode(SkyNode& node, SunCalculator& calculator);
