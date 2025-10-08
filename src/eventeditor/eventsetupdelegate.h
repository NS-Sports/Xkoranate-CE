#ifndef EVENTSETUPDELEGATE_H
#define EVENTSETUPDELEGATE_H

#include <QtGui>

#include <QWidget>
#include <QItemDelegate>

class XkorEventSetupDelegate : public QItemDelegate
{
	Q_OBJECT

	public:
		XkorEventSetupDelegate(QStringList * displayNames, QList<QUuid> * IDs, QObject * parent = 0) : QItemDelegate(parent), availableAthleteNames(displayNames), availableAthletes(IDs) {;;;}
		QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;
		void setEditorData(QWidget * editor, const QModelIndex & index) const;
		void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const;
		
	private:
		QStringList * availableAthleteNames;
		QList<QUuid> * availableAthletes;
	private slots:
		void prepareToCommit();
};

#endif
