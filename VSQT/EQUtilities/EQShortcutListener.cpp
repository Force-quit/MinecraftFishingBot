#include "EQShortcutListener.h"
#include <QString>
#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include "EQShortcutListenerWorker.h"
#include <QPushButton>
#include <QVector>

EQShortcutListener::EQShortcutListener(QString labelText, QWidget *parent)
	: QWidget(parent), workerThread(), changeShortcutButton(), keyboardListener(), isListening()
{
	QHBoxLayout* centralLayout{ new QHBoxLayout };
	
	QLabel* label{ new QLabel(labelText) };
	QLabel* shortcutText{ new QLabel(EQShortcutListenerWorker::DEFAULT_SHORTCUT) };
	changeShortcutButton = new QPushButton("Change");

	centralLayout->addWidget(label);
	centralLayout->addWidget(shortcutText);
	centralLayout->addWidget(changeShortcutButton);

	setLayout(centralLayout);

	worker = new EQShortcutListenerWorker;
	keyboardListener = new EQKeyboardListener(EQShortcutListenerWorker::DEFAULT_CODE);
	worker->moveToThread(&workerThread);
	keyboardListener->moveToThread(&workerThread);
	connect(changeShortcutButton, &QPushButton::clicked, this, &EQShortcutListener::changingShortcut);
	connect(changeShortcutButton, &QPushButton::clicked, keyboardListener, &EQKeyboardListener::stopListening);
	connect(changeShortcutButton, &QPushButton::clicked, worker, &EQShortcutListenerWorker::startListening);

	connect(worker, &EQShortcutListenerWorker::shortcutTextChanged, shortcutText, &QLabel::setText);
	connect(worker, &EQShortcutListenerWorker::shortcutSelected, this, &EQShortcutListener::shortcutChosen);
	connect(worker, &EQShortcutListenerWorker::shortcutReady, this, &EQShortcutListener::shortcutReady);
	
	connect(keyboardListener, SIGNAL(targetKeysPressed()), this, SIGNAL(shortcutPressed()));
	connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
	connect(&workerThread, &QThread::finished, keyboardListener, &QObject::deleteLater);
	workerThread.start();
}

void EQShortcutListener::changingShortcut()
{
	changeShortcutButton->setEnabled(false);
	emit startedChangingShortcut();
}

void EQShortcutListener::disableButton()
{
	changeShortcutButton->setDisabled(true);
}

void EQShortcutListener::enableButton()
{
	changeShortcutButton->setEnabled(true);
}

void EQShortcutListener::shortcutChosen()
{
	changeShortcutButton->setEnabled(true);
	emit stoppedChangingShortcut();
}

void EQShortcutListener::shortcutReady(QVector<int> virtualKeys)
{
	keyboardListener->setTargetKeys(virtualKeys);
	if (isListening)
		keyboardListener->startListening();
}

void EQShortcutListener::startListening()
{
	keyboardListener->startListening();
	isListening = true;
}

void EQShortcutListener::stopListening()
{
	keyboardListener->stopListening();
	isListening = false;
}

EQShortcutListener::~EQShortcutListener()
{
	workerThread.quit();
	workerThread.wait();
}

QVector<int> EQShortcutListener::getTargetKeys() const
{
	return keyboardListener->getTargetKeys();
}

void EQShortcutListener::setTargetKeys(QVector<int>& targetKeys)
{
	keyboardListener->setTargetKeys(targetKeys);
	// TODO change text
}
