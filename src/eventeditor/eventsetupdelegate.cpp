#include "eventsetupdelegate.h"

#include <QComboBox>
#include <QLineEdit>

QWidget * XkorEventSetupDelegate::createEditor(QWidget * parent, const QStyleOptionViewItem &, const QModelIndex & index) const
{
	if(index.parent() != QModelIndex()) // if this is an athlete, not a group name
	{
		QComboBox * comboBox = new QComboBox(parent);
		comboBox->setFrame(false);
		comboBox->insertItems(0, *availableAthleteNames);
		connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(prepareToCommit()));
		return comboBox;
	}
	else
	{
		QLineEdit * lineEdit = new QLineEdit(parent);
		lineEdit->setFrame(false);
		connect(lineEdit, SIGNAL(textEdited(QString)), this, SLOT(prepareToCommit()));
		return lineEdit;
	}
}

void XkorEventSetupDelegate::prepareToCommit()
{
	emit commitData(qobject_cast<QWidget *>(sender()));
}

void XkorEventSetupDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
	if(index.parent() != QModelIndex()) // if this is an athlete, not a group name
	{
		QComboBox * comboBox = qobject_cast<QComboBox *>(editor);
		comboBox->setCurrentIndex(comboBox->findText(index.model()->data(index, Qt::DisplayRole).toString()));
	}
	else
	{
		QLineEdit * lineEdit = qobject_cast<QLineEdit *>(editor);
		lineEdit->setText(index.model()->data(index, Qt::DisplayRole).toString());
	}
}

void XkorEventSetupDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const
{
	if(index.parent() != QModelIndex()) // if this is an athlete, not a group name
	{
		QComboBox * comboBox = qobject_cast<QComboBox *>(editor);
		QString longName = comboBox->currentText();
		int athleteIndex = availableAthleteNames->indexOf(longName);
		QUuid id;
		if(athleteIndex != -1)
		{
			id = availableAthletes->at(athleteIndex);
		}
		else
		{
			id = QUuid();
			longName = tr("<unknown participant>");
		}
		model->setData(index, longName);
		model->setData(index, id.toString(), Qt::UserRole);
	}
	else
	{
		QLineEdit * lineEdit = qobject_cast<QLineEdit *>(editor);
		model->setData(index, lineEdit->text());
	}
}
