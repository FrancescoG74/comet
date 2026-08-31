#include <catch2/catch_test_macros.hpp>
#include <QApplication>

// ============================================================================
// Test Initialization
// ============================================================================
// This file initializes the QApplication required for QPixmap support and
// QWidget-based UI tests in headless test environments.
//
// Test files are organized by domain:
// - tests/core/test_astronomicalbody.cpp       (Tests 1-50: Core hierarchy)
// - tests/simulation/test_physics.cpp          (Tests 51-63: Physics/constants)
// - tests/ui/                                   (PlanetControlWidget/Controller tests)
// ============================================================================

static int argc = 1;
static char argv0[] = "comet_tests";
static char* argv[] = {argv0, nullptr};
static QApplication app(argc, argv);
