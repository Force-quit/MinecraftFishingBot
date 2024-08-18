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

private slots:
	void rightClick();

private:
	static constexpr int SCAN_RANGE{ 15 };
	void scan();
	void setScanRanges();

	void debugThreadLoop(std::stop_token stopToken) const;
	void drawDebugRectangle() const;

	std::jthread debugThread;
	bool mIsActive{};
	bool mIsDebug{};

	HWND minecraftWindowHandle{};

	int scanStartX{};
	int scanStopX{};
	int scanStartY{};
	int scanStopY{};
};
