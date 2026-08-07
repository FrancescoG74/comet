#include <iostream>

#include <QPushButton>
#include <QHBoxLayout>
#include <QWidget>
#include <QColor>
#include <cstdlib>
#include <ctime>

#include "solarsystem.h"
#include "planetcontrolwidget.h"
#include "astronomicalbody.h"

PlanetControlWidget::PlanetControlWidget(SolarSystem* solarSystem, QWidget* parent)
    : QWidget(parent), system(solarSystem){
    srand(static_cast<unsigned>(time(nullptr)));
    addButton = new QPushButton("Add Planet", this);
    removeButton = new QPushButton("Remove Planet", this);
    auto* layout = new QHBoxLayout(this);
    layout->addWidget(addButton);
    layout->addWidget(removeButton);
    setLayout(layout);

    connect(addButton, &QPushButton::clicked, this, &PlanetControlWidget::onAddPlanetClicked);
    connect(removeButton, &QPushButton::clicked, this, &PlanetControlWidget::onRemovePlanetClicked);
}

void PlanetControlWidget::onAddPlanetClicked()
{
    if (system) {
        // Create a new planet at a random position with all required fields
        QVector3D pos(300 + (rand() % 200), 200 + (rand() % 200), 0);
        QVector3D vel(2, 1, 0);
        double mass = 500;
        double radius = 15;
        QColor color = Qt::yellow;
        
        AstronomicalBody newPlanet(pos, vel, mass, radius, color);
        system->appendPlanet(newPlanet);
    }
}

void PlanetControlWidget::onRemovePlanetClicked()
{
    if (system) {
        system->popBackPlanet();
    }
}