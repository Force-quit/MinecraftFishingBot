#pragma once

#include <QMainWindow>
#include <QThread>
#include <QGroupBox>
#include "EQMinecraftFishingBotWorker.h"
#include <EQUtilities/EQShortcutListener.h>
#include <memory>

class EQMinecraftFishingBot : public QMainWindow
{
	Q_OBJECT

public:
	EQMinecraftFishingBot();
	~EQMinecraftFishingBot();

private:
	QThread workerThread;
	std::unique_ptr<EQMinecraftFishingBotWorker> worker;

	// These are deleted whith Qt's parent system
	EQShortcutListener* shortcutListener; 
	QGroupBox* initInstructions();
	QGroupBox* initActivation();
};