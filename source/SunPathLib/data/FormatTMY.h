#pragma once

#include "SunPathLib/samplers/SunTemporal.h"

namespace sp {


struct ParamsTMY {
    ParamsTMY(bool seconds = false, int offset = 0, int precision = 0):
        seconds(seconds), offset(offset), precision(precision) {}

    bool seconds; // write seconds
    int offset; // in seconds
    int precision; // precision for function
};


class SUNPATHLIB FormatTMY
{
public:
    FormatTMY(SunTemporal* sunTemporal);
    SunTemporal* sunTemporal() {return m_sunTemporal;}

    bool read(QString fileName, const ParamsTMY& params = ParamsTMY());
    bool write(QString fileName, const ParamsTMY& params = ParamsTMY());
    QString message() const {return m_message;}

protected:
    void readInfo(QTextStream& fin, const ParamsTMY& params);
    void readData(QTextStream& fin, const ParamsTMY& params);

protected:
    SunTemporal* m_sunTemporal;
    QString m_message;
};


} // namespace sp


/* usage

    Location location(34.707*degree, 33.261*degree);
    SunCalculatorMB sunCalculator(location);

    // temporal
    SunTemporal sunTemporal(&sunCalculator);

    FormatTMY formatTMY(&sunTemporal);
    bool ok = formatTMY.read("file.tmy");
    if (!ok) cout << formatTMY.message();

    TimeSampler timeSampler(&sunTemporal);
    timeSampler.sample(QDateTime(2021, 1, 1), QDateTime(2022, 1, 1), QTime(1, 0), true)

    SunFunctorDNI sunFunctor;
    sunTemporal.setData(&sunFunctor);

    // spatial
    SunSpatial sunSpatial(&sunCalculator);

    SkySampler skySampler(&sunSpatial);
    skySampler.sample(15.*degree, 90.*degree)

    SunFunctorHeliostatFild sunFunctor;
    sunFunctor.t = vec3d::directionAE(180*degree, 30*degree);
    sunFunctor.apply(&sunSpatial);

    FormatWSN formatWSN(&sunSpatial);
    bool ok = formatWSN.write("file.wsn");
    if (!ok) cout << formatWSN.message();
*/
