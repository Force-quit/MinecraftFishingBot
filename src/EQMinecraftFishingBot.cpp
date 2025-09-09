#include "EQMinecraftFishingBot.h"
#include "EQMinecraftFishingBotWorker.h"
#include <QBoxLayout>
#include <QGroupBox>
#include <QCoreApplication>
#include <QStringList>
#include <QStringView>
#include <QDesktopServices>
#include <EQShortcutPicker.hpp>
#include <QLabel>
#include <QCheckbox>
#include <QIcon>
#include <QDebug>
#include <QSlider>
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
	wInstructions->setText(tr(R"(
		<p>For detailed instructions, visit <a href='https://www.youtube.com/@EmileLaforce/videos'>my YouTube channel</a> and find the latest Minecraft fishing bot version.</p>
	)"));
	wCentralLayout->addWidget(wInstructions);

	wCentralLayout->addWidget(initActivation());
	wCentralLayout->addWidget(initParameters());

	setWindowIcon(QIcon(":/images/icon.png"));
}

QGroupBox* EQMinecraftFishingBot::initActivation()
{
	auto* activationGroupBox{ new QGroupBox(tr("Activation")) };

	auto* activationLayout{ new QVBoxLayout };
	activationGroupBox->setLayout(activationLayout);

	auto* activationDebugCheckbox{ new QCheckBox(tr("Debug mode")) };
	activationLayout->addWidget(activationDebugCheckbox);

	activationLayout->addLayout(initBotStatus());

	mShortcutListener->startListening();
	activationLayout->addWidget(mShortcutListener);

	connect(mShortcutListener, &EQShortcutPicker::shortcutPressed, worker, &EQMinecraftFishingBotWorker::toggle);
	connect(worker, &EQMinecraftFishingBotWorker::activated, this, &EQMinecraftFishingBot::activated);
	connect(worker, &EQMinecraftFishingBotWorker::deactivated, this, &EQMinecraftFishingBot::deactivated);
	connect(activationDebugCheckbox, &QCheckBox::stateChanged, worker, &EQMinecraftFishingBotWorker::toggleDebug);
	
	activationDebugCheckbox->setChecked(true);

	workerThread.moveObjectToThread(worker);
	workerThread.start();

	return activationGroupBox;
}

QGroupBox* EQMinecraftFishingBot::initParameters()
{
	auto* groupBox{ new QGroupBox(tr("Parameters")) };

	auto* layout{ new QVBoxLayout };
	groupBox->setLayout(layout);

	layout->addLayout(initScanSize());
	layout->addLayout(initRecastCooldown());
	layout->addLayout(initScanCooldown());

	return groupBox;
}

QHBoxLayout* EQMinecraftFishingBot::initScanSize()
{
	auto* layout{ new QHBoxLayout };

	auto* label{ new QLabel(tr("Scan size : ")) };
	layout->addWidget(label);

	auto* slider{ new QSlider(Qt::Horizontal) };
	layout->addWidget(slider);

	auto* valueLabel{ new QLabel() };
	layout->addWidget(valueLabel);

	connect(slider, &QSlider::valueChanged, worker, &EQMinecraftFishingBotWorker::setScanSize);
	connect(slider, &QSlider::valueChanged, [valueLabel](int iValue)
	{
		valueLabel->setText(QString::number(iValue));
	});

	connect(worker, &EQMinecraftFishingBotWorker::setScanBounds, [slider, valueLabel](int iMinScanSize, int iScanSize, int iMaxScanSize)
	{
		slider->setMinimum(iMinScanSize);
		slider->setValue(iScanSize);
		slider->setMaximum(iMaxScanSize);

		valueLabel->setText(QString::number(iScanSize));
	});

	return layout;
}

QHBoxLayout* EQMinecraftFishingBot::initRecastCooldown()
{
	auto* layout{ new QHBoxLayout };

	auto* label{ new QLabel(tr("Re-cast cooldown : ")) };
	layout->addWidget(label);

	auto* slider{ new QSlider(Qt::Horizontal) };
	slider->setMinimum(EQMinecraftFishingBotWorker::MINIMUM_RECAST_COOLDOWN);
	slider->setMaximum(EQMinecraftFishingBotWorker::MAX_RECAST_COOLDOWN);
	slider->setValue(EQMinecraftFishingBotWorker::DEFAULT_RECAST_COOLDOWN);
	layout->addWidget(slider);

	static const QString labelFormat{ QStringLiteral(u"%1ms") };
	auto* valueLabel{ new QLabel(labelFormat.arg(EQMinecraftFishingBotWorker::DEFAULT_RECAST_COOLDOWN)) };
	layout->addWidget(valueLabel);

	connect(slider, &QSlider::valueChanged, worker, &EQMinecraftFishingBotWorker::setRecastCooldown);
	connect(slider, &QSlider::valueChanged, [valueLabel](int iValue)
	{
		valueLabel->setText(labelFormat.arg(iValue));
	});

	return layout;
}

QHBoxLayout* EQMinecraftFishingBot::initScanCooldown()
{
	auto* layout{ new QHBoxLayout };

	auto* label{ new QLabel(tr("Scan cooldown : ")) };
	layout->addWidget(label);

	auto* slider{ new QSlider(Qt::Horizontal) };
	slider->setMinimum(EQMinecraftFishingBotWorker::MINIMUM_SCAN_COOLDOWN);
	slider->setMaximum(EQMinecraftFishingBotWorker::MAXIMUM_SCAN_COOLDOWN);
	slider->setValue(EQMinecraftFishingBotWorker::DEFAULT_SCAN_COOLDOWN);
	layout->addWidget(slider);

	static const QString labelFormat{ QStringLiteral(u"%1ms") };
	auto* valueLabel{ new QLabel(labelFormat.arg(EQMinecraftFishingBotWorker::DEFAULT_SCAN_COOLDOWN)) };
	layout->addWidget(valueLabel);

	connect(slider, &QSlider::valueChanged, worker, &EQMinecraftFishingBotWorker::setScanCooldown);
	connect(slider, &QSlider::valueChanged, [valueLabel](int iValue)
	{
		valueLabel->setText(labelFormat.arg(iValue));
	});

	return layout;
}

QHBoxLayout* EQMinecraftFishingBot::initBotStatus()
{
	auto* wActivationStatusLayout{ new QHBoxLayout };

	auto* wActivationStatusLabel{ new QLabel(tr("Bot status : ")) };
	wActivationStatusLayout->addWidget(wActivationStatusLabel);

	mStatusLabel->setAutoFillBackground(true);
	wActivationStatusLayout->addWidget(mStatusLabel);
	return wActivationStatusLayout;
}

void EQMinecraftFishingBot::activated()
{
	QPalette palette(mStatusLabel->palette());
	mStatusLabel->setText(tr("Active"));
	palette.setColor(QPalette::WindowText, Qt::black);
	palette.setColor(mStatusLabel->backgroundRole(), Qt::green);
	mStatusLabel->setPalette(palette);
}

void EQMinecraftFishingBot::deactivated()
{
	QPalette palette(mStatusLabel->palette());
	mStatusLabel->setText(tr("Inactive"));
	palette.setColor(QPalette::WindowText, QApplication::palette().text().color());
	palette.setColor(mStatusLabel->backgroundRole(), Qt::transparent);
	mStatusLabel->setPalette(palette);
}