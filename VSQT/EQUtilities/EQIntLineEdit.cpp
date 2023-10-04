#include "EQIntLineEdit.h"
#include "EQIntValidator.h"

EQIntLineEdit::EQIntLineEdit(int minimum, int maximum, QWidget* parent)
	: QLineEdit(parent)
{
	setValidator(new EQIntValidator(minimum, maximum));
	connect(this, &QLineEdit::editingFinished, this, &EQIntLineEdit::emitValueChanged);
}

void EQIntLineEdit::emitValueChanged()
{
	emit valueChanged(text().toInt());
}

EQIntLineEdit::~EQIntLineEdit() {}