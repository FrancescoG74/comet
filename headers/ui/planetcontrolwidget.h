#pragma once
#include <QWidget>
#include <memory>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QDateTime>
#include <QTimer>
class SolarSystem;

class PlanetControlWidget : public QWidget {
    Q_OBJECT
public:
    PlanetControlWidget(SolarSystem* solarSystem, QWidget* parent = nullptr);
    void updateButtonState(bool isRunning);
private slots:
    void onStartStopClicked();
    void onQuitClicked();
    void onSimulationSpeedChanged(int value);
    void onToggleSatellitesClicked();
    void updateSimulationTime();
private:
    std::unique_ptr<QLabel> timeLabel;
    std::unique_ptr<QPushButton> startStopButton;
    std::unique_ptr<QPushButton> quitButton;
    std::unique_ptr<QPushButton> toggleSatellitesButton;
    std::unique_ptr<QSlider> speedSlider;
    std::unique_ptr<QLabel> speedLabel;
    std::unique_ptr<QTimer> timeUpdateTimer;
    SolarSystem* system;  // non-owning pointer
    bool isRunning = false;

};
