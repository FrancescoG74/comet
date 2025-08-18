#include "../headers/astronomicalbody.h"

AstronomicalBody::AstronomicalBody(const QVector3D& pos, const QVector3D& vel, double mass, double radius, QColor color)
    : pos(pos), vel(vel), mass(mass), radius(radius), color(color) {}
