#pragma once

#include <QObject>
#include <QString>
#include <QScreen>
#include <QPixmap>
#include <Windows.h>
#include <EQUtilities/EQKeyboardListener.h>

class EQMinecraftFishingBotWorker  : public QObject
{
	Q_OBJECT

public slots:
	void toggle();
	void toggleDebug();

private:
	static constexpr int SCAN_RANGE{ 15 };
	void scan();
	void resetRanges();

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
