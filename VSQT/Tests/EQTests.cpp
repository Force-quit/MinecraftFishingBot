#include "EQTests.h"
#include <EQUtilities/EQIntLineEdit.h>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <EQUtilities/EQShortcutListener.h>

EQTests::EQTests(QWidget* parent)
	: QMainWindow(parent)
{
	EQShortcutListener* s{ new EQShortcutListener("Blabla") };
	s->startListening();
	setCentralWidget(s);
}

EQTests::~EQTests()
{}
