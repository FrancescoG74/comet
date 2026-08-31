#include <catch2/catch_test_macros.hpp>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include "solarsystem.h"
#include "solarsystemcontroller.h"
#include "planetcontrolwidget.h"
#include "planetcontrolcontroller.h"

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
// UI: PlanetControlController tests - drives the real view via simulated
// clicks and checks the effect on SolarSystem (the model, through its
// forwarding setters/getters), never bypassing the view/controller wiring.
// ============================================================================

TEST_CASE("PlanetControlController toggles simulation active and button text on start stop click", "[ui][planetcontrolcontroller]") {
    SolarSystem system;
    PlanetControlWidget widget;
    PlanetControlController controller(&system, &widget);

    REQUIRE_FALSE(system.getSimulationActive());
    findButtonByText(widget, "Start")->click();
    REQUIRE(system.getSimulationActive());
    REQUIRE(findButtonByText(widget, "Stop") != nullptr);

    findButtonByText(widget, "Stop")->click();
    REQUIRE_FALSE(system.getSimulationActive());
    REQUIRE(findButtonByText(widget, "Start") != nullptr);
}

TEST_CASE("PlanetControlController toggles satellite visibility and button text", "[ui][planetcontrolcontroller]") {
    SolarSystem system;
    PlanetControlWidget widget;
    PlanetControlController controller(&system, &widget);

    REQUIRE(system.getShowSatellites());
    findButtonByText(widget, "Hide Satellites")->click();
    REQUIRE_FALSE(system.getShowSatellites());
    REQUIRE(findButtonByText(widget, "Show Satellites") != nullptr);

    findButtonByText(widget, "Show Satellites")->click();
    REQUIRE(system.getShowSatellites());
    REQUIRE(findButtonByText(widget, "Hide Satellites") != nullptr);
}

TEST_CASE("PlanetControlController converts the slider value into a speed multiplier label", "[ui][planetcontrolcontroller]") {
    SolarSystem system;
    PlanetControlWidget widget;
    PlanetControlController controller(&system, &widget);

    QSlider* slider = widget.findChild<QSlider*>();
    REQUIRE(slider != nullptr);
    slider->setValue(250);  // 250/100 = 2.5x

    QLabel* speedLabel = findSpeedLabel(widget);
    REQUIRE(speedLabel != nullptr);
    REQUIRE(speedLabel->text() == "Speed: 2.5x");
}

TEST_CASE("PlanetControlController updateButtonState syncs the view without touching the model", "[ui][planetcontrolcontroller]") {
    SolarSystem system;
    PlanetControlWidget widget;
    PlanetControlController controller(&system, &widget);

    controller.updateButtonState(true);
    REQUIRE(findButtonByText(widget, "Stop") != nullptr);
    // updateButtonState only reflects external state changes into the view; it must not
    // itself flip the model (that already happened wherever the state change originated).
    REQUIRE_FALSE(system.getSimulationActive());
}

TEST_CASE("PlanetControlController populates the time label from the model on construction", "[ui][planetcontrolcontroller]") {
    SolarSystem system;
    PlanetControlWidget widget;
    PlanetControlController controller(&system, &widget);

    REQUIRE(widget.getTimeLabel()->text() != "--");
}
