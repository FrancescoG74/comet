#include <iostream>

#include <QPushButton>
#include <QHBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>

#include "solarsystem.h"
#include "planetcontrolwidget.h"

PlanetControlWidget::PlanetControlWidget(SolarSystem* solarSystem, QWidget* parent)
    : QWidget(parent), system(solarSystem){
    addButton = new QPushButton("Add Planet", this);
    removeButton = new QPushButton("Remove Planet", this);
    auto* layout = new QHBoxLayout(this);
    layout->addWidget(addButton);
    layout->addWidget(removeButton);
    setLayout(layout);

    connect(addButton, &QPushButton::clicked, this, &PlanetControlWidget::addPlanetRequested);
    connect(removeButton, &QPushButton::clicked, this, &PlanetControlWidget::removePlanetRequested);
}

void PlanetControlWidget::addPlanetRequested()
{
    std::cout << "Add planet requested" << std::endl;
}

void PlanetControlWidget::removePlanetRequested()
{
    system->popBackPlanet();
}