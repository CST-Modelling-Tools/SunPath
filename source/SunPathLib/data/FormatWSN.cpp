#include "FormatWSN.h"

#include <QFile>
#include <QTextStream>

using Qt::endl;

namespace sp {


//  QDate::currentDate().toString("yyyy-MM-dd")

//ParamsWSN::ParamsWSN(
//    bool symmetryEW,
//    int precision,
//    bool positiveAzimuth
//):
//    symmetryEW(symmetryEW),
//    precision(precision),
//    positiveAzimuth(positiveAzimuth)
//{}


bool FormatWSN::write(QString fileName, const ParamsWSN& params)
{
    try {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            throw QString("File not opened: ") + fileName;

        QTextStream fout(&file);
        fout << "Sampling of sun path\n";
        fout << endl;

        SunCalculator* sun = m_sunSpatial->calculator();
        fout << QString("Name: %1\n").arg(sun->location().name());
        fout << QString("Latitude: %1 deg\n").arg(sun->location().latitude()/degree, 0, 'f', 4);
        fout << QString("Longitude: %1 deg\n").arg(sun->location().longitude()/degree, 0, 'f', 4);
        fout << endl;

        const QVector<SkyNode>& skyNodes = m_sunSpatial->skyNodes();
        const QVector<double>& values = m_sunSpatial->values();
        const QVector<double>& weights = m_sunSpatial->weights();
        const QVector<double>& amplitudes = m_sunSpatial->amplitudes();
        int nMax = skyNodes.size();

        fout << QString("Nodes: %1\n").arg(nMax);
        if (!m_sunSpatial->info().isEmpty())
            fout << m_sunSpatial->info() << endl;
        fout << endl;

        QString temp;
        int cw = 16;
        temp += QString("Index").rightJustified(cw) + ",";
        temp += QString("Azimuth [deg]").rightJustified(cw) + ",";
        temp += QString("Elevation [deg]").rightJustified(cw) + ",";
        temp += QString("Sigma [deg]").rightJustified(cw) + ",";

        temp += QString("Function []").rightJustified(cw) + ",";
        temp += QString("Amplitude []").rightJustified(cw) + ",";
        temp += QString("Weight [Wh/m2]").rightJustified(cw) + ",";
        temp += QString("Weight [%]").rightJustified(cw);


        fout << temp << endl;

        double wTotal = 0.;
        for (double w : weights) wTotal += w;

        for (int n = 0; n < nMax; ++n) {
            QString temp;

            const SkyNode& sn = skyNodes[n];
            Horizontal hc = sun->findHorizontal(sn.v);

            temp += QString::number(n + 1).rightJustified(cw) + ",";
            temp += QString::number(hc.azimuth()/degree, 'f', 4).rightJustified(cw) + ",";
            temp += QString::number(hc.elevation()/degree, 'f', 4).rightJustified(cw) + ",";
            temp += QString::number(sn.sigma/degree, 'f', 4).rightJustified(cw) + ",";

            double v = values[n];
            double w = weights[n];
            double a = amplitudes[n];

            temp += QString::number(v, 'f', params.precision).rightJustified(cw) + ",";
            temp += QString::number(a, 'f', params.precision).rightJustified(cw) + ",";
            temp += QString::number(w, 'f', 3).rightJustified(cw) + ",";
            temp += QString::number(w/wTotal*100, 'f', 6).rightJustified(cw);

            fout << temp << endl;
        }

        m_message.clear();
        return true;
    }
    catch (const QString& message) {
        m_message = QString("Runtime error in FormatWSN::write\n") + message;
        qDebug() << m_message;
        return false;
    }
}


} // namespace sp
