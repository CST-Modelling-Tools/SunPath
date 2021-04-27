#pragma once

#include "SunPathLib/samplers/SunTemporal.h"
#include "SunPathLib/calculators/Location.h"


struct ParamsSeville
{
    bool seconds = false; // write seconds
    bool writeSunpos = false;
    int offset = 0; // in seconds
    int precision = 0; // precision for function
};


class FormatSeville
{
public:
    FormatSeville(sp::SunTemporal* sunTemporal);

    void setLocation(sp::Location& location);

    bool read(QString fileName, const ParamsSeville& params = ParamsSeville());
    bool write(QString fileName, const ParamsSeville& params = ParamsSeville());
    QString message() const {return m_message;}

protected:
    void readData(QTextStream& fin, const ParamsSeville& params);

protected:
    sp::SunTemporal* m_sunTemporal;
    QString m_message;
};

