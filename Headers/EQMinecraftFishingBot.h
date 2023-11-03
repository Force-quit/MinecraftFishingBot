#pragma once

#include <QMainWindow>
#include <QString>
#include <QThread>
#include <QGroupBox>
#include "EQMinecraftFishingBotWorker.h"
#include <EQUtilities/EQShortcutListener.h>

class EQMinecraftFishingBot : public QMainWindow
{
	Q_OBJECT

public:
	EQMinecraftFishingBot(QString APP_PATH, QWidget *parent = nullptr);
	~EQMinecraftFishingBot();

private:
	QThread workerThread;
	EQMinecraftFishingBotWorker* worker;
	EQShortcutListener* shortcutListener;

	QGroupBox* initInstructions();
	QGroupBox* initActivation();
};