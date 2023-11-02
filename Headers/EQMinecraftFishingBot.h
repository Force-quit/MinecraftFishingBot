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
	EQMinecraftFishingBot(QWidget *parent = nullptr);
	~EQMinecraftFishingBot();

private:
	const QString APP_PATH;
	QThread workerThread;
	EQMinecraftFishingBotWorker* worker;
	EQShortcutListener* shortcutListener;

	QGroupBox* initInstructions();
};