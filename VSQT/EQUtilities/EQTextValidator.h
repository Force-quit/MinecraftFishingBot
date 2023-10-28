#pragma once
#include "equtilities_global.h"
#include <QRegularExpressionValidator>

class EQUTILITIES_EXPORT EQTextValidator : public QRegularExpressionValidator
{
	Q_OBJECT

public:
	EQTextValidator(QObject* parent = nullptr);
	
	
	void fixup(QString& input) const override;
	QValidator::State validate(QString& input, int& pos) const override;
};

