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

void EQMinecraftFishingBotWorker::scan(std::uint8_t iActivationCount)
{
	if (!mIsActive || iActivationCount != mActivationCount)
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
		rightClick(iActivationCount);
		QTimer::singleShot(1000, this, std::bind_front(&EQMinecraftFishingBotWorker::rightClick, this, iActivationCount));
		QTimer::singleShot(3000, this, std::bind_front(&EQMinecraftFishingBotWorker::scan, this, iActivationCount));
	}
	else
	{
		QTimer::singleShot(100, this, std::bind_front(&EQMinecraftFishingBotWorker::scan, this, iActivationCount));
	}

}

void EQMinecraftFishingBotWorker::rightClick(std::uint8_t iActivationCount)
{
	if (mIsActive && iActivationCount == mActivationCount)
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
		++mActivationCount;
		emit activated();
		scan(mActivationCount);
	}
	else
	{
		debugThread.request_stop();
		emit deactivated();
	}
}