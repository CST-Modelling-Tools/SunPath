#include "tasks.h"

#include "common.h"

#include "SunPathLib/calculators/SunCalculatorMB.h"
#include "SunPathLib/samplers/SunSpatial.h"
#include "SunPathLib/samplers/SunTemporal.h"
#include "SunPathLib/data/FormatTMY.h"
#include "SunPathLib/data/FormatWSN.h"

using namespace sp;

Location sLocationProteas("PROTEAS", 34.707*degree, 33.261*degree, 2*3600);


void taskClearSky()
{
    SunCalculatorMB sunCalculator;
    sunCalculator.setLocation(sLocationProteas);
    SunFunctorDNI sunFunctor;

    // temporal
    SunTemporal sunTemporal(sunCalculator);

    TimeSampler timeSampler(&sunTemporal);
    QDateTime tA(QDate(2021, 1, 1), QTime(0, 00), Qt::OffsetFromUTC, sunCalculator.location().offsetUTC());
    QDateTime tB = tA.addYears(1);
    timeSampler.sample(tA, tB, QTime(0, 5));

    sunTemporal.setData(sunFunctor);

    // spatial
    SunSpatial sunSpatial(sunCalculator);

    SkySampler skySampler(&sunSpatial);
    skySampler.sample(20.*degree, 40.*degree);

    sunSpatial.setValues(sunFunctor);
    sunSpatial.setWeights(sunTemporal);

    // output
    FormatWSN formatWSN(&sunSpatial);
    ParamsWSN params;
//    params.positiveAzimuth = true;
    formatWSN.write(s_dirTemp + "ClearSky.csv", params);
}

void taskTMY()
{
    SunCalculatorMB sunCalculator;
    sunCalculator.setLocation(sLocationProteas);

    // temporal
    SunTemporal sunTemporal(sunCalculator);

    FormatTMY formatTMY(&sunTemporal);
    bool ok = formatTMY.read(s_dirData + "file.tmy");
    if (!ok) return;

    // spatial
    SunSpatial sunSpatial(sunCalculator);

    SkySampler skySampler(&sunSpatial);
    skySampler.sample(30.*degree, 90.*degree);

    sunSpatial.setWeights(sunTemporal);

    // output
    FormatWSN formatWSN(&sunSpatial);
    formatWSN.write(s_dirTemp + "ClearSky.csv");
}
