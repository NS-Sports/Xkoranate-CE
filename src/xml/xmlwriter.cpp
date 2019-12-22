#include "xmlwriter.h"

XkorXmlWriter::XkorXmlWriter(QString filename, XkorRPList * rpList, QList<QPair<QUuid, XkorEvent *> > events)
{
	// stream writer settings
	setAutoFormatting(true);
	setAutoFormattingIndent(-1);

	QFile * f = new QFile(filename);
	f->open(QIODevice::WriteOnly);

	setDevice(f);

	writeStartDocument();
	writeStartElement("scorinationFile");
	writeAttribute("version", "0.3");
	writeRPList(rpList);
	writeEvents(events);
	writeEndDocument();

	delete f;
}

void XkorXmlWriter::writeEvents(QList<QPair<QUuid, XkorEvent *> > events)
{
	for(QList<QPair<QUuid, XkorEvent *> >::iterator i = events.begin(); i != events.end(); ++i)
	{
		writeStartElement("event");
		writeAttribute("id", i->first.toString());
		writeAttribute("name", i->second->name());
		writeTextElement("sport", i->second->sport());
		writeTextElement("competition", i->second->competition());

		writeStartElement("paradigmOptions");
		QHash<QString, QVariant> paradigmOptions = i->second->paradigmOptions();
		for(QHash<QString, QVariant>::iterator j = paradigmOptions.begin(); j != paradigmOptions.end(); ++j)
			writeVariant(j.value(), j.key());
		writeEndElement();

		writeStartElement("competitionOptions");
		QHash<QString, QVariant> competitionOptions = i->second->competitionOptions();
		for(QHash<QString, QVariant>::iterator j = competitionOptions.begin(); j != competitionOptions.end(); ++j)
			writeVariant(j.value(), j.key());
		writeEndElement();

		writeStartElement("results");
		QHash<int, QString> results = i->second->results();
		for(QHash<int, QString>::iterator j = results.begin(); j != results.end(); ++j)
		{
			writeStartElement("result");
			writeAttribute("matchday", QString::number(j.key()));
			writeCharacters(j.value());
			writeEndElement();
		}
		writeEndElement();

		writeStartElement("signupList");
		XkorSignupList signupList = i->second->signupList();
		writeTextElement("minRank", QString::number(signupList.minRank()));
		writeTextElement("maxRank", QString::number(signupList.maxRank()));

		QList<XkorAthlete> athletes = signupList.athletes();
		for(QList<XkorAthlete>::iterator j = athletes.begin(); j != athletes.end(); ++j)
		{
			writeStartElement("signup");
			writeAttribute("id", j->id.toString());
			writeAttribute("name", j->name);
			writeAttribute("nation", j->nation);
			writeAttribute("skill", QString::number(j->skill));

			QHash<QString, QVariant> signupProperties = j->properties;
			for(QHash<QString, QVariant>::iterator k = signupProperties.begin(); k != signupProperties.end(); ++k)
				writeVariant(k.value(), k.key());
			writeEndElement();
		}
		writeEndElement();

		QList<XkorGroup> groups = i->second->groups();
		for(QList<XkorGroup>::iterator j = groups.begin(); j != groups.end(); ++j)
		{
			writeStartElement("group");
			writeAttribute("name", j->name);
			for(QList<QUuid>::iterator k = j->athletes.begin(); k != j->athletes.end(); ++k)
				writeTextElement("signup", k->toString());
			writeEndElement();
		}
		writeEndElement();
	}
}

void XkorXmlWriter::writeRPList(XkorRPList * rpList)
{
	writeStartElement("rpList");
	writeTextElement("useTeams", rpList->useTeams() ? "true" : "false");
	writeTextElement("competitionName", rpList->competitionName());
	writeTextElement("rpEffect", QString::number(rpList->rpEffect()));
	writeTextElement("rpCalculationType", rpList->rpCalculationType());
	writeTextElement("minBonus", QString::number(rpList->minBonus()));
	writeTextElement("maxBonus", QString::number(rpList->maxBonus()));

	QHash<QString, QVariant> rpOptions = rpList->rpOptions();
	writeStartElement("rpOptions");
	for(QHash<QString, QVariant>::iterator j = rpOptions.begin(); j != rpOptions.end(); ++j)
		writeVariant(j.value(), j.key());
	writeEndElement();

	QHash<QString, QHash<QString, double> > bonuses = rpList->bonuses();
	for(QHash<QString, QHash<QString, double> >::iterator j = bonuses.begin(); j != bonuses.end(); ++j)
	{
		writeStartElement("nation");
		writeAttribute("name", j.key());
		for(QHash<QString, double>::iterator k = j.value().begin(); k != j.value().end(); ++k)
		{
			writeStartElement("property");
			writeAttribute("type", k.key());
			writeCharacters(QString::number(k.value()));
			writeEndElement();
		}
		writeEndElement();
	}
	writeEndElement();
}

void XkorXmlWriter::writeVariant(QVariant value, QString key)
{
	if(value.type() == QVariant::Double)
		writeStartElement("double");
	else if(value.type() == QVariant::Int || value.type() == QVariant::UInt)
		writeStartElement("int");
	else if(value.type() == QVariant::List)
	{
		writeStartElement("list");
		if(key != QString())
			writeAttribute("type", key);
		QList<QVariant> list = value.toList();
		for(QList<QVariant>::iterator i = list.begin(); i != list.end(); ++i)
			writeVariant(*i);
		writeEndElement();
		return; // don’t run the code intended for basic types
	}
	else if(value.type() == QVariant::String)
		writeStartElement("string");
	else
	{
		qDebug() << "unknown QVariant type" << value.type() << "encountered in XkorXmlWriter::writeVariant(QString, QVariant)";
		return; // WTF?
	}

	// remaining code is used for double, int, string
	if(key != QString())
		writeAttribute("type", key);
	writeCharacters(value.toString());
	writeEndElement();
}
