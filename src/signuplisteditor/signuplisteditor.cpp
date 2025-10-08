#include "signuplisteditor/signuplisteditor.h"
#include <QLabel>
#include <QFormLayout>

#include "paradigms/paradigmfactory.h"

XkorSignupListEditor::XkorSignupListEditor(QWidget * parent) : QWidget(parent)
{
	isLoading = false;

	athletes = 0;

	QFont headingFont;
	headingFont.setWeight(QFont::Bold);
	QLabel * label = new QLabel(tr("Set participants"));
	label->setFont(headingFont);

	// minimum rank
	minRank = new QDoubleSpinBox;
	minRank->setDecimals(3);
	minRank->setRange(-999.999, 1);
	connect(minRank, SIGNAL(valueChanged(double)), this, SLOT(minRankChanged(double)));
	
	// maximum rank
	maxRank = new QDoubleSpinBox;
	maxRank->setValue(100);
	maxRank->setDecimals(3);
	maxRank->setRange(0, 9999.999);
	connect(maxRank, SIGNAL(valueChanged(double)), this, SLOT(maxRankChanged(double)));
	
	// form layout
	QFormLayout * form = new QFormLayout;
	form->addRow(tr("Minimum skill:"), minRank);
	form->addRow(tr("Maximum skill:"), maxRank);
	
	// layout
	layout = new QVBoxLayout(this);
	layout->addWidget(label, 0, Qt::AlignCenter);
	layout->addLayout(form, 0);
	layout->setContentsMargins(0, 0, 0, 0);

	connect(minRank, SIGNAL(valueChanged(double)), this, SLOT(setDataChanged()));
	connect(maxRank, SIGNAL(valueChanged(double)), this, SLOT(setDataChanged()));
}

void XkorSignupListEditor::maxRankChanged(double newMax)
{
	// min rank can’t be higher than max rank
	minRank->setMaximum(newMax);
	
	// inform the athlete widget
	if(athletes)
		athletes->setMaxRank(newMax);
}

void XkorSignupListEditor::minRankChanged(double newMin)
{
	// max rank can’t be lower than min rank
	maxRank->setMinimum(newMin);
	
	// inform the athlete widget
	if(athletes)
		athletes->setMinRank(newMin);
}

void XkorSignupListEditor::setData(XkorSignupList data)
{
	isLoading = true; // prevent dataChanged from being emitted

	minRank->setValue(data.minRank());
	maxRank->setValue(data.maxRank());
	if(athletes)
	{
		athletes->setMinRank(data.minRank());
		athletes->setMaxRank(data.maxRank());
		athletes->setAthletes(data.athletes());
	}

	m_data = data;
	isLoading = false; // allow dataChanged to be emitted if the user does stuff
}

void XkorSignupListEditor::setSignupListDirectory(QString dir)
{
	emit signupListDirectoryChanged(dir);
}

void XkorSignupListEditor::setSport(XkorSport sport, QHash<QString, QVariant> paradigmOptions)
{
	updateData();

	if(athletes)
		layout->removeWidget(athletes);
	delete athletes;

	XkorAbstractParadigm * p = XkorParadigmFactory::newParadigm(&sport, paradigmOptions);

	athletes = p->newAthleteWidget();
	setData(m_data);
	layout->addWidget(athletes, 1);
	connect(athletes, SIGNAL(listChanged()), this, SLOT(setDataChanged()));
    connect(athletes, SIGNAL(itemDeleted(QUuid)), this, SLOT(setItemDeleted(QUuid)));
	connect(athletes, SIGNAL(signupListDirectoryChanged(QString)), this, SLOT(setSignupListDirectory(QString)));
}

void XkorSignupListEditor::updateData()
{
	m_data.setMinRank(minRank->value());
	m_data.setMaxRank(maxRank->value());
	if(athletes)
		m_data.setAthletes(athletes->athletes());
}
