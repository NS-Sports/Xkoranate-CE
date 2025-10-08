#ifndef COMPETITIONSELECTOR_H
#define COMPETITIONSELECTOR_H

#include <QtGui>
#include "sport.h"

#include <QComboBox>

#include <QWidget>
#include <QGridLayout>
#include <QComboBox>

class XkorCompetitionSelector : public QWidget
{
	Q_OBJECT
	public:
		XkorCompetitionSelector(QWidget * parent = 0);

	signals:
		void competitionChanged(QString competition);
		void competitionOptionsChanged(QHash<QString, QVariant> competitionOptions);

	public slots:
		void setCompetition(QString competition);
		void setSport(XkorSport sport, QHash<QString, QVariant> paradigmOptions);

	private:
		QGridLayout * layout;
		QComboBox * comboBox;
		QWidget * optionsWidget;

		QHash<QString, QVariant> currentOptions;

	private slots:
		void updateCompetition(int index);
		void updateCompetitionOptions(QHash<QString, QVariant> newOptions);
};

#endif // COMPETITIONSELECTOR_H
