#pragma once

#include <QtTest>


class IntegrationTest : public QObject
{
    Q_OBJECT

private slots:
    void test_HeliostatField();
    void test_HeliostatField2();
};
