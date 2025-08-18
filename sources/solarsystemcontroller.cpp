#include "astronomicalbody.h"
#include "solarsystemcontroller.h"
#include "solarsystem.h"
#include "../headers/solarsimconstants.h"
#include <QWheelEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QApplication>
#include <algorithm>

SolarSystemController::SolarSystemController(SolarSystem* system)
    : QObject(system), system(system) {}

void SolarSystemController::handleKeyPress(QKeyEvent *event) {
    if (!system) return;
    // Release dragging if ESC or Q is pressed
    system->setDraggingSun(false);
    if (event->key() == Qt::Key_Escape || event->key() == Qt::Key_Q) {
        QApplication::quit();
    } else if (event->key() == Qt::Key_Space) {
        system->setSimulationActive(!system->getSimulationActive());
    } else if (event->key() == Qt::Key_R) {
        system->setSimulationActive(false);
        system->initBodies();
        system->update();
    } else {
        system->keyPressEvent(event);
    }
}
void SolarSystemController::handleMouseRelease(QMouseEvent *event) {
    if (!system) return;
    if (event->button() == Qt::RightButton) {
        system->setRotatingView(false);
    }
    if (system->getDraggingSun()) {
        struct OrbitalParams { double r; double angle_deg; double v; QColor color; double radius; };
    // Orbital parameters
    double a = (system->width()/2) - SolarSimConstants::MARGIN; // semi-major axis
    double b = (system->height()/2) - SolarSimConstants::MARGIN; // semi-minor axis
    double e = std::sqrt(1.0 - (b*b)/(a*a)); // eccentricity
    // Sun in one of the foci
        QPointF center(system->width()/2, system->height()/2);
        double c = e * a;
    system->setSunPosition(QVector3D(center.x() - c, center.y(), 0)); // left focus
        struct PlanetParams {
            double a, b, angle_deg, z, vz;
            QColor color;
            double mass, radius;
        };
        std::vector<PlanetParams> planetParams = {
            { a, b, 30, 0, 0.5, QColor(70, 120, 255), SolarSimConstants::PLANET_MASS, SolarSimConstants::PLANET_RADIUS },      // blue
            { a * 0.7, b * 0.7, 120, 60, -0.3, QColor(120, 255, 120), SolarSimConstants::PLANET_MASS * 0.7, SolarSimConstants::PLANET_RADIUS * 0.8 }, // green
            { a * 1.2, b * 1.2, 210, -80, 0.2, QColor(255, 180, 80), SolarSimConstants::PLANET_MASS * 1.2, SolarSimConstants::PLANET_RADIUS * 1.1 }    // orange
        };
        system->setSunMass(SolarSimConstants::SUN_MASS);
        double M = system->getSunMass();
        for (const auto& p : planetParams) {
            double rad = qDegreesToRadians(p.angle_deg);
            QVector3D pos = QVector3D(center.x() + p.a * std::cos(rad), center.y() + p.b * std::sin(rad), p.z);
            double r = std::sqrt(std::pow(pos.x() - system->getSunPosition().x(), 2) + std::pow(pos.y() - system->getSunPosition().y(), 2) + std::pow(pos.z() - system->getSunPosition().z(), 2));
            double v = std::sqrt(SolarSimConstants::G * M * (2.0/r - 1.0/p.a));
            double tx = -p.a * std::sin(rad);
            double ty =  p.b * std::cos(rad);
            double tz = 0;
            double norm = std::sqrt(tx*tx + ty*ty + tz*tz);
            QVector3D tangent(tx/norm, ty/norm, p.vz);
            QVector3D tangentNorm = tangent.normalized();
            QVector3D vel = tangentNorm * v;
            system->appendPlanet(AstronomicalBody(pos, vel, p.mass, p.radius, p.color));
        }
        system->setDraggingSun(false);
    }
}
void SolarSystemController::handleMouseMove(QMouseEvent *event) {
    if (!system) return;
    if (system->getRotatingView()) {
        QPoint curPos = event->pos();
        int dx = curPos.x() - system->getLastMousePos().x();
        int dy = curPos.y() - system->getLastMousePos().y();
        system->setViewYaw(system->getViewYaw() + dx * 0.5) ;
        system->setViewPitch(system->getViewPitch() + dy * 0.5);
        if (system->getViewPitch() > 89.0) system->setViewPitch(89.0);
        if (system->getViewPitch() < -89.0) system->setViewPitch(-89.0);
        system->setLastMousePos(curPos);
        system->update();
    } else if (system->getDraggingSun()) {
        QPointF center(system->width()/2, system->height()/2);
        QPointF mousePos = event->pos();
        QPointF world2D = (mousePos - center) / system->getZoomFactor() + center;
        QVector3D worldPos(world2D.x(), world2D.y(), system->getSunPosition().z());
        system->setSunPosition(worldPos + system->getDragOffset());
    // Recalculate the initial conditions of the planets with respect to the new position of the sun
        QVector3D oldSunPos = system->getSunPosition();
        system->initBodies();
        system->setSunPosition(oldSunPos);
        system->update();
    }
}
void SolarSystemController::handleMousePress(QMouseEvent *event) {
    if (!system) return;
    if (event->button() == Qt::RightButton) {
        system->setRotatingView(true);
        system->setLastMousePos(event->pos());
    } else {
    // Transform the mouse position into "world" coordinates considering the zoom
        QPointF center(system->width()/2, system->height()/2);
        QPointF mousePos = event->pos();
        QPointF world2D = (mousePos - center) / system->getZoomFactor() + center;
        QVector3D worldPos(world2D.x(), world2D.y(), system->getSunPosition().z());
        double dist = (system->getSunPosition() - worldPos).length();
        if (dist <= system->getSunRadius()) {
            system->setDraggingSun(true);
            system->setDragOffset(system->getSunPosition() - worldPos);
        }
    }
}
void SolarSystemController::handleWheel(QWheelEvent *event) {
    if (!system) return;
    constexpr double zoomStep = 1.15;
    double newZoom = system->getZoomFactor();
    if (event->angleDelta().y() > 0)
        newZoom *= zoomStep;
    else if (event->angleDelta().y() < 0)
        newZoom /= zoomStep;
    system->setZoomFactor(newZoom);
}
