#include <iostream>

#include <memory>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
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
    addButton = std::make_unique<QPushButton>("Add Planet", this);
    removeButton = std::make_unique<QPushButton>("Remove Planet", this);
    startStopButton = std::make_unique<QPushButton>("Start", this);
    
    // Create horizontal layout for Add/Remove buttons
    auto* horizontalLayout = new QHBoxLayout();
    horizontalLayout->addWidget(addButton.get());
    horizontalLayout->addWidget(removeButton.get());
    
    // Create main vertical layout
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(horizontalLayout);
    mainLayout->addWidget(startStopButton.get());
    setLayout(mainLayout);

    connect(addButton.get(), &QPushButton::clicked, this, &PlanetControlWidget::onAddPlanetClicked);
    connect(removeButton.get(), &QPushButton::clicked, this, &PlanetControlWidget::onRemovePlanetClicked);
    connect(startStopButton.get(), &QPushButton::clicked, this, &PlanetControlWidget::onStartStopClicked);
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
        
        Planet newPlanet(pos, vel, mass, radius, color);
        system->appendPlanet(newPlanet);
    }
}

void PlanetControlWidget::onRemovePlanetClicked()
{
    if (system) {
        system->popBackPlanet();
    }
}

void PlanetControlWidget::onStartStopClicked()
{
    if (system) {
        isRunning = !isRunning;
        system->setSimulationActive(isRunning);
        startStopButton->setText(isRunning ? "Stop" : "Start");
    }
}