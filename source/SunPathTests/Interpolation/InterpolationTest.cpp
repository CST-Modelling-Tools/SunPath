#include "InterpolationTest.h"
#include "CommonLib/common.h"

#include "SunPathLib/calculators/SunCalculatorMB.h"
#include "SunPathLib/math/sampling/ErrorAnalysis.h"
#include "SunPathLib/samplers/SunSpatial.h"
#include "SunPathLib/samplers/SunTemporal.h"

#include <QQueue>


void testSunInterpolator(const SunFunctor& sunFunctor)
{
    SunCalculatorMB sunCalculator;
    sunCalculator.setLocation(sLocationProteas);
    sunCalculator.setVersion(SunCalculatorMB::V2020);

    SunSpatial sunSpatial(sunCalculator);

    QVector<double> resolutions = {30., 15.};
    for (double resolution : resolutions)
    {
        // set
        SkySampler skySampler(&sunSpatial);
        skySampler.sample(resolution*degree, 90.*degree);
        cout << QString("Resolution: %1 deg\n").arg(resolution);
        cout << QString("Sampling points: %1\n").arg(sunSpatial.skyNodes().size());

        sunSpatial.setValues(sunFunctor);
        cout << QString("RMS (nodes): %1\n").arg(sunSpatial.validate());

        // check
        QDateTime tA(QDate(2021, 1, 1), QTime(0, 00), Qt::OffsetFromUTC, sunCalculator.location().offsetUTC());
        QDateTime tB = tA.addYears(1);
        int dt = QTime(0, 10).msecsSinceStartOfDay();

        ErrorAnalysis ea;
        QQueue<QDateTime> ts;
        for (QDateTime t = tA; t < tB; t = t.addMSecs(dt))
        {
            vec3d v = sunCalculator.findVector(t);
            if (v.z < 0.) continue;
            double f = sunSpatial.interpolate(v);
            double f0 = sunFunctor(v);

            ea.push(f, f0);
            if (fabs(f - f0) < ea.maxAbs()) continue;
            ts.enqueue(t);
            if (ts.size() > 5) ts.dequeue();
        }
        cout << QString("RMS (full): %1%\n").arg(100*ea.rms(), 0, 'f', 3);
        cout << endl;

        // worst points
        std::reverse(ts.begin(), ts.end());
        for (QDateTime& t : ts) {
            vec3d v = sunCalculator.findVector(t);
            double f = sunSpatial.interpolate(v);
            double f0 = sunFunctor(v);

            cout << formatSunPos(t, sunCalculator.findHorizontal(v)) << endl;
            cout << QString("Interpolation: %1, Function: %2, Difference: %3\n")
                .arg(f, 0, 'f', 3)
                .arg(f0, 0, 'f', 3)
                .arg(f - f0, 0, 'f', 6);
        }
        cout << endl;
    }
}

void InterpolationTest::test_One()
{
    cout << "--- Test sun interpolation for one ---\n";
    SunFunctorOne functor;
    testSunInterpolator(functor);
}

//void InterpolationTest::test_PanelCos()
//{
//    cout << "--- Test sun interpolation for panel ---\n";
//    SunFunctorPanelCos functor;
//    functor.n = vec3d::directionAE(180*degree, 60*degree);
//    testSunInterpolator(functor);
//}

//void InterpolationTest::test_HeliostatCos()
//{
//    cout << "--- Test sun interpolation for heliostat ---\n";
//    SunFunctorHeliostatCos functor;
//    functor.t = vec3d::directionAE(180*degree, 30*degree);
//    testSunInterpolator(functor);
//}

void InterpolationTest::test_HeliostatField()
{
    cout << "--- Test sun interpolation for heliostat field ---\n";
    SunFunctorHeliostatField functor;
    functor.t = vec3d::directionAE(180*degree, 20*degree);
    testSunInterpolator(functor);
}

//void InterpolationTest::test_ClearSky()
//{
//    cout << "--- Test sun interpolation for irradiance of clear sky  ---\n";
//    SunFunctorDNI functor;
//    testSunInterpolator(functor);
//}

QTEST_MAIN(InterpolationTest)
