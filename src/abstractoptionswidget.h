#ifndef ABSTRACTOPTIONSWIDGET_H
#define ABSTRACTOPTIONSWIDGET_H

#include <QtCore>
#include <QtGui>

#include <QWidget>

class XkorAbstractOptionsWidget : public QWidget
{
	Q_OBJECT
	
	protected:
		QHash<QString, QVariant> options;
		
	public:
		XkorAbstractOptionsWidget(QHash<QString, QVariant> opts, QWidget * parent = 0);
		QHash<QString, QVariant> getOptions() { return options; }
		void setOptions(QHash<QString, QVariant> newOptions);

	signals:
		void optionsChanged(QHash<QString, QVariant> opts);
};

#endif
