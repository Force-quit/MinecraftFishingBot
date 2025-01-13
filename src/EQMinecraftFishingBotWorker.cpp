#include "EQMinecraftFishingBotWorker.h"
#include <QTimer>
#include <QThread>
#include <Windows.h>

bool EQMinecraftFishingBotWorker::isActive() const
{
	return mIsActive;
}

void EQMinecraftFishingBotWorker::toggleDebug()
{
	mIsDebug = !mIsDebug;
}

void EQMinecraftFishingBotWorker::debugThreadLoop(std::stop_token stopToken) const
{
	while (!stopToken.stop_requested())
	{
		if (mIsDebug)
		{
			drawDebugRectangle();
		}
		QThread::msleep(1);
	}
}

void EQMinecraftFishingBotWorker::drawDebugRectangle() const
{
	HDC hdc{ GetDC(mMinecraftWindowHandle) };
	HBRUSH hBrush{ CreateSolidBrush(RGB(255, 255, 255)) };
	HBRUSH hOldBrush{ static_cast<HBRUSH>(SelectObject(hdc, GetStockObject(NULL_BRUSH))) };
	RECT rect{ mScanStartX, mScanStartY, mScanStopX, mScanStopY };
	FrameRect(hdc, &rect, hBrush);
	SelectObject(hdc, hOldBrush);
	DeleteObject(hBrush);
	ReleaseDC(mMinecraftWindowHandle, hdc);
}

void EQMinecraftFishingBotWorker::scan(std::uint8_t iActivationCount)
{
	if (!mIsActive || iActivationCount != mActivationCount)
	{
		return;
	}

	if (!findBlackPixelInWindow())
	{
		rightClick(iActivationCount);
		QTimer::singleShot(1000, this, std::bind_front(&EQMinecraftFishingBotWorker::rightClick, this, iActivationCount));
		QTimer::singleShot(2500, this, std::bind_front(&EQMinecraftFishingBotWorker::waitForFishingLine, this, iActivationCount));
	}
	else
	{
		QTimer::singleShot(100, this, std::bind_front(&EQMinecraftFishingBotWorker::scan, this, iActivationCount));
	}
}

void EQMinecraftFishingBotWorker::waitForFishingLine(std::uint8_t iActivationCount)
{
	if (mIsActive && iActivationCount == mActivationCount)
	{
		if (findBlackPixelInWindow())
		{
			QTimer::singleShot(100, this, std::bind_front(&EQMinecraftFishingBotWorker::scan, this, iActivationCount));
		}
		else
		{
			QTimer::singleShot(100, this, std::bind_front(&EQMinecraftFishingBotWorker::waitForFishingLine, this, iActivationCount));
		}
	}
}

void EQMinecraftFishingBotWorker::rightClick(std::uint8_t iActivationCount)
{
	if (mIsActive && iActivationCount == mActivationCount)
	{
		SendMessage(mMinecraftWindowHandle, WM_RBUTTONDOWN, MK_RBUTTON, MAKELPARAM(15, 15));
		QThread::msleep(50);
		SendMessage(mMinecraftWindowHandle, WM_RBUTTONUP, MK_RBUTTON, MAKELPARAM(15, 15));
	}
}

void EQMinecraftFishingBotWorker::setScanRanges()
{
	RECT wWindowSize{};
	GetWindowRect(mMinecraftWindowHandle, &wWindowSize);

	int wMiddleX{ (wWindowSize.right - wWindowSize.left) / 2 };
	wMiddleX -= 10;
	mScanStartX = wMiddleX - SCAN_RANGE;
	mScanStopX = wMiddleX + SCAN_RANGE;

	int wMiddleY{ (wWindowSize.bottom - wWindowSize.top) / 2 };
	wMiddleY -= 10;
	mScanStartY = wMiddleY - SCAN_RANGE;
	mScanStopY = wMiddleY + SCAN_RANGE;
}

bool EQMinecraftFishingBotWorker::findBlackPixelInWindow() const
{
	HDC wHDC{ GetDC(mMinecraftWindowHandle) };
	HBITMAP wHBitmap{ CreateCompatibleBitmap(wHDC, FULL_SCAN_RANGE, FULL_SCAN_RANGE) };
	HDC wHMemDC{ CreateCompatibleDC(wHDC) };
	HBITMAP wOldBitmap{ static_cast<HBITMAP>(SelectObject(wHMemDC, wHBitmap)) };

	BitBlt(wHMemDC, 0, 0, FULL_SCAN_RANGE, FULL_SCAN_RANGE, wHDC, mScanStartX, mScanStartY, SRCCOPY);

	std::array<BYTE, FULL_SCAN_RANGE * FULL_SCAN_RANGE * 4> wPixelData{};
	BITMAPINFO wBitmapInfo{};
	wBitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	wBitmapInfo.bmiHeader.biWidth = FULL_SCAN_RANGE;
	wBitmapInfo.bmiHeader.biHeight = -FULL_SCAN_RANGE;
	wBitmapInfo.bmiHeader.biPlanes = 1;
	wBitmapInfo.bmiHeader.biBitCount = 32;
	wBitmapInfo.bmiHeader.biCompression = BI_RGB;
	GetDIBits(wHMemDC, wHBitmap, 0, FULL_SCAN_RANGE, wPixelData.data(), &wBitmapInfo, DIB_RGB_COLORS);

	bool wFoundBlackPixel{};
	for (int i{}; i < FULL_SCAN_RANGE && !wFoundBlackPixel; ++i)
	{
		for (int j{}; j < FULL_SCAN_RANGE && !wFoundBlackPixel; ++j)
		{
			const BYTE* pixel = wPixelData.data() + (i * FULL_SCAN_RANGE + j) * 4;
			wFoundBlackPixel = pixel[2] == 0 && pixel[1] == 0 && pixel[0] == 0;
		}
	}

	SelectObject(wHMemDC, wOldBitmap);
	DeleteObject(wHBitmap);
	DeleteDC(wHMemDC);
	ReleaseDC(mMinecraftWindowHandle, wHDC);
	return wFoundBlackPixel;
}

void EQMinecraftFishingBotWorker::toggle()
{
	mIsActive = !mIsActive;
	if (mIsActive)
	{
		mMinecraftWindowHandle = GetForegroundWindow();
		setScanRanges();
		mDebugThread = std::jthread(std::bind_front(&EQMinecraftFishingBotWorker::debugThreadLoop, this));
		++mActivationCount;
		emit activated();
		scan(mActivationCount);
	}
	else
	{
		mDebugThread.request_stop();
		emit deactivated();
	}
}