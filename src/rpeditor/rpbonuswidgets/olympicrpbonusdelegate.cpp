#include "rpeditor/rpbonuswidgets/olympicrpbonusdelegate.h"

#include <QLineEdit>
#include <QDoubleSpinBox>

QWidget * XkorOlympicRPBonusDelegate::createEditor(QWidget * parent, const QStyleOptionViewItem &, const QModelIndex & index) const
{
	if(index.column() == 1)
	{
		QDoubleSpinBox * spinBox = new QDoubleSpinBox(parent);
		spinBox->setDecimals(3);
		spinBox->setMinimum(0);
		spinBox->setMaximum(1);
		spinBox->setSingleStep(0.01);
		spinBox->setFrame(false);
		return spinBox;
	}
	else
	{
		QLineEdit * lineEdit = new QLineEdit(parent);
		lineEdit->setFrame(false);
		return lineEdit;
	}
}

void XkorOlympicRPBonusDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
	if(index.column() == 1)
	{
		QDoubleSpinBox * spinBox = qobject_cast<QDoubleSpinBox *>(editor);
		spinBox->setValue(index.model()->data(index, Qt::DisplayRole).toDouble());
	}
	else
	{
		QLineEdit * lineEdit = qobject_cast<QLineEdit *>(editor);
		lineEdit->setText(index.model()->data(index, Qt::DisplayRole).toString());
	}
}

void XkorOlympicRPBonusDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const
{
	if(index.column() == 1)
	{
		QDoubleSpinBox * spinBox = qobject_cast<QDoubleSpinBox *>(editor);
		model->setData(index, spinBox->value());
	}
	else
	{
		QLineEdit * lineEdit = qobject_cast<QLineEdit *>(editor);
		model->setData(index, lineEdit->text());
	}
}
