#pragma once

#include <QObject>
#include <Windows.h>
#include <thread>

class EQMinecraftFishingBotWorker : public QObject
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
	static constexpr int FULL_SCAN_RANGE{ 2 * SCAN_RANGE };

	void setScanRanges();
	void scan(std::uint8_t iActivationCount);
	void waitForFishingLine(std::uint8_t iActivationCount);
	bool findBlackPixelInWindow() const;

	void debugThreadLoop(std::stop_token stopToken) const;
	void drawDebugRectangle() const;

	HWND mMinecraftWindowHandle{};

	std::jthread mDebugThread;
	bool mIsActive{};
	bool mIsDebug{};
	std::uint8_t mActivationCount{};

	int mScanStartX{};
	int mScanStopX{};
	int mScanStartY{};
	int mScanStopY{};
};
