#include "EQIntLineEdit.h"
#include "EQIntValidator.h"

EQIntLineEdit::EQIntLineEdit(int minimum, int maximum, QWidget* parent)
	: QLineEdit(parent), mLastValue{}
{
	setValidator(new EQIntValidator(minimum, maximum));
	connect(this, &QLineEdit::editingFinished, this, &EQIntLineEdit::verifyValue);
}

void EQIntLineEdit::verifyValue()
{
	QString wText(text());
	const QIntValidator* wValidator{ dynamic_cast<const QIntValidator*>(validator()) };

	if (wText.isEmpty() || wText.toInt() < wValidator->bottom())
		setText(QString::number(wValidator->bottom()));

	wText = text();
	int wCurrentValue{ wText.toInt() };
	if (wCurrentValue != mLastValue)
	{
		mLastValue = wCurrentValue;
		emit valueChanged(mLastValue);
	}
}

void EQIntLineEdit::focusOutEvent(QFocusEvent* e)
{
	QLineEdit::focusOutEvent(e);
	verifyValue();
}

EQIntLineEdit::~EQIntLineEdit() {}