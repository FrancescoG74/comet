#pragma once
#include <QWidget>
#include <memory>
#include <QPushButton>
class SolarSystem;

class PlanetControlWidget : public QWidget {
    Q_OBJECT
public:
    PlanetControlWidget(SolarSystem* solarSystem, QWidget* parent = nullptr);
private slots:
    void onAddPlanetClicked();
    void onRemovePlanetClicked();
    void onStartStopClicked();
private:
    std::unique_ptr<QPushButton> addButton;
    std::unique_ptr<QPushButton> removeButton;
    std::unique_ptr<QPushButton> startStopButton;
    SolarSystem* system;  // non-owning pointer
    bool isRunning = false;

};
