#pragma once
#include <QWidget>
#include <QTimer>
#include <QVector>
#include "astronomicalbody.h"

class SolarSystemController;

class SolarSystem : public QWidget {
//    Q_OBJECT
public:
    SolarSystem(QWidget *parent = nullptr);
    void initBodies();
    void keyPressEvent(QKeyEvent *event) override;
    void setZoomFactor(double zf) { zoomFactor = zf; }
    void setDraggingSun(bool dragging) { draggingSun = dragging; }
    void setDragOffset(const QVector3D& offset) { dragOffset = offset; }
    void setLastMousePos(const QPoint& pos) { lastMousePos = pos; }
    void setRotatingView(bool rotating) { rotatingView = rotating; }
    void setSunPosition(const QVector3D& pos) { sun.pos = pos; }
    void setViewPitch(double pitch) { viewPitch = pitch; }
    void setViewYaw(double yaw) { viewYaw = yaw; }
    void setSunMass(double mass) { sun.mass = mass; }
    void setSimulationActive(bool active) { simulationActive = active; }
    double getZoomFactor() const { return zoomFactor; }
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

    void appendPlanet(const AstronomicalBody& planet) { planets.append(planet); }
    void popBackPlanet() { planets.removeLast(); }
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
    AstronomicalBody sun;
    QVector<AstronomicalBody> planets;
    QTimer *timer;
    int elapsed = 0;
    bool draggingSun = false;
    QVector3D dragOffset;
    bool simulationActive = false;
    double zoomFactor = 1.0;
    // 3D visual rotation
    double viewYaw = 0.0;   // rotation around Y (horizontal)
    double viewPitch = 0.0; // rotation around X (vertical)
    bool rotatingView = false;
    QPoint lastMousePos;
    SolarSystemController* controller = nullptr;
};
