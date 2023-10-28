#include "EQTests.h"
#include <EQUtilities/EQIntLineEdit.h>
#include <QHBoxLayout>
#include <QLabel>

EQTests::EQTests(QWidget* parent)
	: QMainWindow(parent)
{
	EQIntLineEdit* line{ new EQIntLineEdit(69, 420) };
	line->setText(QString::number(420));
	connect(line, &EQIntLineEdit::valueChanged, [](int newValue) {
		qDebug() << newValue;
		});
	setCentralWidget(line);
}

EQTests::~EQTests()
{}
