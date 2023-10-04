#pragma once

#include "equtilities_global.h"
#include <QLineEdit>

class EQUTILITIES_EXPORT EQIntLineEdit : public QLineEdit
{
	Q_OBJECT

public:
	EQIntLineEdit(int minimum, int maximum, QWidget *parent = nullptr);
	~EQIntLineEdit();

signals:
	void valueChanged(int newValue);

private slots:
	void emitValueChanged();
};
