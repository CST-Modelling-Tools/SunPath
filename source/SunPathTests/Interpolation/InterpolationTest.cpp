#include "InterpolationTest.h"
#include "CommonLib/common.h"

#include "SunPathLib/calculators/SunCalculatorMB.h"
#include "SunPathLib/math/sampling/ErrorAnalysis.h"
#include "SunPathLib/samplers/SunSpatial.h"
#include "SunPathLib/samplers/SunTemporal.h"

#include <QQueue>


void testSunInterpolator(const SunFunctor& sunFunctor, double rho, double sigma)
{
    SunCalculatorMB sunCalculator;
    sunCalculator.setLocation(sLocationProteas);

    // sample
    SunSpatial sunSpatial(sunCalculator);

    SkySampler skySampler(&sunSpatial);
    skySampler.sample(rho, sigma);
    cout << QString("Nodes: %1\n").arg(sunSpatial.skyNodes().size());

    sunSpatial.setValues(sunFunctor);
    cout << QString("RMS at nodes: %1\n").arg(sunSpatial.validate());

    // check
    QDateTime tA = sunCalculator.getLocalTime(QDate(2021, 1, 1), QTime(0, 00));
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
    cout << QString("RMS at timestamps: %1 (%2%)\n")
        .arg(ea.rms())
        .arg(100*ea.rms()/ea.fMax(), 0, 'f', 3);

    // report
    cout << QString("Worst points:\n");
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

void testSunInterpolatorLoop(const SunFunctor& sunFunctor)
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
        testSunInterpolator(sunFunctor, p.rho*degree, p.sigma*degree);
    }
}

void InterpolationTest::test_One()
{
    cout << "--- Test sun interpolation for one ---\n";
    SunFunctorOne functor;
    testSunInterpolatorLoop(functor);
}

void InterpolationTest::test_HeliostatField_old()
{
    cout << "--- Test sun interpolation for heliostat field old ---\n";
    SunFunctorHeliostatFieldOld functor;
    functor.t = vec3d::directionAE(180*degree, 30*degree);
    testSunInterpolatorLoop(functor);
}

void InterpolationTest::test_HeliostatField()
{
    cout << "--- Test sun interpolation for heliostat field ---\n";
    SunFunctorHeliostatField functor;
    functor.t = vec3d::directionAE(180*degree, 30*degree);
    testSunInterpolatorLoop(functor);
}

void InterpolationTest::test_ClearSky()
{
    cout << "--- Test sun interpolation for irradiance of clear sky  ---\n";
    SunFunctorDNI functor;
    testSunInterpolatorLoop(functor);
}

QTEST_MAIN(InterpolationTest)
