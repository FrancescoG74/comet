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
#include <QTimer>
#include <QDateTime>
#include <cstdlib>
#include <ctime>

#include "solarsystem.h"
#include "planetcontrolwidget.h"
#include "astronomicalbody.h"

PlanetControlWidget::PlanetControlWidget(SolarSystem* solarSystem, QWidget* parent)
    : QWidget(parent), system(solarSystem){
    srand(static_cast<unsigned>(time(nullptr)));
    
    // Initialize simulation time to a starting date (2000-01-01 00:00:00)
    simulationTime = QDateTime(QDate(2000, 1, 1), QTime(0, 0, 0));
    lastUpdateTime = 0;
    
    timeLabel = std::make_unique<QLabel>("Date/Time: 2000-01-01 00:00:00", this);
    timeLabel->setStyleSheet(
        "QLabel {"
        "  background-color: #222222;"
        "  color: #00FF00;"
        "  font-weight: bold;"
        "  font-size: 12px;"
        "  padding: 5px;"
        "  border: 1px solid #444444;"
        "  border-radius: 3px;"
        "}"
    );
    timeLabel->setAlignment(Qt::AlignCenter);
    timeLabel->setMinimumHeight(30);
    
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
    mainLayout->addWidget(timeLabel.get());  // Add time display at top
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
    
    // Setup timer for updating simulation time display (every 100ms)
    timeUpdateTimer = std::make_unique<QTimer>(this);
    connect(timeUpdateTimer.get(), &QTimer::timeout, this, &PlanetControlWidget::updateSimulationTime);
    timeUpdateTimer->start(100);  // Update every 100ms
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
        
        // Reset the time tracking when starting
        if (isRunning) {
            lastUpdateTime = 0;
        }
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

void PlanetControlWidget::updateSimulationTime()
{
    if (!isRunning) {
        // Display current simulation time even when not running
        QString dateTimeStr = simulationTime.toString("yyyy-MM-dd hh:mm:ss");
        timeLabel->setText(QString("Date/Time: %1").arg(dateTimeStr));
        return;
    }
    
    // Get current real time in milliseconds
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    
    // Initialize lastUpdateTime on first call
    if (lastUpdateTime == 0) {
        lastUpdateTime = currentTime;
        return;
    }
    
    // Calculate elapsed real time since last update (in seconds)
    qint64 elapsedRealMs = currentTime - lastUpdateTime;
    double elapsedRealSeconds = elapsedRealMs / 1000.0;
    
    // Get current speed multiplier
    int sliderValue = speedSlider->value();
    double speedMultiplier = sliderValue / 100.0;
    
    // Calculate simulation time advancement
    // Each real second = speedMultiplier * 86400 seconds of simulation (1 day per real second at 1x speed)
    double simulationSecondsPerRealSecond = speedMultiplier * 86400.0;  // 1 day per real second at 1x
    qint64 simulationSeconds = static_cast<qint64>(elapsedRealSeconds * simulationSecondsPerRealSecond);
    
    // Add to simulation time
    simulationTime = simulationTime.addSecs(simulationSeconds);
    
    // Update display label
    QString dateTimeStr = simulationTime.toString("yyyy-MM-dd hh:mm:ss");
    timeLabel->setText(QString("Date/Time: %1").arg(dateTimeStr));
    
    // Update tracking time
    lastUpdateTime = currentTime;
}