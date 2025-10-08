#include "signuplisteditor/athletewidget.h"

#include <QFile>
#include <QFileDialog>
#include <QHeaderView>
#include <QTextStream>
#include <QtDebug>
#include <ctime>
#include "signuplisteditor/athletedelegate.h"

XkorAthleteWidget::XkorAthleteWidget(QStringList columnKeys, QStringList columnNames, QStringList columnTypes, double minDouble, double maxDouble, double doubleStep) : XkorAbstractAthleteWidget(columnKeys, columnNames, columnTypes, minDouble, maxDouble, doubleStep)
{
	init();
}

XkorAthleteWidget::~XkorAthleteWidget()
{
	delete importAction;
	delete dialog;
	delete delegate;
}

void XkorAthleteWidget::init()
{
	r.seed(time(0));
	dialog = 0;

	delegate = new XkorAthleteDelegate(m_columnTypes, m_minDouble, m_maxDouble, m_doubleStep);
	
	treeWidget->setColumnCount(m_columnKeys.size());
	treeWidget->setSortingEnabled(true);
	treeWidget->setItemDelegate(delegate);
	treeWidget->sortItems(0, Qt::AscendingOrder);
	treeWidget->setHeaderLabels(m_columnNames);
	treeWidget->header()->setStretchLastSection(false);
	
	// set the column widths
	for(int i = 0; i < m_columnTypes.size(); ++i)
		if(m_columnTypes[i] == "double" || m_columnTypes[i] == "golfStyle" || m_columnTypes[i] == "skill")
		{
			treeWidget->header()->setSectionResizeMode(i, QHeaderView::Fixed);
			treeWidget->header()->resizeSection(i, 80);
		}
		else
			treeWidget->header()->setSectionResizeMode(i, QHeaderView::Stretch);
	
	importAction = new QAction(QIcon(":/icons/document-import"), tr("Import from text file"), this);
	importAction->setEnabled(true);
	connect(importAction, SIGNAL(triggered()), this, SLOT(importAthletes()));
	
	QList<QAction *> actions;
	actions.append(insertAction);
	actions.append(deleteAction);
	actions.append(NULL);
	actions.append(importAction);
	setupLayout(actions);
}

QList<XkorAthlete> XkorAthleteWidget::athletes()
{
	QList<XkorAthlete> rval;
	for(QTreeWidgetItemIterator i(treeWidget); *i; ++i)
	{
		XkorAthlete a;
		a.name = (*i)->text(m_columnKeys.indexOf("name"));
		a.id = QUuid((*i)->data(m_columnKeys.indexOf("name"), Qt::UserRole).toString());
		a.nation = (*i)->text(m_columnKeys.indexOf("nation"));
		a.skill = (*i)->text(m_columnKeys.indexOf("skill")).toDouble();
		for(QStringList::iterator j = m_columnKeys.begin(); j != m_columnKeys.end(); ++j)
			a.setProperty(*j, (*i)->text(m_columnKeys.indexOf(*j)));
		rval.append(a);
	}
	return rval;
}

void XkorAthleteWidget::importAthletes()
{
	if(dialog)
		delete dialog;

	dialog = new QFileDialog(this);
	dialog->setWindowTitle(tr("Open semicolon-delimited athlete file"));
	dialog->setNameFilter(tr("Text files (*.txt)"));
	dialog->setWindowModality(Qt::WindowModal);
	dialog->setAcceptMode(QFileDialog::AcceptOpen);

	dialog->setDirectory("signupLists:/");
	dialog->open(this, SLOT(importAthletes(QString)));
}

void XkorAthleteWidget::importAthletes(QString filename)
{
	isInUse = true;

	if(filename != "")
	{
		QFile f(filename);
		if(!f.open(QIODevice::ReadOnly | QIODevice::Text))
			return;
		
		QTextStream ist(&f);
		
		while(!ist.atEnd())
		{
			QString line = ist.readLine();
			QStringList l = line.split(";");
			if(l.size() == 3)
			{
				QString athleteName = l[0].trimmed();
				QString athleteNation = l[1].trimmed();
				QString athleteSkill = l[2].trimmed();
				
				initItem(createItem(), athleteName, QUuid(r(), r(), r(), r(), r(), r(), r(), r(), r(), r(), r()), athleteNation, athleteSkill.toDouble());
			}
		}

		QDir path(filename);
		path.cdUp();
		emit signupListDirectoryChanged(path.canonicalPath());
	}

	isInUse = false;
	emit listChanged();
}

void XkorAthleteWidget::initItem(QTreeWidgetItem * item, QString athleteName, QUuid id, QString nation, double skill, QHash<QString, QVariant> properties)
{
	item->setText(m_columnKeys.indexOf("name"), athleteName);
	item->setData(m_columnKeys.indexOf("name"), Qt::UserRole, id.toString());
	item->setText(m_columnKeys.indexOf("nation"), nation);
	item->setText(m_columnKeys.indexOf("skill"), QString::number(skill));
	item->setTextAlignment(m_columnKeys.indexOf("skill"), Qt::AlignRight);
	for(int i = 0; i < m_columnTypes.size(); ++i)
		if(properties.contains(m_columnKeys[i]))
		{
			item->setText(i, properties[m_columnKeys[i]].toString());
			if(m_columnTypes[i] == "double" || m_columnTypes[i] == "skill")
				item->setTextAlignment(i, Qt::AlignRight);
		}
		else if(m_columnTypes[i] == "double")
		{
			item->setText(i, "0");
			item->setTextAlignment(i, Qt::AlignRight);
		}
}

void XkorAthleteWidget::setAthletes(QList<XkorAthlete> athletes)
{
	treeWidget->clear();
	for(QList<XkorAthlete>::iterator i = athletes.begin(); i != athletes.end(); ++i)
	{
		QTreeWidgetItem * item = new QTreeWidgetItem(treeWidget);
		item->setFlags(item->flags() | Qt::ItemIsEditable);
		initItem(item, i->name, i->id, i->nation, i->skill, i->properties);
	}
	emit listChanged();
}

void XkorAthleteWidget::setMaxRank(double newMax)
{
	delegate->setMaxRank(newMax);
}

void XkorAthleteWidget::setMinRank(double newMin)
{
	delegate->setMinRank(newMin);
}
