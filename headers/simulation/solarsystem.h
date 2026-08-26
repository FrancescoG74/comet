#pragma once
#include <QWidget>
#include <QTimer>
#include <QVector>
#include <QDateTime>
#include <QTimeZone>
#include <memory>
#include "astronomicalbody.h"
#include "astronomy.h"

class SolarSystemController;
class PlanetControlWidget;

class SolarSystem : public QWidget {
    Q_OBJECT
public:
    SolarSystem(QWidget *parent = nullptr);
    void setControlWidget(PlanetControlWidget* widget) { controlWidget = widget; }
    void initBodies();
    void keyPressEvent(QKeyEvent *event) override;
    void setZoomFactor(double zf) { zoomFactor = zf; }
    void setDraggingSun(bool dragging) { draggingSun = dragging; }
    void setDragOffset(const QVector3D& offset) { dragOffset = offset; }
    void setLastMousePos(const QPoint& pos) { lastMousePos = pos; }
    void setRotatingView(bool rotating) { rotatingView = rotating; }
    void setSunPosition(const QVector3D& pos) { sun.setPosition(pos); }
    void setViewPitch(double pitch) { viewPitch = pitch; }
    void setViewYaw(double yaw) { viewYaw = yaw; }
    void setSunMass(double mass) { sun.setMass(mass); }
    void setSimulationActive(bool active) { simulationActive = active; }
    void setSimulationSpeedMultiplier(double multiplier) { speedMultiplier = std::max(0.1, multiplier); }
    void setCameraOffset(const QVector2D& offset) { cameraOffset = offset; }
    void setShowSatellites(bool show) { showSatellites = show; update(); }
    void resetView() {
        zoomFactor = 0.7;
        viewPitch = -35.0;
        viewYaw = 15.0;
        cameraOffset = QVector2D(0, 0);
    }
    double getZoomFactor() const { return zoomFactor; }
    QVector2D getCameraOffset() const { return cameraOffset; }
    QVector3D getSunPosition() const { return sun.getPosition(); }
    double getSunRadius() const { return sun.getRadius(); }
    QPoint getLastMousePos() const { return lastMousePos; }
    QVector3D getDragOffset() const { return dragOffset; }
    bool getDraggingSun() const { return draggingSun; }
    double getViewPitch() const { return viewPitch; }
    double getViewYaw() const { return viewYaw; }
    bool getRotatingView() const { return rotatingView; }
    double getSunMass() const { return sun.getMass(); }
    bool getSimulationActive() const { return simulationActive; }    bool getShowSatellites() const { return showSatellites; }
    // Current simulated date/time, driven by real ephemeris (Astronomy Engine).
    QDateTime getSimulationDateTime() const;

    PlanetControlWidget* controlWidget = nullptr;
protected:
    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
private:
    void handleKeyPress(QKeyEvent *event);
    void handleMouseRelease(QMouseEvent *event);
    void handleMouseMove(QMouseEvent *event);
    void handleMousePress(QMouseEvent *event);
    void handleWheel(QWheelEvent *event);
    void advance();
    void updateCelestialPositions();
    // Converts a real heliocentric distance (AU) to a pixel distance using the same
    // logarithmic compression for both actual body positions and drawn orbit paths.
    double screenAuDistance(double au) const;
    Sun sun;
    QVector<Planet> planets;
    QVector<Satellite> satellites;  // Moons orbiting planets
    std::unique_ptr<QTimer> timer;
    int elapsed = 0;
    bool draggingSun = false;
    QVector3D dragOffset;
    bool simulationActive = false;
    bool showSatellites = true;
    double zoomFactor = 1.0;
    double speedMultiplier = 1.0;
    // Real-world date/time the simulation started from, and simulated days elapsed since then.
    astro_time_t epochTime{};
    double simDaysElapsed = 0.0;
    // Sun's display radius in pixels (pre-zoom), kept proportionate to Mercury's orbit distance.
    double sunDisplayRadius = 0.0;
    // 3D visual rotation
    double viewYaw = 0.0;   // rotation around Y (horizontal)
    double viewPitch = 0.0; // rotation around X (vertical)
    bool rotatingView = false;
    QPoint lastMousePos;
    QVector2D cameraOffset = QVector2D(0, 0);  // Camera pan offset in screen space
    std::unique_ptr<SolarSystemController> controller;
};
