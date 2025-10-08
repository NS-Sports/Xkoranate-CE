#include "signuplisteditor/athletedelegate.h"

#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

XkorAthleteDelegate::XkorAthleteDelegate(QStringList columnTypes, double minDouble, double maxDouble, double doubleStep, QObject * parent) : QItemDelegate(parent)
{
	m_columnTypes = columnTypes;
	m_minDouble = minDouble;
	m_maxDouble = maxDouble;
	m_doubleStep = doubleStep;
	minRank = 0;
	maxRank = 1;
}

QWidget * XkorAthleteDelegate::createEditor(QWidget * parent, const QStyleOptionViewItem &, const QModelIndex & index) const
{
	if(m_columnTypes[index.column()] == "double")
	{
		QDoubleSpinBox * spinBox = new QDoubleSpinBox(parent);
		spinBox->setAlignment(Qt::AlignRight);
		spinBox->setDecimals(3);
		spinBox->setMinimum(m_minDouble);
		spinBox->setMaximum(m_maxDouble);
		spinBox->setSingleStep(m_doubleStep);
		spinBox->setFrame(false);
		return spinBox;
	}
	else if(m_columnTypes[index.column()] == "golfStyle")
	{
		QLineEdit * lineEdit = new QLineEdit(parent);
		lineEdit->setFrame(false);

		QRegularExpression r("[1-6]{6}");
		QRegularExpressionValidator * validator = new QRegularExpressionValidator(r, lineEdit);
		lineEdit->setValidator(validator);

		return lineEdit;
	}
	else if(m_columnTypes[index.column()] == "skill")
	{
		QDoubleSpinBox * spinBox = new QDoubleSpinBox(parent);
		spinBox->setAlignment(Qt::AlignRight);
		spinBox->setDecimals(3);
		spinBox->setMinimum(minRank);
		spinBox->setMaximum(maxRank);
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

void XkorAthleteDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
	if(m_columnTypes[index.column()] == "double" || m_columnTypes[index.column()] == "skill")
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

void XkorAthleteDelegate::setMaxRank(double newMax)
{
	maxRank = newMax;
}

void XkorAthleteDelegate::setMinRank(double newMin)
{
	minRank = newMin;
}

void XkorAthleteDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const
{
	if(m_columnTypes[index.column()] == "double" || m_columnTypes[index.column()] == "skill")
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
