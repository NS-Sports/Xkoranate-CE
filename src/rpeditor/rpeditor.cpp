#include "rpeditor/rpeditor.h"
#include <QButtonGroup>

#include <QFile>
#include <QFileDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QToolButton>
#include <QtDebug>
#include "rplist.h"

#include "rpeditor/rpbonuswidgets/olympicrpbonuswidget.h"
#include "rpeditor/rpbonuswidgets/wc36rpbonuswidget.h"

XkorRpEditor::XkorRpEditor(QWidget * parent) : QWidget(parent)
{
	m_data = 0;
	isLoading = false;

	m_useTeamBonus = new QRadioButton(tr("Teams"));
	m_useParticipantBonus = new QRadioButton(tr("Participants"));
	m_useTeamBonus->setChecked(true);
	QButtonGroup * bonusStyle = new QButtonGroup;
	bonusStyle->addButton(m_useTeamBonus);
	bonusStyle->addButton(m_useParticipantBonus);
	connect(m_useTeamBonus, SIGNAL(toggled(bool)), this, SLOT(setDataChanged()));
	connect(m_useParticipantBonus, SIGNAL(toggled(bool)), this, SLOT(setDataChanged()));

	QGridLayout * assignBonusesTo = new QGridLayout;
	assignBonusesTo->addWidget(m_useTeamBonus, 0, 0);
	assignBonusesTo->addWidget(m_useParticipantBonus, 0, 1);
	assignBonusesTo->setColumnStretch(0, 0); // don’t stretch the left column
	assignBonusesTo->setColumnStretch(1, 1);

	m_bonusEffect = new QDoubleSpinBox;
	m_bonusEffect->setMinimum(0);
	m_bonusEffect->setMaximum(100);
	m_bonusEffect->setSingleStep(1);
	m_bonusEffect->setSuffix("%");
	m_bonusEffect->setDecimals(1);
	m_bonusEffect->setValue(15);
	connect(m_bonusEffect, SIGNAL(valueChanged(double)), this, SLOT(setDataChanged()));

	m_rpCalcType = new QComboBox;
	m_rpCalcType->addItem(tr("Absolute"), "olympic");
	m_rpCalcType->addItem(tr("Relative"), "relative");
	m_rpCalcType->addItem(tr("World Cup 36"), "wc36");
	
	// tree widget
	m_rpBonus = new XkorOlympicRPBonusWidget;
	connect(m_useTeamBonus, SIGNAL(toggled(bool)), m_rpBonus, SLOT(setUseTeamBonus(bool)));
	connect(m_useParticipantBonus, SIGNAL(toggled(bool)), m_rpBonus, SLOT(setUseParticipantBonus(bool)));
	
	// form layout
	QFormLayout * rpForm = new QFormLayout;
	rpForm->addRow(tr("Assign bonuses to:"), assignBonusesTo);
	rpForm->addRow(tr("Effect of bonus:"), m_bonusEffect);
	rpForm->addRow(tr("Bonus formula:"), m_rpCalcType);
	
	// tree widget layout
	m_bonusLayout = new QHBoxLayout;
	m_bonusLayout->addWidget(m_rpBonus);

	// RP bonus group
	QVBoxLayout * rpLayout = new QVBoxLayout(this);
	rpLayout->addLayout(rpForm, 0);
	rpLayout->addLayout(m_bonusLayout, 1);

	connect(m_rpCalcType, SIGNAL(currentIndexChanged(int)), this, SLOT(setDataChanged()));
	connect(m_rpCalcType, SIGNAL(currentIndexChanged(int)), this, SLOT(updateRPBonusWidget()));
	connect(m_rpBonus, SIGNAL(listChanged()), this, SLOT(setDataChanged()));
}

void XkorRpEditor::setData(XkorRPList * data)
{
	isLoading = true; // prevent dataChanged from being emitted
	if(data)
	{
		updateData(); // we want to save any edits to the previous signup list (if any)

		if(data->useTeams())
			m_useTeamBonus->setChecked(true);
		else
			m_useParticipantBonus->setChecked(true);
		m_bonusEffect->setValue(data->rpEffect());
		m_rpCalcType->setCurrentIndex(m_rpCalcType->findData(data->rpCalculationType()));
		updateRPBonusWidget();
		m_rpBonus->setBonuses(data->bonuses());
		m_rpBonus->setOptions(data->rpOptions());
		m_rpBonus->setMaxBonus(data->maxBonus());
		m_rpBonus->setMinBonus(data->minBonus());
	}
	else
		qDebug() << "Null pointer in XkorRpEditor::setData(XkorRPList *)";
	m_data = data;
	isLoading = false; // allow dataChanged to be emitted if the user does stuff
}

void XkorRpEditor::updateData()
{
	if(m_data)
	{
		m_data->setUseTeams(m_useTeamBonus->isChecked());
		m_data->setRPEffect(m_bonusEffect->value());
		m_data->setRPCalculationType(m_rpCalcType->itemData(m_rpCalcType->currentIndex()).toString());
		m_data->setBonuses(m_rpBonus->bonuses());
		m_data->setRPOptions(m_rpBonus->options());
		m_data->setMaxBonus(m_rpBonus->maxBonus());
		m_data->setMinBonus(m_rpBonus->minBonus());
	}
	else
		qDebug() << "Null pointer in XkorRpEditor::updateData()";
}

void XkorRpEditor::updateRPBonusWidget()
{
	QString newRPCalcType = m_rpCalcType->itemData(m_rpCalcType->currentIndex()).toString();
	if(m_currentRPCalcType != newRPCalcType)
	{
		if(m_rpBonus->bonuses().size() > 0)
		{
			QMessageBox warning(QMessageBox::NoIcon, tr("xkoranate"), tr("Are you sure you want to reset all bonuses by changing the bonus formula?"), QMessageBox::Ok | QMessageBox::Cancel, this);
			warning.setInformativeText(tr("If you change the bonus formula, all of your bonuses will be lost."));
			int iconSize = style()->pixelMetric(QStyle::PM_MessageBoxIconSize);
			warning.setIconPixmap(QPixmap(":/icons/xkoranate").scaled(iconSize, iconSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
			warning.setDefaultButton(QMessageBox::Cancel);
			warning.setEscapeButton(QMessageBox::Cancel);
			warning.setWindowModality(Qt::WindowModal);
			int r = warning.exec();
			if(r == QMessageBox::Cancel)
			{
				m_rpCalcType->setCurrentIndex(m_rpCalcType->findData(m_currentRPCalcType));
				return;
			}
		}
		
		m_bonusLayout->removeWidget(m_rpBonus);
		delete m_rpBonus;
		if(newRPCalcType == "wc36")
			m_rpBonus = new XkorWC36RPBonusWidget;
		else
			m_rpBonus = new XkorOlympicRPBonusWidget;

		updateData();
		if(m_data && !m_data->useTeams())
			m_rpBonus->setUseParticipantBonus();
		else
			m_rpBonus->setUseTeamBonus();

		connect(m_rpBonus, SIGNAL(listChanged()), this, SLOT(setDataChanged()));
		connect(m_useTeamBonus, SIGNAL(toggled(bool)), m_rpBonus, SLOT(setUseTeamBonus(bool)));
		connect(m_useParticipantBonus, SIGNAL(toggled(bool)), m_rpBonus, SLOT(setUseParticipantBonus(bool)));
		m_bonusLayout->addWidget(m_rpBonus);
		m_currentRPCalcType = newRPCalcType;
	}
}
