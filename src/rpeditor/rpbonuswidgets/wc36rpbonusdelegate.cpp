#include "rpeditor/rpbonuswidgets/wc36rpbonusdelegate.h"

#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QtDebug>

QWidget * XkorWC36RPBonusDelegate::createEditor(QWidget * parent, const QStyleOptionViewItem &, const QModelIndex & index) const
{
	if(index.column() == 1)
	{
		QComboBox * comboBox = new QComboBox(parent);
		comboBox->setFrame(false);
		comboBox->addItem("Slani", 0);
		comboBox->addItem("Poor", 1);
		comboBox->addItem("Good", 2);
		comboBox->addItem("Great", 3);
		comboBox->addItem("Exceptional", 4);
		return comboBox;
	}
	else if(index.column() == 2)
	{
		QSpinBox * spinBox = new QSpinBox(parent);
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

void XkorWC36RPBonusDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
	if(index.column() == 1)
	{
		QComboBox * comboBox = qobject_cast<QComboBox *>(editor);
		comboBox->setCurrentIndex(comboBox->findText(index.model()->data(index, Qt::DisplayRole).toString()));
		//comboBox->setCurrentIndex(comboBox->findData(index.model()->data(index, 1093)));
	}
	else if(index.column() == 2)
	{
		QSpinBox * spinBox = qobject_cast<QSpinBox *>(editor);
		spinBox->setValue(index.model()->data(index, Qt::DisplayRole).toInt());
	}
	else
	{
		QLineEdit * lineEdit = qobject_cast<QLineEdit *>(editor);
		lineEdit->setText(index.model()->data(index, Qt::DisplayRole).toString());
	}
}

void XkorWC36RPBonusDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const
{
	if(index.column() == 1)
	{
		QComboBox * comboBox = qobject_cast<QComboBox *>(editor);
		model->setData(index, comboBox->currentText());
		model->setData(index, comboBox->itemData(comboBox->currentIndex()), 1093);
	}
	else if(index.column() == 2)
	{
		QSpinBox * spinBox = qobject_cast<QSpinBox *>(editor);
		model->setData(index, spinBox->value());
	}
	else
	{
		QLineEdit * lineEdit = qobject_cast<QLineEdit *>(editor);
		model->setData(index, lineEdit->text());
	}
}
