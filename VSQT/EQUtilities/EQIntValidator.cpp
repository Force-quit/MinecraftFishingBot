#include "EQIntValidator.h"

EQIntValidator::EQIntValidator(int minimum, int maximum, QObject* parent)
	: QIntValidator(minimum, maximum, parent)
{}

QValidator::State EQIntValidator::validate(QString& input, int& pos) const
{
	QValidator::State result{ QIntValidator::validate(input, pos) };

	if (result == QValidator::State::Intermediate)
		if (input.toInt() < bottom() || input.toInt() > top())
			result = QValidator::State::Invalid;

	return result;
}
EQIntValidator::~EQIntValidator(){}