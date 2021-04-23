#pragma once

#include <QSharedPointer>

#include "SunPathLib/samplers/SkySampler.h"
#include "SunPathLib/data/SunFunctor.h"
#include "SunPathLib/math/matrices/MatrixNR.h"

namespace sp {


class SunTemporal;


class SUNPATHLIB SunSpatial
{
public:
    SunSpatial(SunCalculator& calculator);
    SunCalculator* calculator() {return m_calculator.get();}

    void setSkyNodes(const QVector<SkyNode>& skyNodes);
    const QVector<SkyNode>& skyNodes() const {return m_skyNodes;}

    void setValues(const QVector<double>& values);
    void setValues(const SunFunctor& sf);
    void setValues(SunTemporal& sunTemporal);
    const QVector<double>& values() const {return m_values;}
    const QVector<double>& amplitudes() const {return m_amplitudes;}

    void setWeights(SunTemporal& sunTemporal, bool normalize = true);
    const QVector<double>& weights() const {return m_weights;}

    double interpolate(const vec3d& v) const;
    double integrate() const; // weighted
    double average() const;

    double validate() const;

    QString info() const {return m_info;}
    void setInfo(QString info) {m_info = info;}

protected:
    QSharedPointer<SunCalculator> m_calculator;
    QString m_info;

    QVector<SkyNode> m_skyNodes;
    QVector<double> m_values;
    QVector<double> m_amplitudes;
    QVector<double> m_weights;

    MatrixNR m_matrixK;
};


} // namespace sp
