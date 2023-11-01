#include "../Headers/EQMinecraftFishingBot.h"
#include "../Headers/EQMinecraftFishingBotWorker.h"
#include <QBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <EQUtilities/EQIntLineEdit.h>
#include <QGuiApplication>
#include <QScreen>
#include <QString>
#include <QImage>
#include <QCheckBox>
#include <QPushButton>

EQMinecraftFishingBot::EQMinecraftFishingBot(QWidget *parent)
    : QMainWindow(parent), worker(), workerThread(), APP_PATH{ QCoreApplication::applicationDirPath() }
{
    worker = new EQMinecraftFishingBotWorker();
    worker->moveToThread(&workerThread);
    workerThread.start();

    QWidget* centralWidget{ new QWidget };
    QHBoxLayout* centralLayout{ new QHBoxLayout };
   
    QVBoxLayout* parametersAndActivationLayout{ new QVBoxLayout };
    QGroupBox* parameters{ initParameters() };
    QGroupBox* activationGroupBox{ initActivationLayout() };
    parametersAndActivationLayout->addWidget(parameters);
    parametersAndActivationLayout->addWidget(activationGroupBox);

    QVBoxLayout* helperLayout{ initHelpLayout() };

    centralLayout->addLayout(parametersAndActivationLayout);
    centralLayout->addLayout(helperLayout);

    centralWidget->setLayout(centralLayout);
    setCentralWidget(centralWidget);
    setWindowIcon(QIcon(APP_PATH + "/fish.png"));
}

QGroupBox* EQMinecraftFishingBot::initActivationLayout()
{
    QGroupBox* activationGroupBox{ new QGroupBox("Activation") };
    QVBoxLayout* activationLayout{ new QVBoxLayout };

    QHBoxLayout* activationHintLayout{ new QHBoxLayout };
    QLabel* activationHintLabel{ new QLabel("Current activation shortcut :") };
    QLabel* activationHintText{ new QLabel("CTRL") };
    activationHintLayout->addWidget(activationHintLabel);
    activationHintLayout->addWidget(activationHintText);

    QHBoxLayout* activationStatusLayout{ new QHBoxLayout };
    QLabel* activationLabel{ new QLabel("Status :") };
    QLabel* activationStatusText{ new QLabel("Innactive") };
    activationStatusLayout->addWidget(activationLabel);
    activationStatusLayout->addWidget(activationStatusText);

    activationLayout->addLayout(activationHintLayout);
    activationLayout->addLayout(activationStatusLayout);

    activationGroupBox->setLayout(activationLayout);
    
    connect(worker, &EQMinecraftFishingBotWorker::stateChanged, [activationStatusText](const bool isActive) {
        if (isActive)
            activationStatusText->setText("Active");
        else
            activationStatusText->setText("Innactive");
    });
    return activationGroupBox;
}

QGroupBox* EQMinecraftFishingBot::initParameters()
{
    QGroupBox* parameters{ new QGroupBox("Parameters") };

    QVBoxLayout* parametersLayout{ new QVBoxLayout };
    parametersLayout->setAlignment(Qt::AlignCenter);

    QHBoxLayout* targetScreenLayout{ new QHBoxLayout };
    QLabel* targetScreenLabel{ new QLabel("Target screen :") };
    QComboBox* targetScreenComboBox{ new QComboBox };
    for (QScreen* screen : QGuiApplication::screens())
        targetScreenComboBox->addItem(screen->name());
    targetScreenLayout->addWidget(targetScreenLabel);
    targetScreenLayout->addWidget(targetScreenComboBox);

    QHBoxLayout* captureSizeLayout{ new QHBoxLayout };
    QLabel* captureSizeLabel{ new QLabel("Capture size :") };
    unsigned int min{ EQMinecraftFishingBotWorker::MIN_CAPTURE_SIZE };
    unsigned int max{ EQMinecraftFishingBotWorker::MAX_CAPTURE_SIZE };
    unsigned int defaultValue{ EQMinecraftFishingBotWorker::DEFAULT_CAPTURE_SIZE };
    EQIntLineEdit* captureSizeLineEdit{ new EQIntLineEdit(min, max) };
    captureSizeLayout->addWidget(captureSizeLabel);
    captureSizeLayout->addWidget(captureSizeLineEdit);

    QHBoxLayout* captureIntervalLayout{ new QHBoxLayout };
    QLabel* captureIntervalLabel{ new QLabel("Capture interval :") };
    min = EQMinecraftFishingBotWorker::MIN_INTERVAL;
    max = EQMinecraftFishingBotWorker::MAX_INTERVAL;
    defaultValue = EQMinecraftFishingBotWorker::DEFAULT_INTERVAL;
    EQIntLineEdit* captureIntervalLineEdit{ new EQIntLineEdit(min, max) };
    captureIntervalLayout->addWidget(captureIntervalLabel);
    captureIntervalLayout->addWidget(captureIntervalLineEdit);

    parametersLayout->addLayout(targetScreenLayout);
    parametersLayout->addLayout(captureSizeLayout);
    parametersLayout->addLayout(captureIntervalLayout);

    connect(targetScreenComboBox, &QComboBox::currentTextChanged, worker, &EQMinecraftFishingBotWorker::targetScreenChanged);
    connect(captureSizeLineEdit, &EQIntLineEdit::valueChanged, worker, &EQMinecraftFishingBotWorker::captureSizeChanged);
    connect(captureIntervalLineEdit, &EQIntLineEdit::valueChanged, worker, &EQMinecraftFishingBotWorker::setCaptureInterval);
   
    parameters->setLayout(parametersLayout);
    return parameters;
}

QVBoxLayout* EQMinecraftFishingBot::initHelpLayout()
{
    QVBoxLayout* helpLayout{ new QVBoxLayout };
    helpLayout->setAlignment(Qt::AlignCenter);

    QLabel* helpLabel{ new QLabel("Position your camera so you have\n the fishing line in your capture, like this")};
    helpLabel->setAlignment(Qt::AlignHCenter);
    QLabel* helpExample{ new QLabel };
    helpExample->setPixmap(QPixmap::fromImage(QImage("captureExample.png")));
    helpExample->setAlignment(Qt::AlignHCenter);
   
    QLabel* helpLabel2{ new QLabel("Your capture") };
    helpLabel2->setAlignment(Qt::AlignHCenter);
    userCapture = new QLabel;
    userCapture->setAlignment(Qt::AlignHCenter);
    
    QHBoxLayout* displayYourCaptureLayout{ new QHBoxLayout };
    displayYourCaptureLayout->setAlignment(Qt::AlignCenter);
    QCheckBox* displayYourCapture{ new QCheckBox("Show your capture") };
    displayYourCaptureLayout->addWidget(displayYourCapture);

    helpLayout->addWidget(helpLabel);
    helpLayout->addWidget(helpExample);
    helpLayout->addWidget(helpLabel2);
    helpLayout->addWidget(userCapture);
    helpLayout->addLayout(displayYourCaptureLayout);

    connect(displayYourCapture, &QCheckBox::stateChanged, worker, &EQMinecraftFishingBotWorker::requestHelp);
    connect(worker, &EQMinecraftFishingBotWorker::captureTaken, this, &EQMinecraftFishingBot::displayCapture);
    return helpLayout;
}

void EQMinecraftFishingBot::displayCapture(const QPixmap capture)
{
    userCapture->setPixmap(capture);
}

EQMinecraftFishingBot::~EQMinecraftFishingBot() 
{
    worker->stop();
    workerThread.quit();
    workerThread.wait();
}