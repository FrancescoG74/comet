
#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QtMath>
#include <QKeyEvent>

#include <QVector>


#include "solarsystem.h"
#include "planetcontrolwidget.h"
#include "planetcontrolcontroller.h"
#include "solarsystemcontroller.h"
#include <QHBoxLayout>
#include <QVBoxLayout>





int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    // Create main container widget
    QWidget mainWidget;
    QVBoxLayout* rootLayout = new QVBoxLayout(&mainWidget);

    // Create SolarSystem (with mainWidget as parent for proper memory management)
    SolarSystem* solarSystem = new SolarSystem(&mainWidget);
    solarSystem->setWindowTitle("Solar System");

    // Create planet control panel (view) and its controller
    PlanetControlWidget* controlPanel = new PlanetControlWidget(&mainWidget);
    PlanetControlController* controlController = new PlanetControlController(solarSystem, controlPanel, &mainWidget);

    // Connect control panel to solar system so it can update button on spacebar
    solarSystem->setControlController(controlController);

    // Date/time display spans the full window width, sitting above the simulation view.
    rootLayout->addWidget(controlPanel->getTimeLabel());

    QHBoxLayout* bodyLayout = new QHBoxLayout();
    bodyLayout->addWidget(solarSystem, 1);
    bodyLayout->addWidget(controlPanel);
    rootLayout->addLayout(bodyLayout);

    mainWidget.setLayout(rootLayout);
    mainWidget.setWindowTitle("Solar System Simulation");
    mainWidget.resize(1024, 768);
    mainWidget.show();
    return app.exec();
}
