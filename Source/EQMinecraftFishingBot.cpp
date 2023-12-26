#include "../Headers/EQMinecraftFishingBot.h"
#include "../Headers/EQMinecraftFishingBotWorker.h"
#include <QBoxLayout>
#include <QGroupBox>
#include <QCoreApplication>
#include <QStringList>
#include <QStringView>
#include <QDesktopServices>
#include <EQUtilities/EQShortcutListener.h>
#include <QLabel>
#include <QCheckbox>
#include <QIcon>
#include <QApplication>

EQMinecraftFishingBot::EQMinecraftFishingBot()
	: QMainWindow(), worker(std::make_unique<EQMinecraftFishingBotWorker>()), workerThread()
{
	worker->moveToThread(&workerThread);
	workerThread.start();

	QWidget* centralWidget{ new QWidget };
	QVBoxLayout* centralLayout{ new QVBoxLayout };

	centralLayout->addWidget(initInstructions());
	centralLayout->addWidget(initActivation());

	centralWidget->setLayout(centralLayout);
	setCentralWidget(centralWidget);
	setWindowIcon(QIcon(":/images/fish.png"));
	shortcutListener->startListening();
}

QGroupBox* EQMinecraftFishingBot::initInstructions()
{
	QGroupBox* instructionsGroupBox{ new QGroupBox("Instructions") };
	QVBoxLayout* instructionsLayout{ new QVBoxLayout };

	QStringList instructions{
		"Set the pauseOnLostFocus setting to false in Minecraft",
		"Use your fishing rod",
		"Put your Minecraft cursor on the floater (end of fishing line)",
		"Make sure you are on Minecraft (window in foreground and has keyboard focus) and activate the bot",
		"--- Don't minimise the game ---",
	};

	for (QString i : instructions)
		instructionsLayout->addWidget(new QLabel(i));

	instructionsGroupBox->setLayout(instructionsLayout);
	return instructionsGroupBox;
}

QGroupBox* EQMinecraftFishingBot::initActivation()
{
	QGroupBox* activationGroupBox{ new QGroupBox("Activation") };
	QVBoxLayout* activationLayout{ new QVBoxLayout };

	QCheckBox* activationDebugCheckbox{ new QCheckBox("Activate debug mode") };
	activationLayout->addWidget(activationDebugCheckbox);


	QHBoxLayout* activationStatusLayout{ new QHBoxLayout };
	QSizePolicy p;
	p.setHorizontalStretch(1);
	QLabel* activationStatusLabel{ new QLabel("Bot status : ") };
	activationStatusLabel->setSizePolicy(p);
	QLabel* activationStatus{ new QLabel("Inactive") };
	activationStatus->setAutoFillBackground(true);
	activationStatusLayout->addWidget(activationStatusLabel);
	activationStatusLayout->addWidget(activationStatus);

	shortcutListener = new EQShortcutListener("Activation shortcut");
	connect(shortcutListener, &EQShortcutListener::shortcutPressed, [activationStatus]() {
		QPalette palette = activationStatus->palette();

		if (activationStatus->text() == "Inactive")
		{
			activationStatus->setText("Active");
			palette.setColor(QPalette::WindowText, Qt::black);
			palette.setColor(activationStatus->backgroundRole(), Qt::green);

		}
		else
		{
			activationStatus->setText("Inactive");
			palette.setColor(QPalette::WindowText, QApplication::palette().text().color());
			palette.setColor(activationStatus->backgroundRole(), Qt::transparent);
		}

		activationStatus->setPalette(palette);
		});
	connect(shortcutListener, &EQShortcutListener::shortcutPressed, worker.get(), &EQMinecraftFishingBotWorker::toggle);
	connect(activationDebugCheckbox, &QCheckBox::stateChanged, worker.get(), &EQMinecraftFishingBotWorker::toggleDebug);

	activationLayout->addLayout(activationStatusLayout);
	activationLayout->addWidget(shortcutListener);
	activationGroupBox->setLayout(activationLayout);
	return activationGroupBox;
}

EQMinecraftFishingBot::~EQMinecraftFishingBot()
{
	shortcutListener->stopListening();
	workerThread.quit();
	workerThread.wait();
}