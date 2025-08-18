
#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QtMath>
#include <QKeyEvent>

#include <QVector>


#include "headers/solarsystem.h"
#include "headers/planetcontrolwidget.h"
#include "headers/solarsystemcontroller.h"
#include <QHBoxLayout>





int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    // Create main container widget
    QWidget mainWidget;
    QHBoxLayout* layout = new QHBoxLayout(&mainWidget);

    // Create SolarSystem and controller
    SolarSystem* solarSystem = new SolarSystem;
    SolarSystemController* controller = new SolarSystemController(solarSystem);
    solarSystem->setWindowTitle("Solar System");

    // Create planet control panel
    PlanetControlWidget* controlPanel = new PlanetControlWidget(solarSystem);

    layout->addWidget(solarSystem, 1);
    layout->addWidget(controlPanel);
    mainWidget.setLayout(layout);
    mainWidget.setWindowTitle("Solar System Simulation");
    mainWidget.show();
    return app.exec();
}
