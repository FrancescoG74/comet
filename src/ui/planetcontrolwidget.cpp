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
#include <QFontMetrics>

#include "solarsystem.h"
#include "planetcontrolwidget.h"
#include "astronomicalbody.h"

PlanetControlWidget::PlanetControlWidget(SolarSystem* solarSystem, QWidget* parent)
    : QWidget(parent), system(solarSystem){
    // Fix this panel's width so per-second label text changes (proportional font digit
    // widths differ slightly) never nudge the top-level window's layout/geometry.
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    setFixedWidth(230);

    timeLabel = std::make_unique<QLabel>("Date/Time: --", this);
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
    // Widest possible content is a fixed-format "yyyy-MM-dd hh:mm:ss" string; lock the
    // label's width so its sizeHint can't fluctuate as digits change every second.
    QFontMetrics timeMetrics(timeLabel->font());
    timeLabel->setFixedWidth(timeMetrics.horizontalAdvance("Date/Time: 0000-00-00 00:00:00") + 16);
    
    startStopButton = std::make_unique<QPushButton>("Start", this);
    quitButton = std::make_unique<QPushButton>("Quit", this);
    toggleSatellitesButton = std::make_unique<QPushButton>("Hide Satellites", this);
    speedSlider = std::make_unique<QSlider>(Qt::Horizontal, this);
    speedLabel = std::make_unique<QLabel>("Speed: 1.0x", this);
    
    // Configure speed slider (range: 10 to 2000, representing 0.1x to 20x speed)
    speedSlider->setMinimum(10);
    speedSlider->setMaximum(2000);
    speedSlider->setValue(100);  // Default: 1.0x speed
    speedSlider->setTickPosition(QSlider::TicksBelow);
    speedSlider->setTickInterval(200);
    
    // Create speed control layout
    auto* speedLayout = new QHBoxLayout();
    speedLayout->addWidget(speedLabel.get());
    speedLayout->addWidget(speedSlider.get());
    
    // Create main vertical layout
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(timeLabel.get());  // Add time display at top
    mainLayout->addWidget(startStopButton.get());
    mainLayout->addWidget(quitButton.get());
    mainLayout->addWidget(toggleSatellitesButton.get());
    mainLayout->addLayout(speedLayout);
    setLayout(mainLayout);

    connect(startStopButton.get(), &QPushButton::clicked, this, &PlanetControlWidget::onStartStopClicked);
    connect(quitButton.get(), &QPushButton::clicked, this, &PlanetControlWidget::onQuitClicked);
    connect(toggleSatellitesButton.get(), &QPushButton::clicked, this, &PlanetControlWidget::onToggleSatellitesClicked);
    connect(speedSlider.get(), QOverload<int>::of(&QSlider::valueChanged), this, &PlanetControlWidget::onSimulationSpeedChanged);
    
    // Setup timer for updating simulation time display (every 100ms)
    timeUpdateTimer = std::make_unique<QTimer>(this);
    connect(timeUpdateTimer.get(), &QTimer::timeout, this, &PlanetControlWidget::updateSimulationTime);
    timeUpdateTimer->start(100);  // Update every 100ms
    updateSimulationTime();
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

void PlanetControlWidget::onToggleSatellitesClicked()
{
    if (!system) return;
    bool nowVisible = !system->getShowSatellites();
    system->setShowSatellites(nowVisible);
    toggleSatellitesButton->setText(nowVisible ? "Hide Satellites" : "Show Satellites");
}

void PlanetControlWidget::updateSimulationTime()
{
    if (!system) return;
    QString dateTimeStr = system->getSimulationDateTime().toString("yyyy-MM-dd hh:mm:ss");
    timeLabel->setText(QString("Date/Time: %1").arg(dateTimeStr));
}