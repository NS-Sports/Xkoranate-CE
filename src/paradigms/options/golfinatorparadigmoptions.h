#ifndef GOLFINATORPARADIGMOPTIONS_H
#define GOLFINATORPARADIGMOPTIONS_H

#include "abstractoptionswidget.h"
#include <QtGui>

#include <QTableWidget>

class XkorGolfinatorParadigmOptions : public XkorAbstractOptionsWidget
{
	Q_OBJECT

	public:
		XkorGolfinatorParadigmOptions(QHash<QString, QVariant> opts, QWidget * parent = 0);

	private:
		QTableWidget * course;

		QList<QVariant> defaultValue(QString column);

	private slots:
		void updateData(int row, int column);
};

#endif // GOLFINATORPARADIGMOPTIONS_H
