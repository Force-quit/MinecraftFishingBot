#include "../Headers/EQMinecraftFishingBotWorker.h"
#include <QTimer>
#include <QThread>
#include <EUtilities/EUtilities.h>
#include <Windows.h>

void EQMinecraftFishingBotWorker::scan()
{
	if (!active)
		return;

	hasBlack = false;

	for (int y{ scanStartY }; y < scanStopY && !hasBlack; ++y)
		for (int x{ scanStartX }; x < scanStopX && !hasBlack; ++x)
			hasBlack = GetPixel(deviceContext, x, y) == 0;

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

void EQMinecraftFishingBotWorker::toggle()
{
	active = !active;
	if (active)
	{
		minecraftWindowHandle = GetForegroundWindow();
		GetWindowRect(minecraftWindowHandle, &windowSizeRectangle);
		deviceContext = GetDC(minecraftWindowHandle);

		int middleX{ windowSizeRectangle.right / 2 };
		scanStartX = middleX - SCAN_RANGE;
		scanStopX = middleX + SCAN_RANGE;

		int middleY{ windowSizeRectangle.bottom / 2 };
		scanStartY = middleY - SCAN_RANGE;
		scanStopY = middleY + SCAN_RANGE;
		scan();
	}
	else
	{
		ReleaseDC(minecraftWindowHandle, deviceContext);
	}
}