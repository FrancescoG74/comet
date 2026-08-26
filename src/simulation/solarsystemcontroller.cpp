#include "astronomicalbody.h"
#include "solarsystemcontroller.h"
#include "solarsystem.h"
#include "planetcontrolwidget.h"
#include "solarsimconstants.h"
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
        // Update button state when spacebar is pressed
        if (system->controlWidget) {
            system->controlWidget->updateButtonState(system->getSimulationActive());
        }
    } else if (event->key() == Qt::Key_R) {
        system->setSimulationActive(false);
        system->initBodies();
        system->update();
        // Update button state when reset
        if (system->controlWidget) {
            system->controlWidget->updateButtonState(false);
        }
    } else if (event->key() == Qt::Key_V) {
        // Reset view to overview (V for View)
        system->resetView();
        system->update();
    }
    // Ignore unhandled keys - don't propagate back to avoid infinite recursion
    event->ignore();
}
void SolarSystemController::handleMouseRelease(QMouseEvent *event) {
    if (!system) return;
    if (event->button() == Qt::LeftButton) {
        // Left button released: stop camera panning
        system->setDraggingSun(false);
    } else if (event->button() == Qt::RightButton) {
        // Right button released: stop camera rotation
        system->setRotatingView(false);
    }
}
void SolarSystemController::handleMouseMove(QMouseEvent *event) {
    if (!system) return;
    if (system->getRotatingView()) {
        // Right button: Rotate the camera view
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
        // Left button: Translate/pan the camera in screen space
        QPoint curPos = event->pos();
        int dx = curPos.x() - system->getLastMousePos().x();
        int dy = curPos.y() - system->getLastMousePos().y();
        
        // Update camera offset (pan offset in screen space)
        QVector2D currentOffset = system->getCameraOffset();
        QVector2D newOffset = currentOffset + QVector2D(dx, dy);
        system->setCameraOffset(newOffset);
        
        system->setLastMousePos(curPos);
        system->update();
    }
}
void SolarSystemController::handleMousePress(QMouseEvent *event) {
    if (!system) return;
    if (event->button() == Qt::LeftButton) {
        // Left button: Translate/pan the camera
        system->setDraggingSun(true);  // Reuse the flag for camera panning
        system->setLastMousePos(event->pos());
    } else if (event->button() == Qt::RightButton) {
        // Right button: Rotate the camera/view
        system->setRotatingView(true);
        system->setLastMousePos(event->pos());
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
