#include "EQTests.h"
#include <EQUtilities/EQIntLineEdit.h>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <EQUtilities/EQTextValidator.h>

EQTests::EQTests(QWidget* parent)
	: QMainWindow(parent)
{
	QLineEdit* line{ new QLineEdit };
	EQTextValidator* validator{ new EQTextValidator };
	line->setValidator(validator);

	setCentralWidget(line);
}

EQTests::~EQTests()
{}
