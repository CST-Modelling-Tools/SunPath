#include "Interpolator.h"

#include <algorithm>

namespace sp {


// m_data[iA].x < x <= m_data[iA + 1].x
// range [0, size - 1)
int Interpolator::indexA(double x) const
{
    QVector<vec2d>::const_iterator iter = std::lower_bound(m_data.begin(), m_data.end(), x,
        [](const vec2d& a, double x) {return a.x < x;}
    );
    int i = iter - m_data.begin() - 1;
    return i;
}

double Interpolator::function(double x) const
{
    if (x <= m_data.first().x) return m_data.first().y;
    if (x >= m_data.last().x) return m_data.last().y;

    int i = indexA(x);
    return functionA(i, x);
}

double Interpolator::functionA(int iA, double /*x*/) const
{
    return m_data[iA].y;
}

double Interpolator::derivative(double x) const
{
    if (x <= m_data.first().x) return 0.;
    if (x >= m_data.last().x) return 0.;

    int i = indexA(x);
    return derivativeA(i, x);
}

double Interpolator::derivativeA(int /*iA*/, double /*x*/) const
{
    return 0.;
}


void InterpolatorLinear::setData(const QVector<vec2d>& data)
{
    m_data = data;

    double f = 0.;
    m_derivatives << f;
    for (int n = 0; n < m_data.size() - 1; ++n) {
        f = 2.*derivativeA(n, 0.) - f;
        if (derivativeA(n, 0.) == 0.) f = 0.;
        m_derivatives << f;
    }
}

double InterpolatorLinear::functionA(int iA, double x) const
{
    const vec2d& pA = m_data[iA];
    const vec2d& pB = m_data[iA + 1];

    double u = (x - pA.x)/(pB.x - pA.x);
    return pA.y*(1. - u) + pB.y*u;
}

double InterpolatorLinear::derivativeA(int iA, double /*x*/) const
{
    const vec2d& pA = m_data[iA];
    const vec2d& pB = m_data[iA + 1];
    return (pB.y - pA.y)/(pB.x - pA.x);
}


void InterpolatorCubic::setData(const QVector<vec2d>& data)
{
    m_data = data;

    int nMax = m_data.size();
    m_m.resize(nMax);

    {
        const vec2d& pA = m_data[0];
        const vec2d& pB = m_data[1];
        m_m[0] = (pB.y - pA.y)/(pB.x - pA.x);
    }

    for (int i = 1; i < nMax - 1; ++i) {
        const vec2d& pA = m_data[i - 1];
        const vec2d& pB = m_data[i + 1];
        m_m[i] = (pB.y - pA.y)/(pB.x - pA.x);
    }

    int i = nMax - 1;
    {
        const vec2d& pA = m_data[i - 1];
        const vec2d& pB = m_data[i];
        m_m[i] = (pB.y - pA.y)/(pB.x - pA.x);
    }
}

double InterpolatorCubic::functionA(int iA, double x) const
{
    int iB = iA;
    const vec2d& pA = m_data[iA];
    const vec2d& pB = m_data[iB];

    double u = (x - pA.x)/(pB.x - pA.x);
    double uu = u*u;
    double uuu = u*u*u;
    double h00 = 2.*uuu - 3.*uu + 1.;
    double h10 = uuu - 2.*uu + u;
    double h01 = -2.*uuu + 3.*uu;
    double h11 = uuu - uu;

    return h00*pA.y + h10*m_m[iA] + h01*pB.y + h11*m_m[iB];
}

double InterpolatorCubic::derivativeA(int iA, double x) const
{
    int iB = iA;
    const vec2d& pA = m_data[iA];
    const vec2d& pB = m_data[iB];

    double u = (x - pA.x)/(pB.x - pA.x);
    double uu = u*u;

    double h00 = 6.*uu - 6.*u;
    double h10 = 3.*uu - 4.*u + 1.;
    double h01 = -6.*uu + 6.*u;
    double h11 = 3.*uu - 2.*u;

    double temp = h00*pA.y + h10*m_m[iA] + h01*pB.y + h11*m_m[iB];
    return temp/(pB.x - pA.x);
}


void InterpolatorCubicMono::setData(const QVector<vec2d>& data)
{
    m_data = data;

    int nMax = m_data.size();
    m_c1.resize(nMax);
    m_c2.resize(nMax);
    m_c3.resize(nMax);

    // Get consecutive differences and slopes
    QVector<double> dx(nMax - 1);
    QVector<double> m(nMax - 1);
    for (int i = 0; i < nMax - 1; ++i) {
        dx[i] = m_data[i + 1].x - m_data[i].x;
        double dy = m_data[i + 1].y - m_data[i].y;
        m[i] = dy / dx[i];
    }

    // Get degree-1 coefficients
    m_c1[0] = m.first();
    for (int i = 1; i < nMax - 1; ++i) {
        if (m[i - 1]*m[i] <= 0)
            m_c1[i] = 0.;
        else {
            double c = dx[i - 1] + dx[i]; // Brodlie 1980
            m_c1[i] = 3*c/((c + dx[i])/m[i - 1] + (c + dx[i - 1])/m[i]);
        }
    }
    m_c1[nMax - 1] = m.last();

    // Get degree-2 and degree-3 coefficients
    for (int i = 0; i < nMax - 1; ++i) {
        double c = m_c1[i] + m_c1[i + 1] - 2*m[i]; // Fritsch 1980
        m_c2[i] = (m[i] - m_c1[i] - c)/dx[i];
        m_c3[i] = c/(dx[i]*dx[i]);
    }
}

double InterpolatorCubicMono::functionA(int iA, double x) const
{
    double u = x - m_data[iA].x;
    return m_data[iA].y + u*(m_c1[iA] + u*(m_c2[iA] + u*m_c3[iA]));
}

double InterpolatorCubicMono::derivativeA(int iA, double x) const
{
    double u = x - m_data[iA].x;
    return m_c1[iA] + u*(2.*m_c2[iA] + u*3.*m_c3[iA]);
}


} // namespace sp
