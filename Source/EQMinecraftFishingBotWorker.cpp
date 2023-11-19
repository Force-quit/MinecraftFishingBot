#include "../Headers/EQMinecraftFishingBotWorker.h"
#include <QTimer>
#include <QThread>
#include <Windows.h>

void EQMinecraftFishingBotWorker::toggleDebug()
{
	debug = !debug;
}

void EQMinecraftFishingBotWorker::scan()
{
	if (!active)
		return;

	resetRanges(); // In case of window resize

	hasBlack = false;

	for (int y{ scanStartY }; y < scanStopY && (!hasBlack || debug); ++y)
	{
		for (int x{ scanStartX }; x < scanStopX && (!hasBlack || debug); ++x)
		{
			hasBlack = GetPixel(deviceContext, x, y) == 0 || hasBlack;

			if (debug)
			{
				SetPixel(deviceContext, x, y, 0x00FFFFFF);
			}
		}
	}

	if (!hasBlack)
	{
		SendMessage(minecraftWindowHandle, WM_RBUTTONDOWN, MK_RBUTTON, MAKELPARAM(15, 15));
		QThread::msleep(100);
		SendMessage(minecraftWindowHandle, WM_RBUTTONUP, MK_RBUTTON, MAKELPARAM(15, 15));

		QTimer::singleShot(500, [=]()
			{
				SendMessage(minecraftWindowHandle, WM_RBUTTONDOWN, MK_RBUTTON, MAKELPARAM(15, 15));
				QThread::msleep(100);
				SendMessage(minecraftWindowHandle, WM_RBUTTONUP, MK_RBUTTON, MAKELPARAM(15, 15));
			});
		QTimer::singleShot(3000, this, &EQMinecraftFishingBotWorker::scan);
	}
	else
		QTimer::singleShot(100, this, &EQMinecraftFishingBotWorker::scan);
}

void EQMinecraftFishingBotWorker::resetRanges()
{
	GetWindowRect(minecraftWindowHandle, &windowSizeRectangle);
	deviceContext = GetDC(minecraftWindowHandle);

	int middleX{ (windowSizeRectangle.right - windowSizeRectangle.left) / 2 };
	middleX -= 10;
	scanStartX = middleX - SCAN_RANGE;
	scanStopX = middleX + SCAN_RANGE;

	int middleY{ (windowSizeRectangle.bottom - windowSizeRectangle.top) / 2 };
	middleY -= 10;
	scanStartY = middleY - SCAN_RANGE;
	scanStopY = middleY + SCAN_RANGE;
}

void EQMinecraftFishingBotWorker::toggle()
{
	active = !active;
	if (active)
	{
		minecraftWindowHandle = GetForegroundWindow();
		scan();
	}
	else
	{
		ReleaseDC(minecraftWindowHandle, deviceContext);
	}
}