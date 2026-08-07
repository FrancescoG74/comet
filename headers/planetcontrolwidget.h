#pragma once
#include <QWidget>
class QPushButton;
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
    QPushButton* addButton;
    QPushButton* removeButton;
    QPushButton* startStopButton;
    SolarSystem* system;
    bool isRunning = false;

};
