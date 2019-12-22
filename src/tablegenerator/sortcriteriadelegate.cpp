#include "tablegenerator/sortcriteriadelegate.h"

#include <QComboBox>

QWidget * XkorSortCriteriaDelegate::createEditor(QWidget * parent, const QStyleOptionViewItem &, const QModelIndex &) const
{
	QComboBox * comboBox = new QComboBox(parent);
	comboBox->setFrame(false);
	comboBox->insertItems(0, sortNames);
	return comboBox;
}

void XkorSortCriteriaDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
	QComboBox * comboBox = qobject_cast<QComboBox *>(editor);
	comboBox->setCurrentIndex(comboBox->findText(index.model()->data(index, Qt::DisplayRole).toString()));
}

void XkorSortCriteriaDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const
{
	QComboBox * comboBox = qobject_cast<QComboBox *>(editor);
	QString name = comboBox->currentText();
	int n = sortNames.indexOf(name);
	if(n >= 0 && n < sortTypes.size())
		model->setData(index, sortTypes[n], 1093);
	else
		model->setData(index, tr("<unknown sort type %1>").arg(name), 1093);
	model->setData(index, name);
}
