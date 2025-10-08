#ifndef OLYMPICRPBONUSDELEGATE_H
#define OLYMPICRPBONUSDELEGATE_H

#include <QItemDelegate>

#include <QWidget>
#include <QItemDelegate>

class XkorOlympicRPBonusDelegate : public QItemDelegate
{
	Q_OBJECT
	
	public:
		XkorOlympicRPBonusDelegate(QObject * parent = 0) : QItemDelegate(parent) {;;;}
		QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;
		void setEditorData(QWidget * editor, const QModelIndex & index) const;
		void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const;
};

#endif
