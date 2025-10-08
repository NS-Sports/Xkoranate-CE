#ifndef SORTCRITERIADELEGATE_H
#define SORTCRITERIADELEGATE_H

#include <QItemDelegate>

#include <QWidget>
#include <QItemDelegate>

class XkorSortCriteriaDelegate : public QItemDelegate
{
	Q_OBJECT
	
	public:
		XkorSortCriteriaDelegate(QStringList types, QStringList names, QObject * parent = 0) : QItemDelegate(parent), sortTypes(types), sortNames(names) {;;;}
		QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;
		void setEditorData(QWidget * editor, const QModelIndex & index) const;
		void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const;
		
	private:
		QStringList sortTypes, sortNames;
};

#endif
