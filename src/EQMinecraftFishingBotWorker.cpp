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

void EQMinecraftFishingBotWorker::setRecastCooldown(int iRecastCooldown)
{
	mRecastCooldown = iRecastCooldown;
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
		qDebug() << "Re-casting in {mRightClickInterval}";
		QTimer::singleShot(mRecastCooldown, [this, iActivationCount]()
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

void EQMinecraftFishingBotWorker::setScanRanges(bool isInit/*=false*/)
{
	RECT wWindowSize{};
	GetWindowRect(mMinecraftWindowHandle, &wWindowSize);

	const int width = wWindowSize.right - wWindowSize.left;
	const int height = wWindowSize.bottom - wWindowSize.top;

	if (isInit)
	{
		const int windowSize = std::sqrt(std::pow(width, 2) + std::pow(height, 2));
		const int maxScanSize = windowSize * MAX_SCAN_PROPORTION;
		mScanSize = maxScanSize * DEFAULT_SCAN_PROPORTION;
		const int minScanSize = maxScanSize * MIN_SCAN_PROPORTION;
		emit setScanBounds(minScanSize, mScanSize, maxScanSize);
	}

	int middleX{ width / 2 - 10};
	mScanStartX = middleX - mScanSize / 2;
	mScanStopX = middleX + mScanSize / 2;

	int middleY{ height / 2 };
	mScanStartY = middleY - mScanSize / 2;
	mScanStopY = middleY + mScanSize / 2;
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

#ifdef QT_DEBUG
	int closestRValue{ 255 };
	int closestGValue{ 255 };
	int closestBValue{ 255 };
#endif

	for (int i{}; i < mScanSize && !wFoundBlackPixel; ++i)
	{
		for (int j{}; j < mScanSize && !wFoundBlackPixel; ++j)
		{
			const BYTE* pixel = wPixelData.data() + (i * mScanSize + j) * 4;
			wFoundBlackPixel = pixel[0] <= BLACK_PIXEL_TOLERANCE &&
				pixel[1] <= BLACK_PIXEL_TOLERANCE &&
				pixel[2] <= BLACK_PIXEL_TOLERANCE;

#ifdef QT_DEBUG
			// Fuck you authors of Windows.h. I hate your min and max macros.
			closestRValue = (std::min)(closestRValue, static_cast<int>(pixel[0])); 
			closestGValue = (std::min)(closestGValue, static_cast<int>(pixel[1]));
			closestBValue = (std::min)(closestBValue, static_cast<int>(pixel[2]));
#endif
		}
	}

#ifdef QT_DEBUG
	if (!wFoundBlackPixel)
	{
		qDebug() << "Closest black found : (" << closestRValue << closestGValue << closestBValue << ")";
	}
#endif

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
		SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);
		mMinecraftWindowHandle = GetForegroundWindow();
		setScanRanges(true);
		mDebugThread = std::jthread(std::bind_front(&EQMinecraftFishingBotWorker::debugThreadLoop, this));
		++mActivationCount;
		emit activated();
		scan(mActivationCount);
	}
	else
	{
		SetThreadExecutionState(ES_CONTINUOUS);
		mDebugThread.request_stop();
		mMinecraftWindowHandle = nullptr;
		emit deactivated();
	}
}