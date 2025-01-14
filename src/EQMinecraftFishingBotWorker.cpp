#include "EQMinecraftFishingBotWorker.h"
#include <QTimer>
#include <QThread>
#include <Windows.h>
#include <QDebug>

bool EQMinecraftFishingBotWorker::isActive() const
{
	return mIsActive;
}

void EQMinecraftFishingBotWorker::toggleDebug()
{
	mIsDebug = !mIsDebug;
}

void EQMinecraftFishingBotWorker::setScanSize(int iScanSize)
{
	mScanSize = iScanSize;
	if (mMinecraftWindowHandle)
	{
		setScanRanges();
	}
}

void EQMinecraftFishingBotWorker::setRightClickInterval(int iRightClickInterval)
{
	mRightClickInterval = iRightClickInterval;
}

void EQMinecraftFishingBotWorker::setScanCooldown(int iScanCooldown)
{
	mScanCooldown = iScanCooldown;
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
		qDebug() << "Waiting for {mRightClickInterval}";
		QTimer::singleShot(mRightClickInterval, [this, iActivationCount]()
		{
			rightClick(iActivationCount);
			qDebug() << "Waiting for {mScanCooldown}";
			QTimer::singleShot(mScanCooldown, this, std::bind_front(&EQMinecraftFishingBotWorker::waitForFishingLine, this, iActivationCount));
		});
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
			qDebug() << "Fishing line found. Starting scans.";
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
		qDebug() << "Right click";
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
	mScanStartX = wMiddleX - mScanSize / 2;
	mScanStopX = wMiddleX + mScanSize / 2;

	int wMiddleY{ (wWindowSize.bottom - wWindowSize.top) / 2 };
	wMiddleY -= 10;
	mScanStartY = wMiddleY - mScanSize / 2;
	mScanStopY = wMiddleY + mScanSize / 2;
}

bool EQMinecraftFishingBotWorker::findBlackPixelInWindow() const
{
	HDC wHDC{ GetDC(mMinecraftWindowHandle) };
	HBITMAP wHBitmap{ CreateCompatibleBitmap(wHDC, mScanSize, mScanSize) };
	HDC wHMemDC{ CreateCompatibleDC(wHDC) };
	HBITMAP wOldBitmap{ static_cast<HBITMAP>(SelectObject(wHMemDC, wHBitmap)) };

	BitBlt(wHMemDC, 0, 0, mScanSize, mScanSize, wHDC, mScanStartX, mScanStartY, SRCCOPY);

	std::vector<BYTE> wPixelData(mScanSize * mScanSize * 4);
	BITMAPINFO wBitmapInfo{};
	wBitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	wBitmapInfo.bmiHeader.biWidth = mScanSize;
	wBitmapInfo.bmiHeader.biHeight = -mScanSize;
	wBitmapInfo.bmiHeader.biPlanes = 1;
	wBitmapInfo.bmiHeader.biBitCount = 32;
	wBitmapInfo.bmiHeader.biCompression = BI_RGB;
	GetDIBits(wHMemDC, wHBitmap, 0, mScanSize, wPixelData.data(), &wBitmapInfo, DIB_RGB_COLORS);

	bool wFoundBlackPixel{};
	for (int i{}; i < mScanSize && !wFoundBlackPixel; ++i)
	{
		for (int j{}; j < mScanSize && !wFoundBlackPixel; ++j)
		{
			const BYTE* pixel = wPixelData.data() + (i * mScanSize + j) * 4;
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
		mMinecraftWindowHandle = nullptr;
		emit deactivated();
	}
}