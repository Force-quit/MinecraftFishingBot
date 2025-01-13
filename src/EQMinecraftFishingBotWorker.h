#pragma once

#include <QObject>
#include <Windows.h>
#include <thread>

class EQMinecraftFishingBotWorker : public QObject
{
	Q_OBJECT

public:
	bool isActive() const;

	static constexpr int MINIMUM_SCAN_SIZE{ 25 };
	static constexpr int MAX_SCAN_SIZE{ 60 };
	static constexpr int DEFAULT_SCAN_SIZE{ MAX_SCAN_SIZE / 2 };

public slots:
	void toggle();
	void toggleDebug();
	void setScanSize(int iScanSize);

signals:
	void activated();
	void deactivated();

private slots:
	void rightClick(std::uint8_t iActivationCount);

private:
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

	int mScanSize{ DEFAULT_SCAN_SIZE };
	int mScanStartX{};
	int mScanStopX{};
	int mScanStartY{};
	int mScanStopY{};
};
