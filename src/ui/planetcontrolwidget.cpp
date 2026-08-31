#include <memory>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QFrame>

#include "planetcontrolwidget.h"

PlanetControlWidget::PlanetControlWidget(QWidget* parent)
    : QWidget(parent) {
    // Fix this panel's width so button/slider layout never nudges the top-level window's
    // geometry (the date/time label lives elsewhere now, in the top bar).
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    setFixedWidth(180);

    timeLabel = std::make_unique<QLabel>("--");
    timeLabel->setStyleSheet(
        "QLabel {"
        "  background-color: #222222;"
        "  color: #00FF00;"
        "  font-weight: bold;"
        "  font-size: 22px;"
        "  padding: 8px;"
        "  border: 1px solid #444444;"
        "  border-radius: 3px;"
        "}"
    );
    timeLabel->setAlignment(Qt::AlignCenter);
    timeLabel->setMinimumHeight(48);

    startStopButton = std::make_unique<QPushButton>("Start", this);
    quitButton = std::make_unique<QPushButton>("Quit", this);
    toggleSatellitesButton = std::make_unique<QPushButton>("Hide Satellites", this);
    speedSlider = std::make_unique<QSlider>(Qt::Horizontal, this);
    speedLabel = std::make_unique<QLabel>("Speed: 1.0x", this);
    
    // Configure speed slider (range: 10 to 2000, representing 0.1x to 20x speed)
    speedSlider->setMinimum(10);
    speedSlider->setMaximum(2000);
    speedSlider->setValue(100);  // Default: 1.0x speed
    speedSlider->setTickPosition(QSlider::TicksBelow);
    speedSlider->setTickInterval(200);
    
    // Create speed control layout
    auto* speedLayout = new QHBoxLayout();
    speedLayout->addWidget(speedLabel.get());
    speedLayout->addWidget(speedSlider.get());
    
    // Create main vertical layout (date/time label is placed by the caller, e.g. a top bar)
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(startStopButton.get());
    mainLayout->addWidget(quitButton.get());

    auto* divider = new QFrame(this);
    divider->setFrameShape(QFrame::HLine);
    divider->setFrameShadow(QFrame::Sunken);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(divider);
    mainLayout->addSpacing(10);

    mainLayout->addWidget(toggleSatellitesButton.get());
    mainLayout->addLayout(speedLayout);
    mainLayout->addStretch(1);
    setLayout(mainLayout);

    connect(startStopButton.get(), &QPushButton::clicked, this, &PlanetControlWidget::startStopClicked);
    connect(quitButton.get(), &QPushButton::clicked, this, &PlanetControlWidget::quitClicked);
    connect(toggleSatellitesButton.get(), &QPushButton::clicked, this, &PlanetControlWidget::toggleSatellitesClicked);
    connect(speedSlider.get(), QOverload<int>::of(&QSlider::valueChanged), this, &PlanetControlWidget::speedSliderChanged);
}

void PlanetControlWidget::setStartStopRunning(bool running)
{
    startStopButton->setText(running ? "Stop" : "Start");
}

void PlanetControlWidget::setSatellitesVisibleLabel(bool visible)
{
    toggleSatellitesButton->setText(visible ? "Hide Satellites" : "Show Satellites");
}

void PlanetControlWidget::setSpeedLabelText(const QString& text)
{
    speedLabel->setText(text);
}

void PlanetControlWidget::setTimeText(const QString& text)
{
    timeLabel->setText(text);
}
