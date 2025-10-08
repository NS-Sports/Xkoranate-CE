#ifndef SPORT_H
#define SPORT_H

#include <QtCore>
#include <random>

class XkorSport
{
	private:
		QString m_name, m_alphaName;
		QString m_discipline, m_event, m_scorinator;
		QString m_paradigm;
		QHash<QString, QVariant> m_paradigmOptions;
		
		QHash<QString, QMap<double, double> > m_dataPoints;

        std::mt19937 * r;
		
		double rand_kumaraswamy(double a, double b, bool skew);
		double randWeighted(double skill, double minConstant, double midConstant, double maxConstant, bool skew);
		double transformNumber(double x, QString index) const;
	public:
		XkorSport();

		void addDataPoints(QString key, QMap<double, double> value) { m_dataPoints.insert(key, value); }
		QString alphabetizedName();
		uint countValues() const { return m_dataPoints.size(); }
		QString discipline() const { return m_discipline; }
		QString event() const { return m_event; }
		double h2hScore(QString index, double skill, double oppSkill);
		bool hasValue(QString key) { return m_dataPoints.contains(key); }
		double individualScore(QString index);
		double individualScore(QString index, double skill);
		QString name() const { return m_name; }
		QString paradigm() const { return m_paradigm; }
		QHash<QString, QVariant> paradigmOptions() { return m_paradigmOptions; }
		double randGaussian();
		double randGaussianCapped(double cap);
		double randUniform();
		double randWeighted(double skill);
		double randWeighted(double skill, double oppSkill);
		QString scorinator() const { return m_scorinator; }
		void setAlphabetizedName(QString name) { m_alphaName = name; }
		void setDiscipline(QString discipline) { m_discipline = discipline; }
		void setEvent(QString event) { m_event = event; }
		void setName(QString name) { m_name = name; }
		void setParadigm(QString paradigm) { m_paradigm = paradigm; }
		void setParadigmOptions(QHash<QString, QVariant> paradigmOptions) { m_paradigmOptions = paradigmOptions; }
        void setPRNG(std::mt19937 * newR) { r = newR; }
		void setScorinator(QString scorinator) { m_scorinator = scorinator; }
};

#endif
