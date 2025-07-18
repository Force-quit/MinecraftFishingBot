#pragma once

#include <QMainWindow>
#include <EQThread.hpp>
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

private slots:
	void activated();
	void deactivated();

private:
	QGroupBox* initActivation();
	QHBoxLayout* initBotStatus();

	QGroupBox* initParameters();
	QHBoxLayout* initScanSize();
	QHBoxLayout* initRecastCooldown();
	QHBoxLayout* initScanCooldown();

	QLabel* mStatusLabel{ new QLabel(tr("Inactive")) };
	EQShortcutPicker* mShortcutListener{ new EQShortcutPicker(tr("Toggle bot :")) };

	EQMinecraftFishingBotWorker* worker{ new EQMinecraftFishingBotWorker };
	EQThread workerThread;
};