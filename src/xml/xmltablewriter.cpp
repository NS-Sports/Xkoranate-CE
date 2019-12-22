#include "xml/xmltablewriter.h"

XkorXmlTableWriter::XkorXmlTableWriter(QString filename, XkorTable t)
{
	// stream writer settings
	setAutoFormatting(true);
	setAutoFormattingIndent(-1);

	QFile * f = new QFile(filename);
	f->open(QIODevice::WriteOnly);

	setDevice(f);

	writeStartDocument();
	writeStartElement("table");
	writeAttribute("version", "0.3");
	writeTable(t);
	writeEndDocument();

	delete f;
}

void XkorXmlTableWriter::writeTable(XkorTable t)
{
	writeStartElement("sortCriteria");
	std::vector<QString> sortCriteria = t.getSortCriteria();
	for(std::vector<QString>::iterator i = sortCriteria.begin(); i != sortCriteria.end(); ++i)
		writeTextElement("sortCriterion", *i);
	writeEndElement();

	writeTextElement("pointsForWin", QString::number(t.getPointsForWin()));
	writeTextElement("pointsForDraw", QString::number(t.getPointsForDraw()));
	writeTextElement("pointsForLoss", QString::number(t.getPointsForLoss()));

	writeTextElement("columnWidth", QString::number(t.getColumnWidth()));
	writeTextElement("showDraws", t.getShowDraws() ? "true" : "false");
	writeTextElement("showResultsGrid", t.getShowResultsGrid() ? "true" : "false");
	writeTextElement("goalName", t.getGoalName());

	writeStartElement("matches");
	std::vector<XkorTableMatch> matches = t.getMatches();
	for(std::vector<XkorTableMatch>::iterator i = matches.begin(); i != matches.end(); ++i)
		writeTextElement("match", QString("%1 %2–%3 %4").arg(i->team1).arg(i->score1).arg(i->score2).arg(i->team2));
	writeEndElement();
}
