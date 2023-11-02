#include "../Headers/EQMinecraftFishingBot.h"
#include "../Headers/EQMinecraftFishingBotWorker.h"
#include <QBoxLayout>
#include <QGroupBox>
#include <QCoreApplication>
#include <QStringList>
#include <QStringView>
#include <EQUtilities/EQShortcutListener.h>
#include <QLabel>
#include <QIcon>

EQMinecraftFishingBot::EQMinecraftFishingBot(QWidget* parent)
	: QMainWindow(parent), worker(), workerThread(), APP_PATH{ QCoreApplication::applicationDirPath() }
{
	worker = new EQMinecraftFishingBotWorker();
	worker->moveToThread(&workerThread);
	workerThread.start();

	QWidget* centralWidget{ new QWidget };
	QVBoxLayout* centralLayout{ new QVBoxLayout };

	centralLayout->addWidget(initInstructions());

	QGroupBox* activationGroupBox{ new QGroupBox("Activation") };
	QVBoxLayout* activationLayout{ new QVBoxLayout };
	QLabel* activationStatus{ new QLabel("Inactive") };
	shortcutListener = new EQShortcutListener("Activation shortcut");
	connect(shortcutListener, &EQShortcutListener::shortcutPressed, [activationStatus]() {
		if (activationStatus->text() == "Inactive")
			activationStatus->setText("Active");
		else
			activationStatus->setText("Inactive");
	});
	connect(shortcutListener, &EQShortcutListener::shortcutPressed, worker, &EQMinecraftFishingBotWorker::toggle);
	activationLayout->addWidget(activationStatus);
	activationLayout->addWidget(shortcutListener);
	activationGroupBox->setLayout(activationLayout);
	centralLayout->addWidget(activationGroupBox);

	centralWidget->setLayout(centralLayout);
	setCentralWidget(centralWidget);
	setWindowIcon(QIcon(APP_PATH + "/fish.png"));
	shortcutListener->startListening();
}

QGroupBox* EQMinecraftFishingBot::initInstructions()
{
	QGroupBox* instructionsGroupBox{ new QGroupBox("Instructions") };
	QVBoxLayout* instructionsLayout{ new QVBoxLayout };

	QStringList instructions{
		"Make sure to set the pauseOnLostFocus setting to false in Minecraft",
		"Use your fishing rod where you want",
		"Put your Minecraft cursor on the floater (end of fishing line)",
		"Activate only when in Minecraft (window in foreground and has keyboard focus)",
		// "After verifying two catch, you can leave the window in" TODO
	};

	for (QString i : instructions)
		instructionsLayout->addWidget(new QLabel(i));

	instructionsGroupBox->setLayout(instructionsLayout);
	return instructionsGroupBox;
}

EQMinecraftFishingBot::~EQMinecraftFishingBot()
{
	shortcutListener->stopListening();
	workerThread.quit();
	workerThread.wait();
}