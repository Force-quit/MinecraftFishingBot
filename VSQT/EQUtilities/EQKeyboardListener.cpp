#include "EQKeyboardListener.h"
#include <QString>
#include <QTimer>
#include <Windows.h>
#include <QVector>

EQKeyboardListener::EQKeyboardListener(QVector<int> targetKeys, unsigned short checkInterval)
	: targetKeys(targetKeys), checkInterval(checkInterval), canTrigger(true), pressedKeys(), listening()
{

}

void EQKeyboardListener::setTargetKeys(QVector<int> virtualKeys)
{
	targetKeys = virtualKeys;
}

void EQKeyboardListener::check()
{
	pressedKeys = 0;

	for (int& i : targetKeys)
	{
		if (GetAsyncKeyState(i))
			++pressedKeys;
		else
			canTrigger = true;
	}

	if (pressedKeys == targetKeys.size() && canTrigger)
	{
		emit targetKeysPressed();
		canTrigger = false;
	}
	
	if (listening)
		QTimer::singleShot(checkInterval, this, &EQKeyboardListener::check);
}

void EQKeyboardListener::stopListening()
{
	listening = false;
}

void EQKeyboardListener::startListening()
{
	listening = true;
	QTimer::singleShot(checkInterval, this, &EQKeyboardListener::check);
}

EQKeyboardListener::~EQKeyboardListener() {}

QVector<int> EQKeyboardListener::getTargetKeys() const
{
	return targetKeys;
}
