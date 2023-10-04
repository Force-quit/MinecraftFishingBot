#pragma once

#include <QIntValidator>
#include <QString>

class EQIntValidator  : public QIntValidator
{
	Q_OBJECT

public:
	EQIntValidator(int minimum, int maximum, QObject* parent = nullptr);
	~EQIntValidator();

	QValidator::State validate(QString& input, int& pos) const override;
};
