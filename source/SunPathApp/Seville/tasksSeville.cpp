#include "tasksSeville.h"

#include "common.h"

#include "SunPathLib/calculators/SunCalculatorMB.h"
#include "SunPathLib/samplers/SunSpatial.h"
#include "SunPathLib/samplers/SunTemporal.h"
#include "SunPathLib/data/FormatTMY.h"
#include "SunPathLib/data/FormatWSN.h"

using namespace sp;

#include "FormatSeville.h"

Location sLocationPS10("PS10", 37.43*degree, -6.25*degree, 1*3600);
Location sLocationSeville("Seville", (37.+ 24./60 + 42./3600)*degree, -(6. + 0./60 + 21./3600)*degree, 0*1*3600);


void taskSeville()
{
    SunCalculatorMB sunCalculator;
    sunCalculator.setLocation(sLocationSeville);
    bool ok;

    // temporal
    SunTemporal sunTemporal(sunCalculator);
    FormatSeville formatSeville(&sunTemporal);
    ok = formatSeville.read(s_dirData + "Seville/2016_Seville_1hor.csv");
//    ok = formatSeville.read(s_dirData + "Seville/2016_Seville_10min.csv");
//    ok = formatSeville.read(s_dirData + "Seville/2016_Seville_1min.csv");
    if (!ok) return;

    if (!true) { // check import
        formatSeville.write(s_dirTemp + "Seville.csv");
    }


    // spatial
    SunSpatial sunSpatial(sunCalculator);
    SkySampler skySampler(&sunSpatial);
    skySampler.sample(20.*degree, 40.*degree);

    sunSpatial.setWeights(sunTemporal);

    if (!true) { // integral
        SunFunctorHeliostatField sunFunctor;
        sunFunctor.t = vec3d::directionAE(180*degree, 30*degree);
        sunSpatial.setValues(sunFunctor);

        double integralSampled = sunSpatial.integrate();
        double integralDirect = sunTemporal.integrateWeighted(sunFunctor);
        double diff = 100*fabs((integralSampled - integralDirect)/integralDirect);
        cout << QString("Integral sampled: %1\n").arg(integralSampled);
        cout << QString("Integral direct: %1\n").arg(integralDirect);
        cout << QString("Difference: %1%\n").arg(diff);
        cout << endl;
    }

    if (!true) { // reconstruct spatial profile
        sunSpatial.setValues(sunTemporal);

        // init another sunTemporal to check


//        double integralSampled = sunSpatial.integrate(); // bad
//        double integralDirect = sunTemporal.integrate();
//        double diff = 100*fabs((integralSampled - integralDirect)/integralDirect);
//        cout << QString("Integral sampled: %1\n").arg(integralSampled);
//        cout << QString("Integral direct: %1\n").arg(integralDirect);
//        cout << QString("Difference: %1%\n").arg(diff);
//        cout << endl;
    }

    if (true) { // save weights
        FormatWSN formatWSN(&sunSpatial);
        formatWSN.write(s_dirTemp + "SevilleWeights.csv");
    }
}

void taskSevillePast()
{
    SunCalculatorMB sunCalculator;
    sunCalculator.setLocation(sLocationSeville);

    // temporal
    SunTemporal sunTemporal(sunCalculator);

    FormatSeville formatSeville(&sunTemporal);
    ParamsSeville params;
    params.seconds = false;
    bool ok;

    params.offset = 0;
    ok = formatSeville.read(s_dirData + "Seville/2016_Seville_1hor.csv", params);
//    params.offset = 10*60;
//    ok = formatSeville.read(s_dirData + "Seville/2016_Seville_10min.csv", params);
//    params.offset = 60;
//    ok = formatSeville.read(s_dirData + "Seville/2016_Seville_1min.csv", params);
    if (!ok) return;

    formatSeville.write(s_dirTemp + "tmy_60min.csv");
}

void taskSevilleto1M()
{
    SunCalculatorMB sunCalculator;
    sunCalculator.setLocation(sLocationSeville);

    // temporal
    SunTemporal sunTemporal(sunCalculator);
    FormatTMY formatTMY(&sunTemporal);
    ParamsTMY params;
    params.precision = 3;
    params.adjustDay = true;
    bool ok = formatTMY.read(s_dirData + "Seville/tmy_60min.csv", params);
//    bool ok = formatTMY.read(s_dirData + "Seville/tmy_10min.csv");
    if (!ok) return;


    if (!true) { // check import
        formatTMY.write(s_dirTemp + "Seville.csv", params);
    }

    SunTemporal sunTemporal2(sunCalculator);
    TimeSampler timeSampler(&sunTemporal2);
    QDateTime tA = sunCalculator.getLocalTime(QDate(2016, 1, 1), QTime(0, 00));
    QDateTime tB = tA.addDays(365);
    timeSampler.sample(tA, tB, QTime(0, 1));

    QVector<double> vals;
    for (int n = 1; n < sunTemporal2.timeStamps().size(); ++n) {
        QDateTime tA = sunTemporal2.timeStamps()[n - 1].t;
        QDateTime tB = sunTemporal2.timeStamps()[n].t;
        vals << sunTemporal.average(tA, tB);
    }
    sunTemporal2.setData(vals);
//    TimeSampler timeSampler(&sunTemporal);
//    timeSampler.resample(QTime(0, 1), params.adjustDay);

    FormatTMY formatX(&sunTemporal2);
    formatX.write(s_dirTemp + "SevilleRectmy.csv", params);
}

void taskSevilleWeights()
{
    SunCalculatorMB sunCalculator;
    sunCalculator.setLocation(sLocationSeville);

    // temporal
    SunTemporal sunTemporal(sunCalculator);
    FormatTMY formatTMY(&sunTemporal);
    bool ok = formatTMY.read(s_dirData + "Seville/tmy_01min.csv");
//    bool ok = formatTMY.read(s_dirData + "Seville/tmy_01min_from_10min.csv");
//    bool ok = formatTMY.read(s_dirData + "Seville/tmy_01min_from_60min.csv");
//    bool ok = formatTMY.read(s_dirData + "Seville/tmy_01min_from_60min_adj.csv");
    if (!ok) return;

    SunSpatial sunSpatial(sunCalculator);
    SkySampler skySampler(&sunSpatial);
    skySampler.sample(20*degree, 40.*degree);

    sunSpatial.setWeights(sunTemporal);
//    sunSpatial.setValues({

//    });

    FormatWSN formatWSN(&sunSpatial);
    formatWSN.write(s_dirTemp + "SevilleWeights.csv");
}
