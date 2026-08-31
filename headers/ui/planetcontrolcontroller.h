#pragma once
#include <QObject>
#include <memory>

class QTimer;
class SolarSystem;
class PlanetControlWidget;

// Controller mediating between the PlanetControlWidget view and SolarSystem/the model: it is
// the only class allowed to call into SolarSystem on the panel's behalf, and the only one
// that pushes display updates into the view.
class PlanetControlController : public QObject {
    Q_OBJECT
public:
    PlanetControlController(SolarSystem* system, PlanetControlWidget* view, QObject* parent = nullptr);
    // Called by SolarSystemController (e.g. spacebar toggle, reset) to keep the view in sync.
    void updateButtonState(bool running);
private slots:
    void onStartStopClicked();
    void onQuitClicked();
    void onSpeedSliderChanged(int value);
    void onToggleSatellitesClicked();
    void updateSimulationTime();
private:
    SolarSystem* system;         // non-owning
    PlanetControlWidget* view;   // non-owning
    bool isRunning = false;
    std::unique_ptr<QTimer> timeUpdateTimer;
};
