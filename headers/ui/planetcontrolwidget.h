#pragma once
#include <QWidget>
#include <memory>
#include <QPushButton>
#include <QSlider>
#include <QLabel>

// Pure view: owns the panel's widgets and lays them out, but has no knowledge of
// SolarSystem/SolarSystemModel. User actions are exposed as signals and display changes are
// driven exclusively through the setters below, both handled by PlanetControlController.
class PlanetControlWidget : public QWidget {
    Q_OBJECT
public:
    explicit PlanetControlWidget(QWidget* parent = nullptr);

    // Owned by this widget but meant to be placed by the caller (e.g. in a top bar).
    QLabel* getTimeLabel() const { return timeLabel.get(); }

    void setStartStopRunning(bool running);
    void setSatellitesVisibleLabel(bool visible);
    void setSpeedLabelText(const QString& text);
    void setTimeText(const QString& text);

signals:
    void startStopClicked();
    void quitClicked();
    void toggleSatellitesClicked();
    void speedSliderChanged(int value);

private:
    std::unique_ptr<QLabel> timeLabel;
    std::unique_ptr<QPushButton> startStopButton;
    std::unique_ptr<QPushButton> quitButton;
    std::unique_ptr<QPushButton> toggleSatellitesButton;
    std::unique_ptr<QSlider> speedSlider;
    std::unique_ptr<QLabel> speedLabel;
};

