#include <algorithm>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QMatrix4x4>
#include <QApplication>
#include <QKeyEvent>
#include <QtMath>

#include "../headers/solarsimconstants.h"
#include "solarsystem.h"
#include "solarsystemcontroller.h"

void SolarSystem::wheelEvent(QWheelEvent *event) {
    handleWheel(event);
}
void SolarSystem::handleWheel(QWheelEvent *event) {
    if (controller) {
        controller->handleWheel(event);
    }
}
void SolarSystem::mousePressEvent(QMouseEvent *event) {
    handleMousePress(event);
}
void SolarSystem::handleMousePress(QMouseEvent *event) {
    if (controller) {
        controller->handleMousePress(event);
    }
}

void SolarSystem::mouseMoveEvent(QMouseEvent *event) {
    handleMouseMove(event);
}
void SolarSystem::handleMouseMove(QMouseEvent *event) {
    if (controller) {
        controller->handleMouseMove(event);
    }
}

SolarSystem::SolarSystem(QWidget *parent) : QWidget(parent) {
    setFixedSize(SolarSimConstants::WINDOW_WIDTH, SolarSimConstants::WINDOW_HEIGHT);
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        if (simulationActive) {
            for (int i = 0; i < SolarSimConstants::STEPS_PER_FRAME; ++i) advance();
        }
        update();
    });
    timer->start(SolarSimConstants::TIMER_INTERVAL_MS);
    setFocusPolicy(Qt::StrongFocus);
    controller = new SolarSystemController(this);
    initBodies();
}

void SolarSystem::paintEvent(QPaintEvent *) {
    // DEBUG: Print number of planets being drawn
    // qDebug("[DEBUG] Number of planets: %d", planets.size());
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    // Draw black background
    p.fillRect(rect(), Qt::black);
    // Draw fixed stars
    static const QPoint starPositions[] = {
        {50, 60}, {200, 120}, {400, 80}, {700, 200}, {900, 100},
        {1000, 700}, {800, 600}, {600, 500}, {300, 700}, {150, 400},
        {500, 300}, {900, 400}, {750, 100}, {250, 650}, {950, 650}
    };
    p.setPen(QPen(Qt::white, 2));
    for (const QPoint& pt : starPositions) {
        p.drawPoint(pt);
    }
    // Apply zoom and 3D rotation
    p.save();
    QPointF center(width()/2, height()/2);
    // 3D rotation matrix (yaw, pitch)
    QMatrix4x4 rot;
    rot.rotate(viewYaw, 0, 1, 0);
    rot.rotate(viewPitch, 1, 0, 0);
    // Draw 3D axes (X=red, Y=green, Z=blue) from the sun's position
    const double axisLength = 120.0;
    QVector3D origin3D = sun.pos;
    QVector3D xAxis3D = origin3D + QVector3D(axisLength, 0, 0);
    QVector3D yAxis3D = origin3D + QVector3D(0, axisLength, 0);
    QVector3D zAxis3D = origin3D + QVector3D(0, 0, axisLength);
    QVector3D axes3D[4] = {origin3D, xAxis3D, yAxis3D, zAxis3D};
    QPointF axes2D[4];
    for (int i = 0; i < 4; ++i) {
        QVector3D p3 = rot.map(axes3D[i] - QVector3D(width()/2, height()/2, 0));
        axes2D[i] = QPointF(p3.x(), p3.y()) * zoomFactor + QPointF(width()/2, height()/2);
    }
    QPen oldPen = p.pen();
    QPen xPen(Qt::red, 2);
    QPen yPen(Qt::green, 2);
    QPen zPen(Qt::blue, 2);
    p.setPen(xPen);
    p.drawLine(axes2D[0], axes2D[1]);
    p.setPen(yPen);
    p.drawLine(axes2D[0], axes2D[2]);
    p.setPen(zPen);
    p.drawLine(axes2D[0], axes2D[3]);
    p.setPen(oldPen);
    // Draw Sun (3D->2D projection with depth)
    QVector3D spos = rot.map(sun.pos - QVector3D(width()/2, height()/2, 0));
    QPointF sun2D = QPointF(spos.x(), spos.y()) * zoomFactor + QPointF(width()/2, height()/2);
    double sunDepth = 1.0 / (1.0 + 0.002 * spos.z());
    double sunRadius = sun.radius * zoomFactor * sunDepth;
    QColor sunColor = sun.color;
    sunColor = sunColor.lighter(100 + int(-spos.z()));
    p.setBrush(sunColor);
    p.drawEllipse(sun2D, sunRadius, sunRadius);
    // Draw Planets (3D->2D projection with depth and gradient)
    for (const auto& planet : planets) {
        QVector3D ppos = rot.map(planet.pos - QVector3D(width()/2, height()/2, 0));
        QPointF planet2D = QPointF(ppos.x(), ppos.y()) * zoomFactor + QPointF(width()/2, height()/2);
        double depth = 1.0 / (1.0 + 0.002 * ppos.z());
        double pradius = planet.radius * zoomFactor * depth;

    // Direction from planet to sun in 2D (screen space)
        QPointF sun2D = QPointF(sun.pos.x(), sun.pos.y());
        QPointF planet2Dpos = QPointF(planet.pos.x(), planet.pos.y());
        QPointF dir = sun2D - planet2Dpos;
        double len = std::sqrt(dir.x()*dir.x() + dir.y()*dir.y());
        QPointF gradCenter = planet2D;
        if (len > 1e-3) {
            QPointF offset = dir / len * pradius * 0.5; // move gradient center toward sun
            gradCenter = planet2D + offset;
        }
    QRadialGradient grad(gradCenter, pradius, gradCenter);
    grad.setColorAt(0.0, planet.color); // planet's color at sun-facing side
    grad.setColorAt(1.0, Qt::black);    // black at shadow side
    p.setBrush(grad);
    p.drawEllipse(planet2D, pradius, pradius);
    }
    // (end of paintEvent)
    p.restore();
}


void SolarSystem::advance() {
    for (auto& planet : planets) {
        QVector3D r = sun.pos - planet.pos;
        double dist = r.length();
        if (dist < 1) dist = 1;
        double force = SolarSimConstants::G * sun.mass * planet.mass / (dist * dist);
        QVector3D acc = r.normalized() * (force / planet.mass);
        planet.vel += acc * SolarSimConstants::TIME_STEP;
        planet.pos += planet.vel * SolarSimConstants::TIME_STEP;
    }
    elapsed += 16;
    update();
}

void SolarSystem::keyPressEvent(QKeyEvent *event) {
    handleKeyPress(event);
}
void SolarSystem::handleKeyPress(QKeyEvent *event) {
    if (controller) {
        controller->handleKeyPress(event);
    }
}

void SolarSystem::initBodies() {
    sun = AstronomicalBody(QVector3D(width()/2, height()/2, 0), QVector3D(0,0,0), SolarSimConstants::SUN_MASS, SolarSimConstants::SUN_RADIUS, Qt::yellow);
    planets.clear();
    // Orbital parameters
    double a = (width()/2) - SolarSimConstants::MARGIN; // semi-major axis
    double b = (height()/2) - SolarSimConstants::MARGIN; // semi-minor axis
    double e = std::sqrt(1.0 - (b*b)/(a*a)); // eccentricity
    // Sun in one of the foci
    QPointF center(width()/2, height()/2);
    double c = e * a;
    sun.pos = QVector3D(center.x() - c, center.y(), 0); // left focus
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
    sun.mass = SolarSimConstants::SUN_MASS;
    double M = sun.mass;
    for (const auto& p : planetParams) {
        double rad = qDegreesToRadians(p.angle_deg);
        QVector3D pos = QVector3D(center.x() + p.a * std::cos(rad), center.y() + p.b * std::sin(rad), p.z);
        double r = std::sqrt(std::pow(pos.x() - sun.pos.x(), 2) + std::pow(pos.y() - sun.pos.y(), 2) + std::pow(pos.z() - sun.pos.z(), 2));
        double v = std::sqrt(SolarSimConstants::G * M * (2.0/r - 1.0/p.a));
        double tx = -p.a * std::sin(rad);
        double ty =  p.b * std::cos(rad);
        double tz = 0;
        double norm = std::sqrt(tx*tx + ty*ty + tz*tz);
        QVector3D tangent(tx/norm, ty/norm, p.vz);
        QVector3D tangentNorm = tangent.normalized();
        QVector3D vel = tangentNorm * v;
        planets.append(AstronomicalBody(pos, vel, p.mass, p.radius, p.color));
    }
}

void SolarSystem::mouseReleaseEvent(QMouseEvent *event) {
    handleMouseRelease(event);
}
void SolarSystem::handleMouseRelease(QMouseEvent *event) {
    if (controller) {
        controller->handleMouseRelease(event);
    }
}
