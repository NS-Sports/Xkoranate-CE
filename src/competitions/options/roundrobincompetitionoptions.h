#ifndef ROUNDROBINCOMPETITIONOPTIONS_H
#define ROUNDROBINCOMPETITIONOPTIONS_H

#include <QTabWidget>
#include "abstractoptionswidget.h"
#include "tablegenerator/sortcriteriawidget.h"

class XkorRoundRobinCompetitionOptions : public XkorAbstractOptionsWidget
{
	Q_OBJECT
	
	private:
		QCheckBox * allowDraws, * showResultsGrid;
		QSpinBox * numberOfLegs;
		XkorSortCriteriaWidget * sc;
		QSpinBox * ptsWin, * ptsDraw, * ptsLoss;
		
	private slots:
		void checkResultsGridUsability(int legs);
		void setAllowDraws(int value);
		void setNumberOfLegs(int value);
		void setPointsForDraw(int value);
		void setPointsForLoss(int value);
		void setPointsForWin(int value);
		void setShowResultsGrid(int value);
		void setSortCriteria();
		
	public:
		XkorRoundRobinCompetitionOptions(QHash<QString, QVariant> opts, QWidget * parent = 0);
};

#endif
