#include <catch2/catch_test_macros.hpp>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include "planetcontrolwidget.h"

namespace {
QPushButton* findButtonByText(const PlanetControlWidget& widget, const QString& text) {
    for (auto* button : widget.findChildren<QPushButton*>()) {
        if (button->text() == text) return button;
    }
    return nullptr;
}

QLabel* findSpeedLabel(const PlanetControlWidget& widget) {
    for (auto* label : widget.findChildren<QLabel*>()) {
        if (label != widget.getTimeLabel()) return label;
    }
    return nullptr;
}
}

// ============================================================================
// UI: PlanetControlWidget (view) tests
// ============================================================================

TEST_CASE("PlanetControlWidget starts with placeholder time text", "[ui][planetcontrolwidget]") {
    PlanetControlWidget widget;
    REQUIRE(widget.getTimeLabel() != nullptr);
    REQUIRE(widget.getTimeLabel()->text() == "--");
}

TEST_CASE("PlanetControlWidget Start button click emits startStopClicked", "[ui][planetcontrolwidget]") {
    PlanetControlWidget widget;
    bool emitted = false;
    QObject::connect(&widget, &PlanetControlWidget::startStopClicked, [&]() { emitted = true; });
    QPushButton* startButton = findButtonByText(widget, "Start");
    REQUIRE(startButton != nullptr);
    startButton->click();
    REQUIRE(emitted);
}

TEST_CASE("PlanetControlWidget Quit button click emits quitClicked", "[ui][planetcontrolwidget]") {
    PlanetControlWidget widget;
    bool emitted = false;
    QObject::connect(&widget, &PlanetControlWidget::quitClicked, [&]() { emitted = true; });
    QPushButton* quitButton = findButtonByText(widget, "Quit");
    REQUIRE(quitButton != nullptr);
    quitButton->click();
    REQUIRE(emitted);
}

TEST_CASE("PlanetControlWidget satellites button click emits toggleSatellitesClicked", "[ui][planetcontrolwidget]") {
    PlanetControlWidget widget;
    bool emitted = false;
    QObject::connect(&widget, &PlanetControlWidget::toggleSatellitesClicked, [&]() { emitted = true; });
    QPushButton* toggleButton = findButtonByText(widget, "Hide Satellites");
    REQUIRE(toggleButton != nullptr);
    toggleButton->click();
    REQUIRE(emitted);
}

TEST_CASE("PlanetControlWidget speed slider change emits speedSliderChanged with the raw value", "[ui][planetcontrolwidget]") {
    PlanetControlWidget widget;
    int received = -1;
    QSlider* slider = widget.findChild<QSlider*>();
    REQUIRE(slider != nullptr);
    QObject::connect(&widget, &PlanetControlWidget::speedSliderChanged, [&](int value) { received = value; });
    slider->setValue(500);
    REQUIRE(received == 500);
}

TEST_CASE("PlanetControlWidget setStartStopRunning toggles the button text", "[ui][planetcontrolwidget]") {
    PlanetControlWidget widget;
    widget.setStartStopRunning(true);
    REQUIRE(findButtonByText(widget, "Stop") != nullptr);
    widget.setStartStopRunning(false);
    REQUIRE(findButtonByText(widget, "Start") != nullptr);
}

TEST_CASE("PlanetControlWidget setSatellitesVisibleLabel toggles the button text", "[ui][planetcontrolwidget]") {
    PlanetControlWidget widget;
    widget.setSatellitesVisibleLabel(false);
    REQUIRE(findButtonByText(widget, "Show Satellites") != nullptr);
    widget.setSatellitesVisibleLabel(true);
    REQUIRE(findButtonByText(widget, "Hide Satellites") != nullptr);
}

TEST_CASE("PlanetControlWidget setSpeedLabelText updates the speed label", "[ui][planetcontrolwidget]") {
    PlanetControlWidget widget;
    widget.setSpeedLabelText("Speed: 3.5x");
    QLabel* speedLabel = findSpeedLabel(widget);
    REQUIRE(speedLabel != nullptr);
    REQUIRE(speedLabel->text() == "Speed: 3.5x");
}

TEST_CASE("PlanetControlWidget setTimeText updates the time label", "[ui][planetcontrolwidget]") {
    PlanetControlWidget widget;
    widget.setTimeText("2026-08-31 12:00:00");
    REQUIRE(widget.getTimeLabel()->text() == "2026-08-31 12:00:00");
}
