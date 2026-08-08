#pragma once
#include <QWidget>
#include <memory>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
class SolarSystem;

class PlanetControlWidget : public QWidget {
    Q_OBJECT
public:
    PlanetControlWidget(SolarSystem* solarSystem, QWidget* parent = nullptr);
    void updateButtonState(bool isRunning);
private slots:
    void onAddPlanetClicked();
    void onRemovePlanetClicked();
    void onStartStopClicked();
    void onQuitClicked();
    void onSimulationSpeedChanged(int value);
private:
    std::unique_ptr<QPushButton> addButton;
    std::unique_ptr<QPushButton> removeButton;
    std::unique_ptr<QPushButton> startStopButton;
    std::unique_ptr<QPushButton> quitButton;
    std::unique_ptr<QSlider> speedSlider;
    std::unique_ptr<QLabel> speedLabel;
    SolarSystem* system;  // non-owning pointer
    bool isRunning = false;

};
