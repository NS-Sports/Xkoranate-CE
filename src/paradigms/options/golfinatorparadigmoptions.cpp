#include "paradigms/options/golfinatorparadigmoptions.h"
#include <QHeaderView>
#include <QGridLayout>

XkorGolfinatorParadigmOptions::XkorGolfinatorParadigmOptions(QHash<QString, QVariant> opts, QWidget * parent) : XkorAbstractOptionsWidget(opts, parent)
{
	course = new QTableWidget;
	course->setColumnCount(7);
	course->setRowCount(18);
	course->setHorizontalHeaderLabels(QStringList() << tr("Yards") << tr("Par") << tr("Differential") << tr("Sand") << tr("Water") << tr("Narrow") << tr("Green"));
	course->setVerticalHeaderLabels(QStringList() << "1" << "2" << "3" << "4" << "5" << "6" << "7" << "8" << "9" << "10" << "11" << "12" << "13" << "14" << "15" << "16" << "17" << "18");

	course->setGridStyle(Qt::NoPen);
	course->setAlternatingRowColors(true);

	course->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	QFont f;
	QFontMetrics metrics(f);
	course->verticalHeader()->setDefaultSectionSize(metrics.height() + 2);

	QGridLayout * layout = new QGridLayout(this);
	layout->addWidget(course, 0, 0);
	layout->setContentsMargins(0, 0, 0, 0);

	// set values
	QList<QVariant> yardage = options.value("yardage", defaultValue("yardage")).toList();
	QList<QVariant> par = options.value("par", defaultValue("par")).toList();
	QList<QVariant> differential = options.value("differential", defaultValue("differential")).toList();
	QList<QVariant> sand = options.value("sand", defaultValue("sand")).toList();
	QList<QVariant> water = options.value("water", defaultValue("water")).toList();
	QList<QVariant> narrow = options.value("narrow", defaultValue("narrow")).toList();
	QList<QVariant> green = options.value("green", defaultValue("green")).toList();

	for(int i = 0; i < yardage.size(); ++i)
		course->setItem(i, 0, new QTableWidgetItem(yardage[i].toString()));
	for(int i = 0; i < par.size(); ++i)
		course->setItem(i, 1, new QTableWidgetItem(par[i].toString()));
	for(int i = 0; i < differential.size(); ++i)
		course->setItem(i, 2, new QTableWidgetItem(differential[i].toString()));
	for(int i = 0; i < sand.size(); ++i)
		course->setItem(i, 3, new QTableWidgetItem(sand[i].toString()));
	for(int i = 0; i < water.size(); ++i)
		course->setItem(i, 4, new QTableWidgetItem(water[i].toString()));
	for(int i = 0; i < narrow.size(); ++i)
		course->setItem(i, 5, new QTableWidgetItem(narrow[i].toString()));
	for(int i = 0; i < green.size(); ++i)
		course->setItem(i, 6, new QTableWidgetItem(green[i].toString()));

	connect(course, SIGNAL(cellChanged(int,int)), this, SLOT(updateData(int, int)));
}

QList<QVariant> XkorGolfinatorParadigmOptions::defaultValue(QString column)
{
	if(column == "yardage")
		return QList<QVariant>() << 435 << 529 << 198 << 517 << 451 << 408 << 178 << 457 << 414 << 495 << 505 << 155 << 614 << 435 << 478 << 381 << 455 << 357;
	else if(column == "par")
		return QList<QVariant>() << 4 << 5 << 3 << 5 << 4 << 4 << 3 << 4 << 4 << 4 << 4 << 3 << 5 << 4 << 4 << 4 << 4 << 4;
	else if(column == "differential")
		return QList<QVariant>() << 0.19 << -0.36 << 0.15 << 0 << 0.23 << 0.19 << 0.24 << 0.13 << 0.18 << 0.23 << 0.32 << 0.3 << -0.14 << 0.25 << 0.35 << 0.19 << 0.63 << -0.46;
	else if(column == "sand")
		return QList<QVariant>() << 1 << 1 << 1 << 0 << 1 << 0 << 1 << 1 << 1 << 0 << 0 << 0 << 1 << 1 << 0 << 0 << 1 << 0;
	else if(column == "water")
		return QList<QVariant>() << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 1 << 1 << 0 << 0 << 0 << 0 << 0 << 0;
	else if(column == "narrow")
		return QList<QVariant>() << 0 << 1 << 0 << 0 << 1 << 1 << 0 << 1 << 0 << 1 << 0 << 1 << 0 << 1 << 1 << 0 << 0 << 0;
	else if(column == "green")
		return QList<QVariant>() << 0 << 0 << 1 << 1 << 1 << 0 << 1 << 0 << 1 << 0 << 0 << 0 << 1 << 1 << 0 << 1 << 0 << 0;
	else
		return QList<QVariant>();
}

void XkorGolfinatorParadigmOptions::updateData(int row, int column)
{
	QString columnName;
	if(column == 0)
		columnName = "yardage";
	else if(column == 1)
		columnName = "par";
	else if(column == 2)
		columnName = "differential";
	else if(column == 3)
		columnName = "sand";
	else if(column == 4)
		columnName = "water";
	else if(column == 5)
		columnName = "narrow";
	else if(column == 6)
		columnName = "grass";
	else
		qDebug() << "non-existent column edited in XkorGolfinatorParadigmOptions::updateData(int, int)";

	QList<QVariant> columnValues = options.value(columnName, defaultValue(columnName)).toList();
	columnValues.insert(row, course->item(row, column)->text().toDouble());
	options[columnName] = columnValues;
	emit optionsChanged(options);
}
