#pragma once

#include <QObject>
#include <Windows.h>
#include <thread>

class EQMinecraftFishingBotWorker  : public QObject
{
	Q_OBJECT

public:
	bool isActive() const;

public slots:
	void toggle();
	void toggleDebug();

signals:
	void activated();
	void deactivated();

private slots:
	void rightClick(std::uint8_t iActivationCount);

private:
	static constexpr int SCAN_RANGE{ 15 };
	void scan(std::uint8_t iActivationCount);
	void setScanRanges();

	void debugThreadLoop(std::stop_token stopToken) const;
	void drawDebugRectangle() const;

	std::jthread debugThread;
	bool mIsActive{};
	bool mIsDebug{};
	std::uint8_t mActivationCount{};

	HWND minecraftWindowHandle{};

	int scanStartX{};
	int scanStopX{};
	int scanStartY{};
	int scanStopY{};
};
