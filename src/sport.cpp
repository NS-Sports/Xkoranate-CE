#include "sport.h"

#include <QtDebug>
#include <iterator>

using namespace std;

XkorSport::XkorSport()
{
	r = 0;
}

QString XkorSport::alphabetizedName()
{
	if(m_alphaName != QString())
		return m_alphaName;
	else
		return m_name;
}

double XkorSport::h2hScore(QString index, double skill, double oppSkill)
{
	return transformNumber(randWeighted(skill, oppSkill), index);
}

double XkorSport::individualScore(QString index)
{
	return transformNumber(randUniform(), index);
}

double XkorSport::individualScore(QString index, double skill)
{
	return transformNumber(randWeighted(skill), index);
}

double XkorSport::randGaussian()
{
	// Box–Muller method
	double r1 = randUniform(), r2 = randUniform();
	return pow(-2 * log(r1), 0.5) * cos(2 * M_PI * r2);
}

double XkorSport::randGaussianCapped(double cap)
{
	double rval = randGaussian();
	if(rval < cap)
		rval = cap;
	else if(rval > 1 - cap)
		rval = 1 - cap;
	return rval;
}

double XkorSport::rand_kumaraswamy(double a, double b, bool skew) // skew = true : skewed toward 1; false : skewed toward 0
{
	double rand = randUniform();
	double rval;

	if(skew)
		rval = pow(1 - pow(1 - rand, 1 / b), 1 / a);
	else
		rval = 1 - pow(1 - pow(1 - rand, 1 / a), 1 / b);
	return rval;
}

double XkorSport::randUniform()
{
	if(r)
		return ((*r)() - r->min()) / (double)(r->max() - r->min());
	else
	{
		qDebug() << "no PRNG set";
		return -1;
	}
}

double XkorSport::randWeighted(double skill)
{
	skill = 0.5 - (0.5 - skill > 0 ? 1 : -1) * pow(abs(0.5 - skill), 1 / 4.0) / pow(2, 3 / 4.0);
	return randWeighted(skill, 1.4, 2, 4, true);
}

double XkorSport::randWeighted(double skill, double oppSkill)
{
	return randWeighted(0.5 + (skill - oppSkill) / 2, 1.02, 2.4, 3.8, false);
}


double XkorSport::randWeighted(double skill, double minConstant, double midConstant, double maxConstant, bool skew)
{
	double rand = 0;

	double scorSkill = skill;
	double a = (scorSkill > 0.5 ? (scorSkill - 0.5) * 2 * (maxConstant - midConstant) + midConstant : scorSkill * 2 * (midConstant - minConstant) + minConstant);
	double b = (scorSkill < 0.5 ? (0.5 - scorSkill) * 2 * (maxConstant - midConstant) + midConstant : (1 - scorSkill) * (midConstant - minConstant) + minConstant);
	rand = rand_kumaraswamy(a, b, skew);

	return rand;
}

double XkorSport::transformNumber(double x, QString index) const
{
	// store the QMap in a local variable to ensure its lifetime and avoid segfault
	const QMap<double, double> &dataMap = m_dataPoints.value(index);

	QMap<double, double>::const_iterator a, b;
	if(dataMap.size() <= 1) // we can’t deal with only one value
	{
		qDebug() << "dataMap not big enough for index" << index << "in XkorSport::transformNumber(double, QString)";
		return -1;
	}
	for(QMap<double, double>::const_iterator j = dataMap.begin(); j != dataMap.end(); ++j)
	{
		QMap<double, double>::const_iterator next = std::next(j);
		if(next == dataMap.end())
		{
			// b is j; a is the point before j
			a = std::prev(j);
			b = j;
			break;
		}
		else if(x < next.key())
		{
			a = j;
			b = std::next(j);
			break;
		}
	}
	return a.value() + (b.value() - a.value()) / (b.key() - a.key()) * (x - a.key());
}
