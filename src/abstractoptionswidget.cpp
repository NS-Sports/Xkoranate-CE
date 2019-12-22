#include "abstractoptionswidget.h"

XkorAbstractOptionsWidget::XkorAbstractOptionsWidget(QHash<QString, QVariant> opts, QWidget * parent) : QWidget(parent)
{
	options = opts;
}

void XkorAbstractOptionsWidget::setOptions(QHash<QString, QVariant> newOptions)
{
	options = newOptions;
	emit optionsChanged(options);
}
