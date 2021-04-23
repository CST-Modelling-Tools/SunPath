#include "IntegrationTest.h"
#include "CommonLib/common.h"

#include "SunPathLib/samplers/SunSpatial.h"
#include "SunPathLib/samplers/SunTemporal.h"
#include "SunPathLib/data/SunFunctor.h"
#include "SunPathLib/calculators/SunCalculatorMB.h"
#include "SunPathLib/samplers/SkySampler.h"
#include "SunPathLib/samplers/TimeSampler.h"
#include "SunPathLib/math/sampling/ErrorAnalysis.h"


void testSunIntegrator(const SunFunctor& sunFunctor, const SunFunctor& wFunctor,
    double rho, double sigma)
{
    SunCalculatorMB sunCalculator;
    sunCalculator.setLocation(sLocationProteas);

    // time
    SunTemporal sunTemporal(sunCalculator);

    TimeSampler timeSampler(&sunTemporal);
    QDateTime tA = sunCalculator.getLocalTime(QDate(2021, 1, 1), QTime(0, 00));
    QDateTime tB = tA.addYears(1);
    timeSampler.sample(tA, tB, QTime(0, 1), !true);

    sunTemporal.setData(wFunctor);

    // space
    SunSpatial sunSpatial(sunCalculator);

    SkySampler skySampler(&sunSpatial);
    skySampler.sample(rho, sigma);
    cout << QString("Sampling points: %1\n").arg(sunSpatial.skyNodes().size());

    sunSpatial.setValues(sunFunctor);
    sunSpatial.setWeights(sunTemporal);

    // report
    double integralSampled = sunSpatial.integrate();
    double integralDirect = sunTemporal.integrateWeighted(sunFunctor);
    double diff = 100*fabs((integralSampled - integralDirect)/integralDirect);
    cout << QString("Integral sampled: %1\n").arg(integralSampled);
    cout << QString("Integral direct: %1\n").arg(integralDirect);
    cout << QString("Difference: %1%\n").arg(formatF6(diff));
    cout << endl;
}

void testSunIntegratorLoop(const SunFunctor& sunFunctor, const SunFunctor& wFunctor)
{
    struct Params {
        double rho;
        double sigma;
    };

    QVector<Params> runs;
    runs << Params{30., 90.};
    runs << Params{15., 90.};
    runs << Params{20., 40.};

    for (Params& p : runs) {
        cout << QString("Resolution: %1 deg\n").arg(p.rho);
        cout << QString("Sigma: %1 deg\n").arg(p.sigma);
        testSunIntegrator(sunFunctor, wFunctor, p.rho*degree, p.sigma*degree);
    }
}

void IntegrationTest::test_HeliostatField_old()
{
    cout << "--- Test sun integration for heliostat old ---\n";
    SunFunctorHeliostatFieldOld functor;
    functor.t = vec3d::directionAE(180*degree, 30*degree);
    testSunIntegratorLoop(functor, SunFunctorDNI());
}

void IntegrationTest::test_HeliostatField()
{
    cout << "--- Test sun integration for heliostat ---\n";
    SunFunctorHeliostatField functor;
    functor.t = vec3d::directionAE(180*degree, 30*degree);
    testSunIntegratorLoop(functor, SunFunctorDNI());
}


QTEST_MAIN(IntegrationTest)
