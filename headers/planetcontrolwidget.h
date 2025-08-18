#pragma once
//#include <QWidget>
class QPushButton;
class SolarSystem;

class PlanetControlWidget : public QWidget {
//    Q_OBJECT
public:
    PlanetControlWidget(SolarSystem* solarSystem, QWidget* parent = nullptr);
signals:
    void addPlanetRequested();
    void removePlanetRequested();
private:
    QPushButton* addButton;
    QPushButton* removeButton;
    SolarSystem* system;

};
