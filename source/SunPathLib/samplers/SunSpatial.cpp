#include "SunSpatial.h"

#include "SunPathLib/samplers/SunTemporal.h"
#include "SunPathLib/math/sampling/ErrorAnalysis.h"

namespace sp {


struct SunFunctorKernel: SunFunctor
{
    SunFunctorKernel(SkyNode node): node(node) {}
    SkyNode node;
    double operator()(const vec3d& s) const {return node.kernel(s);}
};


struct SunFunctorKernel2: SunFunctor
{
    SunFunctorKernel2(SkyNode p, SkyNode q): nodeP(p), nodeQ(q) {}
    SkyNode nodeP;
    SkyNode nodeQ;
    double operator()(const vec3d& s) const {
//        if (s.z <= 0.) return 0.; // can give negative irradiance
        return nodeP.kernel(s)*nodeQ.kernel(s);
    }
};


SunSpatial::SunSpatial(SunCalculator& calculator):
    m_calculator(calculator.copy())
{

}

void SunSpatial::setSkyNodes(const QVector<SkyNode>& skyNodes)
{
    m_skyNodes = skyNodes;

    int nMax = m_skyNodes.size();
    m_matrixK.resize(nMax, nMax);
    for (int n = 0; n < nMax; ++n)
        for (int m = 0; m < nMax; ++m)
            m_matrixK(n, m) = m_skyNodes[n].kernel(skyNodes[m].v);

    m_values.fill(0., nMax);
    m_amplitudes.fill(0., nMax);
    m_weights.fill(1., nMax);
}

void SunSpatial::setValues(const QVector<double>& values)
{
    Q_ASSERT(values.size() == m_skyNodes.size());
    m_values = values;
    m_amplitudes = m_matrixK.solve(values);
}

void SunSpatial::setValues(const SunFunctor& sf)
{
    QVector<double> values;
    for (const SkyNode& sk : m_skyNodes)
        values << sf(sk.v);
    setValues(values);
}

void SunSpatial::setValues(SunTemporal& sunTemporal)
{  
    QVector<double> overlapsW;
    for (SkyNode& sn : m_skyNodes) {
        SunFunctorKernel sk(sn);
        overlapsW << sunTemporal.integrateWeighted(sk);
    }

    int nMax = m_skyNodes.size();
    MatrixNR matrixKK(nMax, nMax);
    SunTemporal sunTemporalK(*sunTemporal.calculator());
    sunTemporalK.setTimeStamps(sunTemporal.timeStamps());
    if (true) {
        for (int p = 0; p < nMax; ++p)
            for (int q = p; q < nMax; ++q) {
                SunFunctorKernel2 sk(m_skyNodes[p], m_skyNodes[q]);
                sunTemporalK.setData(sk);
                double s = sunTemporalK.integrate();
                matrixKK(p, q) = s;
                if (p != q) matrixKK(q, p) = s;
            }
    }
//    else
//    {
//        QVector<double> overlapsK;

//        for (SkyNode& sn : m_skyNodes) {
//            SunFunctorKernel sk(sn);
//            sunTemporalK.setData(sk);
//            overlapsK << sunTemporalK.integrate();
//        }

//        for (int p = 0; p < nMax; ++p)
//            for (int q = p; q < nMax; ++q) {
//                QVector<double> kpkq;
//                for (int m = 0; m < nMax; ++m)
//                    kpkq << m_matrixK(p, m)*m_matrixK(q, m);
//                QVector<double> cs = m_matrixK.solve(kpkq);

//                double s = 0.;
//                for (int m = 0; m < nMax; ++m)
//                    s += cs[m]*overlapsK[m];
//                matrixKK(p, q) = s;
//                if (p != q) matrixKK(q, p) = s;
//            }
//    }

    m_amplitudes = matrixKK.solve(overlapsW);

    m_values.clear();
    for (SkyNode& sn : m_skyNodes)
        m_values << interpolate(sn.v);
}

void SunSpatial::setWeights(SunTemporal& sunTemporal, bool normalize)
{
    QVector<double> overlaps;
    for (SkyNode& sn : m_skyNodes) {
        SunFunctorKernel sk(sn);
        overlaps << sunTemporal.integrateWeighted(sk);
    }

    QVector<double> weights = m_matrixK.solve(overlaps);
    if (normalize) {
        double wTotal = 0.;
        for (double w : weights) wTotal += w;
        for (double& w : weights) w /= wTotal;
        wTotal = sunTemporal.integrate();
        for (double& w : weights) w *= wTotal;
    }
    m_weights = weights;
}

double SunSpatial::interpolate(const vec3d& v) const
{
    double ans = 0.;
    for (int n = 0; n < m_skyNodes.size(); ++n)
        ans += m_amplitudes[n]*m_skyNodes[n].kernel(v);
    return ans;
}

double SunSpatial::integrate() const
{
    double ans = 0.;
    for (int n = 0; n < m_skyNodes.size(); ++n)
        ans += m_weights[n]*m_values[n];
    return ans;
}

double SunSpatial::average() const
{
    double wf = 0.;
    double w = 0.;
    for (int n = 0; n < m_skyNodes.size(); ++n) {
        wf += m_weights[n]*m_values[n];
        w += m_weights[n];
    }
    return wf/w;
}

double SunSpatial::validate() const
{
    ErrorAnalysis ea;

    for (int n = 0; n < m_skyNodes.size(); ++n)
        ea << interpolate(m_skyNodes[n].v) - m_values[n];

    return ea.rms();
}


} // namespace sp
