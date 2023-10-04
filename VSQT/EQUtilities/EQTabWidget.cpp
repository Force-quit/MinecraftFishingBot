#include "EQTabWidget.h"
#include <QTabWidget>

EQTabWidget::EQTabWidget(QWidget* parent)
	: QTabWidget(parent), locked(), previousTab()
{
	connect(this, &QTabWidget::currentChanged, this, &EQTabWidget::checkIfLocked);
}

EQTabWidget::~EQTabWidget() {}

void EQTabWidget::lock()
{
	locked = true;
}

void EQTabWidget::unlock()
{
	locked = false;
}

void EQTabWidget::checkIfLocked(int index)
{
	if (index != previousTab)
	{
		if (locked)
			setCurrentIndex(previousTab);
		else
			previousTab = index;
	}
}