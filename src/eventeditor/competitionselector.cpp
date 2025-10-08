#include "competitionselector.h"

#include "competitions/competitionfactory.h"
#include "paradigms/paradigmfactory.h"

XkorCompetitionSelector::XkorCompetitionSelector(QWidget * parent) : QWidget(parent)
{
	optionsWidget = 0;

	comboBox = new QComboBox;
	comboBox->setInsertPolicy(QComboBox::InsertAlphabetically);
	connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateCompetition(int)));

	QFont headingFont;
	headingFont.setWeight(QFont::Bold);

	layout = new QGridLayout(this);
	QLabel * label = new QLabel(tr("Choose competition type"));
	label->setFont(headingFont);
	layout->addWidget(label, 0, 0, Qt::AlignCenter);
	layout->addWidget(comboBox, 1, 0, Qt::AlignTop | Qt::AlignHCenter);
	layout->setRowStretch(0, 0);
	layout->setRowStretch(1, 0);
	layout->setRowStretch(2, 1);
	layout->setContentsMargins(0, 0, 0, 0);

	updateCompetition(-1); // show a big competition options widget so that the layout has room for it
}

void XkorCompetitionSelector::setCompetition(QString competition)
{
	int index = comboBox->findData(competition, Qt::UserRole);
	comboBox->setCurrentIndex(index);
	updateCompetition(index);
}

void XkorCompetitionSelector::setSport(XkorSport sport, QHash<QString, QVariant> paradigmOptions)
{
	currentOptions = paradigmOptions;

	XkorAbstractParadigm * p = XkorParadigmFactory::newParadigm(&sport, paradigmOptions);

	// update the competition types list
	QString currentCompetitionType;
	currentCompetitionType = comboBox->itemData(comboBox->currentIndex(), Qt::UserRole).toString();

	if(optionsWidget)
		layout->removeWidget(optionsWidget);
	delete optionsWidget;
	optionsWidget = 0;

	comboBox->clear();

	QHash<QString, QString> competitionTypes = XkorCompetitionFactory::competitionTypes();
	for(QHash<QString, QString>::iterator i = competitionTypes.begin(); i != competitionTypes.end(); ++i)
		if(p->supportsCompetition(i.key()))
			comboBox->insertItem(0, i.value(), i.key());

	int newIndex = comboBox->findData(currentCompetitionType, Qt::UserRole);
	if(newIndex == -1)
		newIndex = comboBox->findData(p->defaultCompetition(), Qt::UserRole);
	if(newIndex == -1)
		newIndex = 0; // pick the first format in the list if nothing useful is available
	comboBox->setCurrentIndex(newIndex);
	comboBox->show(); // resizes the combo box
	updateCompetition(newIndex);

	delete p;
}

void XkorCompetitionSelector::updateCompetition(int index)
{
	QString newComp = comboBox->itemData(index).toString();

	// update the competition in the event
	emit competitionChanged(newComp);

	// check whether this competition has an options dialog
	XkorAbstractCompetition * c = XkorCompetitionFactory::newCompetition(newComp);

	if(optionsWidget)
		layout->removeWidget(optionsWidget);
	delete optionsWidget;
	optionsWidget = 0;

	if(c->hasOptionsWidget())
	{
		optionsWidget = c->newOptionsWidget(currentOptions);
		layout->addWidget(optionsWidget, 2, 0, Qt::AlignTop);
		connect(optionsWidget, SIGNAL(optionsChanged(QHash<QString, QVariant>)), this, SLOT(updateCompetitionOptions(QHash<QString, QVariant>)));
	}

	delete c;
}

void XkorCompetitionSelector::updateCompetitionOptions(QHash<QString, QVariant> newOptions)
{
	currentOptions = newOptions;
	emit competitionOptionsChanged(newOptions);
}
