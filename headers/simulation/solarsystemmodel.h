#pragma once
#include <QVector>
#include <QDateTime>
#include "astronomicalbody.h"
#include "astronomy.h"

// Simulation/physics model: owns the Sun/planets/satellites and the simulated clock, and
// knows how to advance them using real ephemeris data (Astronomy Engine). Deliberately not a
// QWidget - it has no painting code and no knowledge of input handling, so it can be unit
// tested and reused independently of the view that renders it.
class SolarSystemModel {
public:
    // viewWidth/viewHeight are only used to place bodies in the same pixel space the view
    // renders in (Sun centered on the window, planets/moons offset from it in screen pixels).
    void initBodies(int viewWidth, int viewHeight);
    void advance(int viewWidth, int viewHeight);
    void updateCelestialPositions(int viewWidth, int viewHeight);
    double screenAuDistance(int viewWidth, double au) const;
    QDateTime getSimulationDateTime() const;

    const Sun& getSun() const { return sun; }
    const QVector<Planet>& getPlanets() const { return planets; }
    const QVector<Satellite>& getSatellites() const { return satellites; }
    // Sun's display radius in pixels (pre-zoom), kept proportionate to Mercury's orbit distance.
    double getSunDisplayRadius() const { return sunDisplayRadius; }

    void setSunPosition(const QVector3D& pos) { sun.setPosition(pos); }
    void setSunMass(double mass) { sun.setMass(mass); }
    double getSunMass() const { return sun.getMass(); }

    bool isSimulationActive() const { return simulationActive; }
    void setSimulationActive(bool active) { simulationActive = active; }
    void setSimulationSpeedMultiplier(double multiplier);
    double getSimulationSpeedMultiplier() const { return speedMultiplier; }
    bool getShowSatellites() const { return showSatellites; }
    void setShowSatellites(bool show) { showSatellites = show; }

private:
    Sun sun;
    QVector<Planet> planets;
    QVector<Satellite> satellites;
    bool simulationActive = false;
    bool showSatellites = true;
    double speedMultiplier = 1.0;
    // Real-world date/time the simulation started from, and simulated days elapsed since then.
    astro_time_t epochTime{};
    double simDaysElapsed = 0.0;
    double sunDisplayRadius = 0.0;
    int elapsed = 0;
};
