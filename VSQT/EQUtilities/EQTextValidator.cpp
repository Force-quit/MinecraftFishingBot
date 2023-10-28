#include "EQTextValidator.h"

EQTextValidator::EQTextValidator(QObject* parent)
	: QRegularExpressionValidator(parent)
{
	QRegularExpression wRe("\\p{L}*");
	setRegularExpression(wRe);
}

void EQTextValidator::fixup(QString& input) const
{
	input = input.simplified();
}

QValidator::State EQTextValidator::validate(QString& input, int& pos) const
{
	fixup(input);
	return QRegularExpressionValidator::validate(input, pos);
}
