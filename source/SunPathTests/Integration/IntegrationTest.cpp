#include "IntegrationTest.h"
#include "CommonLib/common.h"

#include "SunPathLib/samplers/SunSpatial.h"
#include "SunPathLib/samplers/SunTemporal.h"
#include "SunPathLib/data/SunFunctor.h"
#include "SunPathLib/calculators/SunCalculatorMB.h"
#include "SunPathLib/samplers/SkySampler.h"
#include "SunPathLib/samplers/TimeSampler.h"
#include "SunPathLib/math/sampling/ErrorAnalysis.h"


void testSunIntegrator(const SunFunctor& sunFunctor, const SunFunctor& wFunctor)
{
    SunCalculatorMB sunCalculator;
    sunCalculator.setLocation(sLocationProteas);
    sunCalculator.setVersion(SunCalculatorMB::V2020);

    SunTemporal sunTemporal(sunCalculator);
    SunSpatial sunSpatial(sunCalculator);

//    QVector<double> resolutions = {30., 15.};
    QVector<double> resolutions = {20.};
    for (double resolution : resolutions)
    {
        // time
        TimeSampler timeSampler(&sunTemporal);
        QDateTime tA(QDate(2021, 1, 1), QTime(0, 00), Qt::OffsetFromUTC, sunCalculator.location().offsetUTC());
        QDateTime tB = tA.addYears(1);
        timeSampler.sample(tA, tB, QTime(0, 1), !true);
        sunTemporal.setData(wFunctor);

        // space
        SkySampler skySampler(&sunSpatial);
        skySampler.sample(resolution*degree, 40.*degree);
        cout << QString("Resolution: %1 deg\n").arg(resolution);
        cout << QString("Sampling points: %1\n").arg(sunSpatial.skyNodes().size());
        sunSpatial.setValues(sunFunctor);
        sunSpatial.setWeights(sunTemporal);


        double integralSampled = sunSpatial.integrate();
        double integralDirect = sunTemporal.integrateWeighted(sunFunctor);
        double diff = 100*fabs((integralSampled - integralDirect)/integralDirect);
        cout << QString("Integral sampled: %1\n").arg(integralSampled);
        cout << QString("Integral direct: %1\n").arg(integralDirect);
        cout << QString("Difference: %1%\n").arg(formatF6(diff));
        cout << endl;
    }
}

void IntegrationTest::test_HeliostatField()
{
    cout << "--- Test sun integration for heliostat ---\n";
    SunFunctorHeliostatField functor;
    functor.t = vec3d::directionAE(180*degree, 30*degree);
    testSunIntegrator(functor, SunFunctorDNI());
}

void IntegrationTest::test_HeliostatField2()
{
    cout << "--- Test sun integration for heliostat ---\n";
    SunFunctorHeliostatField2 functor;
    functor.t = vec3d::directionAE(180*degree, 30*degree);
    testSunIntegrator(functor, SunFunctorDNI());
}


QTEST_MAIN(IntegrationTest)
//#include "IntegrationTest.moc"

