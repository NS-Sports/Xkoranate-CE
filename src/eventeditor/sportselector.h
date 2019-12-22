#ifndef SPORTSELECTOR_H
#define SPORTSELECTOR_H

#include <QtGui>
#include "abstractoptionswidget.h"
#include "sport.h"

class XkorSportSelector : public QWidget
{
	Q_OBJECT
	public:
		XkorSportSelector(QWidget * parent = 0);
		void setSelectedSport(QString fullName);
		XkorSport sport();
		void updateSportList();

	signals:
		void paradigmOptionsChanged(QHash<QString, QVariant> paradigmOptions);
		void sportChanged(XkorSport sport);

	public slots:
		void setParadigmOptions(QHash<QString, QVariant> paradigmOptions);

	private:
		QTreeView * sportView;
		QItemSelectionModel * selectionModel;

		XkorAbstractOptionsWidget * paradigmOptionsWidget;
		QHash<QString, QVariant> currentParadigmOptions;
		QGridLayout * layout;

        bool isLoading;

	private slots:
		void setSelectedSport(QModelIndex index);
		void updateParadigmOptions(QHash<QString, QVariant> paradigmOptions);
		void updateSport();
};

#endif // SPORTSELECTOR_H
