#ifndef WC36RPBONUSDELEGATE_H
#define WC36RPBONUSDELEGATE_H

#include <QItemDelegate>

class XkorWC36RPBonusDelegate : public QItemDelegate
{
	Q_OBJECT
	
	public:
		XkorWC36RPBonusDelegate(QObject * parent = 0) : QItemDelegate(parent) {;;;}
		QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;
		void setEditorData(QWidget * editor, const QModelIndex & index) const;
		void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const;
};

#endif
