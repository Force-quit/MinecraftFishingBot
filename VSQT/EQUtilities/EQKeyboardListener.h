#pragma once

#include "equtilities_global.h"
#include <QWidget>
#include <unordered_map>
#include <QString>
#include <QVector>

class EQUTILITIES_EXPORT EQKeyboardListener : public QWidget
{
	Q_OBJECT

public:
	EQKeyboardListener(QVector<int> targetKeys, unsigned short checkInterval = 50);
	~EQKeyboardListener();

	QVector<int> getTargetKeys() const;
public slots:
	void setTargetKeys(QVector<int> virtualKeys);
	void stopListening();
	void startListening();

signals:
	void targetKeysPressed();

private slots:
	void check();

private:
	QVector<int> targetKeys;
	const unsigned short checkInterval;
	unsigned short pressedKeys;
	bool canTrigger;

	bool listening;
};