#include "../Headers/EQMinecraftFishingBotWorker.h"
#include <QTimer>
#include <QThread>
#include <Windows.h>

import eutilities;

bool EQMinecraftFishingBotWorker::isActive() const
{
	return mIsActive;
}

void EQMinecraftFishingBotWorker::toggleDebug()
{
	mIsDebug = !mIsDebug;
}

void EQMinecraftFishingBotWorker::scan()
{
	if (!mIsActive)
	{
		return;
	}

	HDC hdc = GetDC(minecraftWindowHandle);
	bool hasBlack{};
	for (int y{ scanStartY }; y < scanStopY && !hasBlack; ++y)
	{
		for (int x{ scanStartX }; x < scanStopX && !hasBlack; ++x)
		{
			hasBlack = GetPixel(hdc, x, y) == 0;
		}
	}
	ReleaseDC(minecraftWindowHandle, hdc);


	if (!hasBlack)
	{
		rightClick();
		QTimer::singleShot(1000, this, &EQMinecraftFishingBotWorker::rightClick);
		QTimer::singleShot(3000, this, &EQMinecraftFishingBotWorker::scan);
	}
	else
	{
		QTimer::singleShot(100, this, &EQMinecraftFishingBotWorker::scan);
	}

}

void EQMinecraftFishingBotWorker::rightClick()
{
	if (mIsActive)
	{
		SendMessage(minecraftWindowHandle, WM_RBUTTONDOWN, MK_RBUTTON, MAKELPARAM(15, 15));
		QThread::msleep(50);
		SendMessage(minecraftWindowHandle, WM_RBUTTONUP, MK_RBUTTON, MAKELPARAM(15, 15));
	}
}

void EQMinecraftFishingBotWorker::setScanRanges()
{
	RECT wWindowSize{};
	GetWindowRect(minecraftWindowHandle, &wWindowSize);

	int middleX{ (wWindowSize.right - wWindowSize.left) / 2 };
	middleX -= 10;
	scanStartX = middleX - SCAN_RANGE;
	scanStopX = middleX + SCAN_RANGE;

	int middleY{ (wWindowSize.bottom - wWindowSize.top) / 2 };
	middleY -= 10;
	scanStartY = middleY - SCAN_RANGE;
	scanStopY = middleY + SCAN_RANGE;
}

void EQMinecraftFishingBotWorker::debugThreadLoop(std::stop_token stopToken) const
{
	while (!stopToken.stop_requested())
	{
		if (mIsDebug)
		{
			drawDebugRectangle();
		}
		eutilities::sleepFor(1);
	}
}

void EQMinecraftFishingBotWorker::drawDebugRectangle() const
{
	HDC hdc = GetDC(minecraftWindowHandle);
	HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
	auto hOldBrush = static_cast<HBRUSH>(SelectObject(hdc, GetStockObject(NULL_BRUSH)));
	RECT rect = { scanStartX, scanStartY, scanStopX, scanStopY };
	FrameRect(hdc, &rect, hBrush);
	SelectObject(hdc, hOldBrush);
	DeleteObject(hBrush);
	ReleaseDC(minecraftWindowHandle, hdc);
}

void EQMinecraftFishingBotWorker::toggle()
{
	mIsActive = !mIsActive;
	if (mIsActive)
	{
		minecraftWindowHandle = GetForegroundWindow();
		setScanRanges();
		debugThread = std::jthread(std::bind_front(&EQMinecraftFishingBotWorker::debugThreadLoop, this));
		scan();
	}
	else
	{
		debugThread.request_stop();
	}
}