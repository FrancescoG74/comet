
#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QtMath>
#include <QKeyEvent>

#include <QVector>


#include "solarsystem.h"
#include "planetcontrolwidget.h"
#include "solarsystemcontroller.h"
#include <QHBoxLayout>





int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    // Create main container widget
    QWidget mainWidget;
    QHBoxLayout* layout = new QHBoxLayout(&mainWidget);

    // Create SolarSystem (with mainWidget as parent for proper memory management)
    SolarSystem* solarSystem = new SolarSystem(&mainWidget);
    solarSystem->setWindowTitle("Solar System");

    // Create planet control panel
    PlanetControlWidget* controlPanel = new PlanetControlWidget(solarSystem, &mainWidget);
    
    // Connect control panel to solar system so it can update button on spacebar
    solarSystem->setControlWidget(controlPanel);

    layout->addWidget(solarSystem, 1);
    layout->addWidget(controlPanel);
    mainWidget.setLayout(layout);
    mainWidget.setWindowTitle("Solar System Simulation");
    mainWidget.resize(1024, 768);
    mainWidget.show();
    return app.exec();
}
