#pragma once

#include <QtTest>  // http://doc.qt.io/qt-5/qttest-index.html


class PositionTest : public QObject
{
    Q_OBJECT

public:
//    PositionTest() {}
//    ~PositionTest() {}

private slots:
//    void initTestCase(); // called before the first test function
//    void init(); // called before every test function
//    void cleanup(); // called after every test function
//    void cleanupTestCase(); // called after the last test function

    void test_MB_ET();
    void test_MB_MB2();
    void test_MB_NREL();
    void test_MB_RG();
};

