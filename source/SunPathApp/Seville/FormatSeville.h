#pragma once

#include "SunPathLib/samplers/SunTemporal.h"
#include "SunPathLib/calculators/Location.h"


class FormatSeville
{
public:
    FormatSeville(sp::SunTemporal* sunTemporal);

    void setLocation(sp::Location& location);

    bool read(QString fileName);
    bool write(QString fileName);
    QString message() const {return m_message;}

protected:
    void readData(QTextStream& fin);

protected:
    sp::SunTemporal* m_sunTemporal;
    QString m_message;
};

