#include <catch2/catch_test_macros.hpp>
#include <QGuiApplication>

// ============================================================================
// Test Initialization
// ============================================================================
// This file initializes the QGuiApplication required for QPixmap support
// in headless test environments.
//
// Test files are organized by domain:
// - tests/core/test_astronomicalbody.cpp       (Tests 1-50: Core hierarchy)
// - tests/simulation/test_physics.cpp          (Tests 51-63: Physics/constants)
// - tests/ui/                                   (UI tests placeholder)
// ============================================================================

static int argc = 1;
static char argv0[] = "comet_tests";
static char* argv[] = {argv0, nullptr};
static QGuiApplication app(argc, argv);
