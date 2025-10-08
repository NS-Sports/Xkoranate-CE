#ifndef SIGNUPLISTEDITOR_H
#define SIGNUPLISTEDITOR_H

#include <QtGui>

#include "signuplist.h"
#include "signuplisteditor/abstractathletewidget.h"
#include "sport.h"

#include <QWidget>
#include <QDoubleSpinBox>

class XkorSignupListEditor : public QWidget
{
	Q_OBJECT

	public:
		XkorSignupListEditor(QWidget * parent = 0);

		virtual XkorSignupList data() { updateData(); return m_data; }
		virtual void updateData();

	signals:
		void dataChanged();
        void itemDeleted(QUuid id);
		void signupListDirectoryChanged(QString dir);

    public slots:
		virtual void setData(XkorSignupList data);
		virtual void setSport(XkorSport sport, QHash<QString, QVariant> paradigmOptions);

	protected:
		XkorSignupList m_data;

		QVBoxLayout * layout;
		QDoubleSpinBox * minRank, * maxRank;
		XkorAbstractAthleteWidget * athletes;

		bool isLoading;

	protected slots:
		virtual void minRankChanged(double newMin);
		virtual void maxRankChanged(double newMax);
		virtual void setDataChanged() { if(!isLoading) emit dataChanged(); }
        virtual void setItemDeleted(QUuid id) { emit itemDeleted(id); }
		virtual void setSignupListDirectory(QString dir);
};

#endif
