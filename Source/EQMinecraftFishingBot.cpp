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
#include <QApplication>

EQMinecraftFishingBot::EQMinecraftFishingBot()
{
	auto* wCentralWidget{ new QWidget };
	setCentralWidget(wCentralWidget);

	auto* wCentralLayout{ new QVBoxLayout };
	wCentralWidget->setLayout(wCentralLayout);

	auto wInstructions{ new QLabel };
	wCentralLayout->addWidget(wInstructions);
	wInstructions->setOpenExternalLinks(true);
	wInstructions->setWordWrap(true);
	wInstructions->setText(R"(
		<h2>How to Use the Fishing Bot</h2>
		<p>1. Start fishing.</p>
		<p>2. Put your cursor on the floater (end of the fishing line).</p>
		<p>3. Activate/deactive the bot with the indicated shortcut.</p>
		<p>4. Enjoy!</p>
		<p>For more details, visit my video showing <a href='https://youtu.be/ir8nRKQIZ28?si=0Zxs-2CfayvATsm4'>how to use this bot</a>.</p>
		<p>Don't minimise the Minecraft window and you should be good to go!</p>
	)");

	wCentralLayout->addWidget(initActivation());

	setWindowIcon(QIcon(":/images/fish.png"));
}

QGroupBox* EQMinecraftFishingBot::initActivation()
{
	auto* activationGroupBox{ new QGroupBox("Activation") };
	auto* activationLayout{ new QVBoxLayout };

	auto* activationDebugCheckbox{ new QCheckBox("Activate debug mode") };
	activationLayout->addWidget(activationDebugCheckbox);

	auto* activationStatusLayout{ new QHBoxLayout };
	QSizePolicy p;
	p.setHorizontalStretch(1);
	auto* activationStatusLabel{ new QLabel("Bot status : ") };
	activationStatusLabel->setSizePolicy(p);

	mStatusLabel->setAutoFillBackground(true);
	activationStatusLayout->addWidget(activationStatusLabel);
	activationStatusLayout->addWidget(mStatusLabel);

	connect(shortcutListener, &EQShortcutPicker::shortcutPressed, worker, &EQMinecraftFishingBotWorker::toggle);
	connect(shortcutListener, &EQShortcutPicker::shortcutPressed, this, &EQMinecraftFishingBot::toggle);

	connect(activationDebugCheckbox, &QCheckBox::stateChanged, worker, &EQMinecraftFishingBotWorker::toggleDebug);

	connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
	worker->moveToThread(&workerThread);
	workerThread.start();

	shortcutListener->startListening();

	activationLayout->addLayout(activationStatusLayout);
	activationLayout->addWidget(shortcutListener);
	activationGroupBox->setLayout(activationLayout);
	return activationGroupBox;
}

void EQMinecraftFishingBot::toggle()
{
	QPalette palette = mStatusLabel->palette();

	if (worker->isActive())
	{
		mStatusLabel->setText("Active");
		palette.setColor(QPalette::WindowText, Qt::black);
		palette.setColor(mStatusLabel->backgroundRole(), Qt::green);
	}
	else
	{
		mStatusLabel->setText("Inactive");
		palette.setColor(QPalette::WindowText, QApplication::palette().text().color());
		palette.setColor(mStatusLabel->backgroundRole(), Qt::transparent);
	}

	mStatusLabel->setPalette(palette);
}

EQMinecraftFishingBot::~EQMinecraftFishingBot()
{
	shortcutListener->stopListening();
	workerThread.quit();
	workerThread.wait();
}