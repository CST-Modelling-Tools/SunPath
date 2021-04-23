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
//    ok = formatSeville.read(sDirData + "Seville/2016_Seville_1hor.csv");
    ok = formatSeville.read(sDirData + "Seville/2016_Seville_1min.csv");
    if (!ok) return;

    if (!true) { // check import
        formatSeville.write(sDirTemp + "Seville.csv");
    }

    // spatial
    SunSpatial sunSpatial(sunCalculator);
    SkySampler skySampler(&sunSpatial);
    skySampler.sample(20.*degree, 40.*degree);

    sunSpatial.setWeights(sunTemporal);

    if (!true) { // integral
        SunFunctorHeliostatField2 sunFunctor;
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

    if (true) { // reconstruct spatial profile
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
        formatWSN.write(sDirTemp + "SevilleWeights.csv");
    }
}
