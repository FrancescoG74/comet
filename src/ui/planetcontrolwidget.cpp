#include <iostream>

#include <memory>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QColor>
#include <QApplication>
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
    quitButton = std::make_unique<QPushButton>("Quit", this);
    speedSlider = std::make_unique<QSlider>(Qt::Horizontal, this);
    speedLabel = std::make_unique<QLabel>("Speed: 1.0x", this);
    
    // Configure speed slider (range: 10 to 2000, representing 0.1x to 20x speed)
    speedSlider->setMinimum(10);
    speedSlider->setMaximum(2000);
    speedSlider->setValue(100);  // Default: 1.0x speed
    speedSlider->setTickPosition(QSlider::TicksBelow);
    speedSlider->setTickInterval(200);
    
    // Create horizontal layout for Add/Remove buttons
    auto* horizontalLayout = new QHBoxLayout();
    horizontalLayout->addWidget(addButton.get());
    horizontalLayout->addWidget(removeButton.get());
    
    // Create speed control layout
    auto* speedLayout = new QHBoxLayout();
    speedLayout->addWidget(speedLabel.get());
    speedLayout->addWidget(speedSlider.get());
    
    // Create main vertical layout
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(horizontalLayout);
    mainLayout->addWidget(startStopButton.get());
    mainLayout->addWidget(quitButton.get());
    mainLayout->addLayout(speedLayout);
    setLayout(mainLayout);

    connect(addButton.get(), &QPushButton::clicked, this, &PlanetControlWidget::onAddPlanetClicked);
    connect(removeButton.get(), &QPushButton::clicked, this, &PlanetControlWidget::onRemovePlanetClicked);
    connect(startStopButton.get(), &QPushButton::clicked, this, &PlanetControlWidget::onStartStopClicked);
    connect(quitButton.get(), &QPushButton::clicked, this, &PlanetControlWidget::onQuitClicked);
    connect(speedSlider.get(), QOverload<int>::of(&QSlider::valueChanged), this, &PlanetControlWidget::onSimulationSpeedChanged);
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

void PlanetControlWidget::updateButtonState(bool isRunning)
{
    this->isRunning = isRunning;
    startStopButton->setText(isRunning ? "Stop" : "Start");
}

void PlanetControlWidget::onSimulationSpeedChanged(int value)
{
    // Convert slider value (10-2000) to speed multiplier (0.1x - 20x)
    double speedMultiplier = value / 100.0;
    if (system) {
        system->setSimulationSpeedMultiplier(speedMultiplier);
    }
    // Update label
    speedLabel->setText(QString("Speed: %1x").arg(speedMultiplier, 0, 'f', 1));
}

void PlanetControlWidget::onQuitClicked()
{
    QApplication::quit();
}