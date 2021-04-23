#pragma once

#include <QtTest>


class InterpolationTest : public QObject
{
    Q_OBJECT

private slots:
    void test_One();
    void test_HeliostatField_old();
    void test_HeliostatField();
    void test_ClearSky();
};
