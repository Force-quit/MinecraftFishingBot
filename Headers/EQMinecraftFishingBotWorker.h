#pragma once

#include <QObject>
#include <Windows.h>

class EQMinecraftFishingBotWorker  : public QObject
{
	Q_OBJECT

public slots:
	void toggle();
	void toggleDebug();

private slots:
	void rightClick();

private:
	static constexpr int SCAN_RANGE{ 15 };
	void scan();
	void setScanRanges();
	

	bool active;
	bool debug;
	bool hasBlack;

	HWND minecraftWindowHandle;
	RECT windowSizeRectangle;
	HDC deviceContext;

	int scanStartX;
	int scanStopX;
	int scanStartY;
	int scanStopY;
};
