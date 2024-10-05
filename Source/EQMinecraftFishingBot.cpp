#include "../Headers/EQMinecraftFishingBot.h"
#include "../Headers/EQMinecraftFishingBotWorker.h"
#include <QBoxLayout>
#include <QGroupBox>
#include <QCoreApplication>
#include <QStringList>
#include <QStringView>
#include <QDesktopServices>
#include <EQUtilities/EQShortcutPicker.h>
#include <QLabel>
#include <QCheckbox>
#include <QIcon>
#include <QDebug>
#include <QApplication>

EQMinecraftFishingBot::EQMinecraftFishingBot()
{
	auto* wCentralWidget{ new QWidget };
	setCentralWidget(wCentralWidget);

	auto* wCentralLayout{ new QVBoxLayout };
	wCentralWidget->setLayout(wCentralLayout);

	auto wInstructions{ new QLabel };
	wInstructions->setOpenExternalLinks(true);
	wInstructions->setWordWrap(true);
	wInstructions->setText(R"(
		<h2>How to Use the Fishing Bot</h2>
		<p>- Start fishing.</p>
		<p>- Put your cursor on the bobber.</p>
		<p>- Activate/deactive the bot with the indicated shortcut.</p>
		<p>For more details, visit my video showing <a href='https://youtu.be/ir8nRKQIZ28?si=0Zxs-2CfayvATsm4'>how to use this bot</a>.</p>
		<p>Don't minimise the Minecraft window</p>
		<p>Fullscreen doesn't work</p>
		<p>You can use the "debug" mode to see the box where the scan is taking place. Make sure the bobber goes out of that box when there's a fish.
	)");
	wCentralLayout->addWidget(wInstructions);

	wCentralLayout->addWidget(initActivation());

	setWindowIcon(QIcon(":/images/fish.png"));
}

QGroupBox* EQMinecraftFishingBot::initActivation()
{
	auto* activationGroupBox{ new QGroupBox("Activation") };

	auto* activationLayout{ new QVBoxLayout };
	activationGroupBox->setLayout(activationLayout);

	auto* activationDebugCheckbox{ new QCheckBox("Activate debug mode") };
	activationLayout->addWidget(activationDebugCheckbox);

	activationLayout->addLayout(initBotStatus());

	mShortcutListener->startListening();
	activationLayout->addWidget(mShortcutListener);

	connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
	connect(mShortcutListener, &EQShortcutPicker::shortcutPressed, worker, &EQMinecraftFishingBotWorker::toggle);
	connect(worker, &EQMinecraftFishingBotWorker::activated, this, &EQMinecraftFishingBot::activated);
	connect(worker, &EQMinecraftFishingBotWorker::deactivated, this, &EQMinecraftFishingBot::deactivated);
	connect(activationDebugCheckbox, &QCheckBox::stateChanged, worker, &EQMinecraftFishingBotWorker::toggleDebug);

	worker->moveToThread(&workerThread);
	workerThread.start();

	return activationGroupBox;
}

QHBoxLayout* EQMinecraftFishingBot::initBotStatus()
{
	auto* wActivationStatusLayout{ new QHBoxLayout };

	auto* wActivationStatusLabel{ new QLabel("Bot status : ") };
	wActivationStatusLayout->addWidget(wActivationStatusLabel);

	mStatusLabel->setAutoFillBackground(true);
	wActivationStatusLayout->addWidget(mStatusLabel);
	return wActivationStatusLayout;
}

EQMinecraftFishingBot::~EQMinecraftFishingBot()
{
	workerThread.quit();
	workerThread.wait();
}

void EQMinecraftFishingBot::activated()
{
	QPalette palette(mStatusLabel->palette());
	mStatusLabel->setText("Active");
	palette.setColor(QPalette::WindowText, Qt::black);
	palette.setColor(mStatusLabel->backgroundRole(), Qt::green);
	mStatusLabel->setPalette(palette);
}

void EQMinecraftFishingBot::deactivated()
{
	QPalette palette(mStatusLabel->palette());
	mStatusLabel->setText("Inactive");
	palette.setColor(QPalette::WindowText, QApplication::palette().text().color());
	palette.setColor(mStatusLabel->backgroundRole(), Qt::transparent);
	mStatusLabel->setPalette(palette);
}