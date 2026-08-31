#pragma once
#include <QWidget>
#include <QTimer>
#include <QDateTime>
#include <memory>
#include "solarsystemmodel.h"

class SolarSystemController;
class PlanetControlController;

class SolarSystem : public QWidget {
    Q_OBJECT
public:
    SolarSystem(QWidget *parent = nullptr);
    void setControlController(PlanetControlController* controlPanel) { controlController = controlPanel; }
    void initBodies();
    void keyPressEvent(QKeyEvent *event) override;
    void setZoomFactor(double zf) { zoomFactor = zf; }
    void setDraggingSun(bool dragging) { draggingSun = dragging; }
    void setDragOffset(const QVector3D& offset) { dragOffset = offset; }
    void setLastMousePos(const QPoint& pos) { lastMousePos = pos; }
    void setRotatingView(bool rotating) { rotatingView = rotating; }
    void setSunPosition(const QVector3D& pos) { model.setSunPosition(pos); }
    void setViewPitch(double pitch) { viewPitch = pitch; }
    void setViewYaw(double yaw) { viewYaw = yaw; }
    void setSunMass(double mass) { model.setSunMass(mass); }
    void setSimulationActive(bool active) { model.setSimulationActive(active); }
    void setSimulationSpeedMultiplier(double multiplier) { model.setSimulationSpeedMultiplier(multiplier); }
    void setCameraOffset(const QVector2D& offset) { cameraOffset = offset; }
    void setShowSatellites(bool show) { model.setShowSatellites(show); update(); }
    void resetView() {
        zoomFactor = 0.7;
        viewPitch = -35.0;
        viewYaw = 15.0;
        cameraOffset = QVector2D(0, 0);
    }
    double getZoomFactor() const { return zoomFactor; }
    QVector2D getCameraOffset() const { return cameraOffset; }
    QVector3D getSunPosition() const { return model.getSun().getPosition(); }
    double getSunRadius() const { return model.getSun().getRadius(); }
    QPoint getLastMousePos() const { return lastMousePos; }
    QVector3D getDragOffset() const { return dragOffset; }
    bool getDraggingSun() const { return draggingSun; }
    double getViewPitch() const { return viewPitch; }
    double getViewYaw() const { return viewYaw; }
    bool getRotatingView() const { return rotatingView; }
    double getSunMass() const { return model.getSunMass(); }
    bool getSimulationActive() const { return model.isSimulationActive(); }
    bool getShowSatellites() const { return model.getShowSatellites(); }
    // Current simulated date/time, driven by real ephemeris (Astronomy Engine).
    QDateTime getSimulationDateTime() const { return model.getSimulationDateTime(); }

    PlanetControlController* controlController = nullptr;
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
    // Converts a real heliocentric distance (AU) to a pixel distance using the same
    // logarithmic compression for both actual body positions and drawn orbit paths.
    double screenAuDistance(double au) const { return model.screenAuDistance(width(), au); }
    // The view only reads simulation/physics state from the model (see paintEvent); all
    // mutation happens through the thin setters above, called by the controller/control widget.
    SolarSystemModel model;
    std::unique_ptr<QTimer> timer;
    bool draggingSun = false;
    QVector3D dragOffset;
    double zoomFactor = 1.0;
    // 3D visual rotation
    double viewYaw = 0.0;   // rotation around Y (horizontal)
    double viewPitch = 0.0; // rotation around X (vertical)
    bool rotatingView = false;
    QPoint lastMousePos;
    QVector2D cameraOffset = QVector2D(0, 0);  // Camera pan offset in screen space
    std::unique_ptr<SolarSystemController> controller;
};
