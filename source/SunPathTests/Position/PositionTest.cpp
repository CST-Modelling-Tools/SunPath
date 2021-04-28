#include "PositionTest.h"
#include "CommonLib/common.h"

#include "SunPathLib/calculators/SunCalculatorET.h"
#include "SunPathLib/calculators/SunCalculatorMB.h"
#include "SunPathLib/calculators/SunCalculatorNREL.h"
#include "SunPathLib/calculators/SunCalculatorRG.h"
#include "SunPathLib/math/sampling/ErrorAnalysis.h"

using namespace sp;


void test(SunCalculatorMB& scA, SunCalculator& scB)
{
    scA.setLocation(sLocationProteas);
    scB.setLocation(sLocationProteas);

    QDateTime tA = QDateTime(QDate(2021, 1, 1), QTime(0, 00), Qt::OffsetFromUTC, 2*3600);
//    QDateTime tA = scA.getLocalTime(QDate(2021, 1, 1), QTime(0, 00));
    QDateTime tB = tA.addYears(1);
    int dt = QTime(0, 05).msecsSinceStartOfDay();

    ErrorAnalysis ea;
    QQueue<QDateTime> ts;
    for (QDateTime t = tA; t < tB; t = t.addMSecs(dt))
    {
        vec3d vA = scA.findVector(t);
        if (vA.z < 0) continue;
        vec3d vB = scB.findVector(t);
        double eps = acos(dot(vA, vB));

        ea.push(eps);
        if (eps < ea.maxAbs()) continue;
        ts.enqueue(t);
        if (ts.size() > 5) ts.dequeue();
    }

    cout << QString("RMS: %1 deg\n").arg(ea.rms()/degree, 0, 'f', 3);
    cout << endl;

    // worst points
    std::reverse(ts.begin(), ts.end());
    for (QDateTime& t : ts) {
        vec3d vA = scA.findVector(t);
        vec3d vB = scB.findVector(t);
        double eps = acos(dot(vA, vB));

        cout << QString("Difference: %1 deg (%2)\n%3\n%4\n\n").arg(
            formatF3(eps/degree),
            formatTime(t),
            formatSunPos(scA.findHorizontal(vA)),
            formatSunPos(scB.findHorizontal(vB))
        );
    }
    cout << endl;
}

void PositionTest::test_MB_ET()
{
    cout << "--- Test sun calculators ET ---\n";
    SunCalculatorMB sunMB;
    SunCalculatorET sunET;
    test(sunMB, sunET);
}

void PositionTest::test_MB_MB2()
{
    cout << "--- Test sun calculators MB2 ---\n";
    SunCalculatorMB sunMB;
    sunMB.setVersion(SunCalculatorMB::V2020);
    SunCalculatorMB sunMB2;
    sunMB2.setVersion(SunCalculatorMB::V2001);
    test(sunMB, sunMB2);
}

void PositionTest::test_MB_NREL()
{
    cout << "--- Test sun calculators NREL ---\n";
    SunCalculatorMB sunMB;
    SunCalculatorNREL sunNEL;
    test(sunMB, sunNEL);
}

void PositionTest::test_MB_RG()
{
    cout << "--- Test sun calculators RG ---\n";
    SunCalculatorMB sunMB;
    SunCalculatorRG sunRG;
    test(sunMB, sunRG);
}

QTEST_MAIN(PositionTest)
//#include "PositionTest.moc"
