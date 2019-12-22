#ifndef RESULT_H
#define RESULT_H

#include "athlete.h"
#include <QHash>
#include <QVariant>

class XkorResult
{
	public:
		XkorAthlete athlete;
		QHash<QString, QVariant> result;

		XkorResult() {;;;}
		XkorResult(double score, XkorAthlete ath = XkorAthlete()) : athlete(ath) { setScore(score); }
		XkorResult(double score, QString scoreString, XkorAthlete ath = XkorAthlete()) : athlete(ath) { setScore(score); setScoreString(scoreString); }
		bool contains(QString key) { return result.contains(key); }
		QString output() { return result.value("output").toString(); }
		double score() { return result.value("score").toDouble(); }
		QString scoreString() { if(result.contains("scoreString")) return result.value("scoreString").toString(); else return QString::number(score()); }
		void setOutput(QString s) { result["output"] = s; }
		void setScore(double s) { result["score"] = s; }
		void setScoreString(QString s) { result["scoreString"] = s; }
		QVariant value(QString key) { return result.value(key); }

		bool operator ==(const XkorResult & b) const { return this->athlete.id == b.athlete.id; }
};

#endif // RESULT_H
