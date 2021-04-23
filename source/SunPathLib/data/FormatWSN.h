#pragma once

#include "SunPathLib/samplers/SunSpatial.h"

namespace sp {

// Weighted Sun Nodes
// Parameters, symmetrized


struct ParamsWSN {
    bool symmetryEW = false; // East-West symmetry
    int precision = 6;
    bool positiveAzimuth = false;
};
// Designated initializers
// struct ParamsWSN params = {.positiveAzimuth = true};


class SUNPATHLIB FormatWSN
{
public:
    FormatWSN(SunSpatial* sunSpatial) {m_sunSpatial = sunSpatial;}

    bool write(QString fileName, const ParamsWSN& params = ParamsWSN());
    QString message() const {return m_message;}

protected:
    SunSpatial* m_sunSpatial;
    QString m_message;
};


} // namespace sp
