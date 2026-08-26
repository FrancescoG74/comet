#pragma once
#include <QObject>
#include <QPoint>
#include "solarsystem.h"

class SolarSystem;
class QKeyEvent;
class QMouseEvent;
class QWheelEvent;

class SolarSystemController : public QObject {
    Q_OBJECT
public:
    SolarSystemController(SolarSystem* system);
    void handleKeyPress(QKeyEvent* event);
    void handleMousePress(QMouseEvent* event);
    void handleMouseMove(QMouseEvent* event);
    void handleMouseRelease(QMouseEvent* event);
    void handleWheel(QWheelEvent* event);
private:
    SolarSystem* system;
};
