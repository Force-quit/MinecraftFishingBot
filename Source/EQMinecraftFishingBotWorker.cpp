#include "../Headers/EQMinecraftFishingBotWorker.h"
#include <QGuiApplication>
#include <QPixmap>
#include <QScreen>
#include <QSize>
#include <QTimer>
#include <EUtilities/EUtilities.h>
#include <Windows.h>

EQMinecraftFishingBotWorker::EQMinecraftFishingBotWorker()
	: captureSize(DEFAULT_CAPTURE_SIZE), heightStartPixel(), widthStartPixel(), captureInterval(DEFAULT_INTERVAL),
	active(), captureHelp(), userActivation(), targetScreen(), shortcutListener()
{
	targetScreenChanged(QGuiApplication::primaryScreen()->name());
	shortcutListener = new EQKeyboardListener({ VK_RCONTROL }, 10);
	connect(shortcutListener, &EQKeyboardListener::targetKeysPressed, this, &EQMinecraftFishingBotWorker::activate);
	shortcutListener->startListening();
}


void EQMinecraftFishingBotWorker::captureSizeChanged(const unsigned short captureSize)
{
	QSize targetScreenSize{ targetScreen->size() };
	widthStartPixel = targetScreenSize.width() / 2 - captureSize / 2 - 2;
	heightStartPixel = targetScreenSize.height() / 2 - captureSize / 2 - 5;
	this->captureSize = captureSize;
}

void EQMinecraftFishingBotWorker::targetScreenChanged(const QString& screenName)
{
	for (QScreen* screen : QGuiApplication::screens())
	{
		if (screenName == screen->name())
		{
			targetScreen = screen;
			captureSizeChanged(captureSize);
			break;
		}
	}
}

void EQMinecraftFishingBotWorker::captureScreen()
{
	QPixmap capture = targetScreen->grabWindow(0, widthStartPixel, heightStartPixel, captureSize, captureSize);
	if (captureHelp)
		emit captureTaken(capture);

	if (userActivation)
	{
		QImage captureImage = capture.toImage();
		bool hasBlack{};
		for (short i = 0; i < captureSize && !hasBlack; ++i)
		{
			for (short j = 0; j < captureSize && !hasBlack; ++j)
			{
				QColor pixelColor = captureImage.pixel(i, j);
				hasBlack = pixelColor.black() == 255;
			}
		}

		if (!hasBlack)
		{
			EUtilities::rightClick();
			QTimer::singleShot(500, []() {EUtilities::rightClick(); });
			QTimer::singleShot(2000, this, &EQMinecraftFishingBotWorker::captureScreen);
			return;
		}
	}

	if (userActivation || captureHelp)
		QTimer::singleShot(captureInterval, this, &EQMinecraftFishingBotWorker::captureScreen);
	else
		active = false;
}

void EQMinecraftFishingBotWorker::requestHelp()
{
	captureHelp = !captureHelp;
	if (captureHelp && !active)
	{
		active = true;
		QTimer::singleShot(captureInterval, this, &EQMinecraftFishingBotWorker::captureScreen);
	}
}

void EQMinecraftFishingBotWorker::activate()
{
	userActivation = !userActivation;
	emit stateChanged(userActivation);
	if (userActivation && !active)
	{
		active = true;
		QTimer::singleShot(captureInterval, this, &EQMinecraftFishingBotWorker::captureScreen);
	}
}

void EQMinecraftFishingBotWorker::setCaptureInterval(const unsigned short interval)
{
	this->captureInterval = interval;
}
void EQMinecraftFishingBotWorker::stop()
{
	active = false;
	captureHelp = false;
}

EQMinecraftFishingBotWorker::~EQMinecraftFishingBotWorker()
{
	delete shortcutListener;
}