#include <QCoreApplication>
#include <QCommandLineParser>

#include "common.h"
#include "Proteas/tasks.h"
#include "Seville/tasksSeville.h"


int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    QCommandLineParser parser;
    parser.process(app);

    QDir::current().mkpath(sDirTemp); // make path

    taskClearSky();
//    taskSeville();

//    return app.exec();
}
