#ifndef ATHLETEDELEGATE_H
#define ATHLETEDELEGATE_H

#include <QtGui>

#include <QItemDelegate>

#include <QWidget>
#include <QItemDelegate>

class XkorAthleteDelegate : public QItemDelegate
{
	Q_OBJECT
	
	private:
		QStringList m_columnTypes;
		double m_minDouble, m_maxDouble, m_doubleStep;
		double minRank, maxRank;
	
	public:
		XkorAthleteDelegate(QStringList columnTypes, double minDouble, double maxDouble, double doubleStep, QObject * parent = 0);
		QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;
		void setEditorData(QWidget * editor, const QModelIndex & index) const;
		void setMaxRank(double newMax);
		void setMinRank(double newMin);
		void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const;
};

#endif
