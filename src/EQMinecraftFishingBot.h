#pragma once

#include <QMainWindow>
#include <QThread>
#include <QGroupBox>
#include "EQMinecraftFishingBotWorker.h"
#include <EQShortcutPicker.hpp>
#include <QLabel>
#include <QHBoxLayout>

class EQMinecraftFishingBot : public QMainWindow
{
	Q_OBJECT

public:
	EQMinecraftFishingBot();
	~EQMinecraftFishingBot();

private slots:
	void activated();
	void deactivated();

private:
	QGroupBox* initActivation();
	QHBoxLayout* initBotStatus();

	QLabel* mStatusLabel{ new QLabel("Inactive") };
	EQShortcutPicker* mShortcutListener{ new EQShortcutPicker("Toggle bot :") };

	EQMinecraftFishingBotWorker* worker{ new EQMinecraftFishingBotWorker };
	QThread workerThread;
};