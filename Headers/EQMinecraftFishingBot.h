#pragma once

#include <QMainWindow>
#include <QThread>
#include <QGroupBox>
#include "EQMinecraftFishingBotWorker.h"
#include <EQUtilities/EQShortcutPicker.h>
#include <QLabel>

class EQMinecraftFishingBot : public QMainWindow
{
	Q_OBJECT

public:
	EQMinecraftFishingBot();
	~EQMinecraftFishingBot();

private slots:
	void toggle();

private:
	QGroupBox* initActivation();

	QLabel* mStatusLabel{ new QLabel("Inactive") };
	EQShortcutPicker* shortcutListener{ new EQShortcutPicker("Toggle bot :") };

	EQMinecraftFishingBotWorker* worker{ new EQMinecraftFishingBotWorker };
	QThread workerThread;
};