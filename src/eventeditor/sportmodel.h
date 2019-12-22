#ifndef SPORTMODEL_H
#define SPORTMODEL_H

#include <QStandardItemModel>

#include "sport.h"

#define SPORT_NAME (Qt::UserRole + 93)
#define SPORT_PARADIGM (Qt::UserRole + 10)
#define SPORT_DATA (Qt::UserRole + 984)

Q_DECLARE_METATYPE(XkorSport)

class XkorSportModel : public QStandardItemModel
{
	public:
		XkorSportModel(QObject * parent = 0) : QStandardItemModel(parent) {;;;}
		QModelIndex findSport(QString discipline, QString event, QString scorinator);
		QModelIndex findSport(QString fullName);
		void insertSport(XkorSport s);
	private:
		void initItem(QStandardItem * item, XkorSport sport);
};

#endif
