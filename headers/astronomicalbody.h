#pragma once
#include <QVector3D>
#include <QColor>

class AstronomicalBody {
public:
    AstronomicalBody() = default;
    AstronomicalBody(const QVector3D& pos, const QVector3D& vel, double mass, double radius, QColor color);
    QVector3D getPosition() const { return pos; }
    QVector3D getVelocity() const { return vel; }
    double getMass() const { return mass; }
    double getRadius() const { return radius; }
    QColor getColor() const { return color; }
    QVector3D pos;
    QVector3D vel;
    double mass;
    double radius;
    QColor color;
};
