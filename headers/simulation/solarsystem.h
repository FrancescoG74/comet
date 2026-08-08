#pragma once
#include <QWidget>
#include <QTimer>
#include <QVector>
#include <memory>
#include "astronomicalbody.h"

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
    bool getSimulationActive() const { return simulationActive; }

    void appendPlanet(const Planet& planet) { planets.append(planet); }
    void popBackPlanet() { if (!planets.empty()) planets.removeLast(); }
    
    PlanetControlWidget* controlWidget = nullptr;
protected:
    void paintEvent(QPaintEvent *) override;
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
    Sun sun;
    QVector<Planet> planets;
    std::unique_ptr<QTimer> timer;
    int elapsed = 0;
    bool draggingSun = false;
    QVector3D dragOffset;
    bool simulationActive = false;
    double zoomFactor = 1.0;
    double speedMultiplier = 0.1;  // Start 10x slower
    // 3D visual rotation
    double viewYaw = 0.0;   // rotation around Y (horizontal)
    double viewPitch = 0.0; // rotation around X (vertical)
    bool rotatingView = false;
    QPoint lastMousePos;
    QVector2D cameraOffset = QVector2D(0, 0);  // Camera pan offset in screen space
    std::unique_ptr<SolarSystemController> controller;
};
