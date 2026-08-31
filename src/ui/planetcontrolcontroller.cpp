#include <QApplication>
#include <QTimer>
#include <QDateTime>

#include "planetcontrolcontroller.h"
#include "planetcontrolwidget.h"
#include "solarsystem.h"

PlanetControlController::PlanetControlController(SolarSystem* system, PlanetControlWidget* view, QObject* parent)
    : QObject(parent), system(system), view(view) {
    connect(view, &PlanetControlWidget::startStopClicked, this, &PlanetControlController::onStartStopClicked);
    connect(view, &PlanetControlWidget::quitClicked, this, &PlanetControlController::onQuitClicked);
    connect(view, &PlanetControlWidget::toggleSatellitesClicked, this, &PlanetControlController::onToggleSatellitesClicked);
    connect(view, &PlanetControlWidget::speedSliderChanged, this, &PlanetControlController::onSpeedSliderChanged);

    // Setup timer for updating simulation time display (every 100ms)
    timeUpdateTimer = std::make_unique<QTimer>(this);
    connect(timeUpdateTimer.get(), &QTimer::timeout, this, &PlanetControlController::updateSimulationTime);
    timeUpdateTimer->start(100);
    updateSimulationTime();
}

void PlanetControlController::onStartStopClicked()
{
    if (!system) return;
    isRunning = !isRunning;
    system->setSimulationActive(isRunning);
    view->setStartStopRunning(isRunning);
}

void PlanetControlController::updateButtonState(bool running)
{
    isRunning = running;
    view->setStartStopRunning(running);
}

void PlanetControlController::onSpeedSliderChanged(int value)
{
    // Convert slider value (10-2000) to speed multiplier (0.1x - 20x)
    double speedMultiplier = value / 100.0;
    if (system) {
        system->setSimulationSpeedMultiplier(speedMultiplier);
    }
    view->setSpeedLabelText(QString("Speed: %1x").arg(speedMultiplier, 0, 'f', 1));
}

void PlanetControlController::onQuitClicked()
{
    QApplication::quit();
}

void PlanetControlController::onToggleSatellitesClicked()
{
    if (!system) return;
    bool nowVisible = !system->getShowSatellites();
    system->setShowSatellites(nowVisible);
    view->setSatellitesVisibleLabel(nowVisible);
}

void PlanetControlController::updateSimulationTime()
{
    if (!system) return;
    QString dateTimeStr = system->getSimulationDateTime().toString("yyyy-MM-dd hh:mm:ss");
    view->setTimeText(dateTimeStr);
}
