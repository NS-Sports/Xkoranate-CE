#ifndef CONFIGEDITOR_H
#define CONFIGEDITOR_H

#include <QtWidgets>

#include "rplist.h"
#include "rpeditor/abstractrpbonuswidget.h"

class XkorRPEditor : public QWidget
{
	Q_OBJECT

	public:
		XkorRPEditor(QWidget * parent = 0);

		void updateData();
	signals:
		void dataChanged();
	public slots:
		void setData(XkorRPList * data);
	
	private:
		XkorRPList * m_data;

		QRadioButton * m_useParticipantBonus;
		QRadioButton * m_useTeamBonus;
		QDoubleSpinBox * m_bonusEffect;
		QComboBox * m_rpCalcType;
		XkorAbstractRPBonusWidget * m_rpBonus;
		QHBoxLayout * m_bonusLayout;
		QString m_currentRPCalcType;
		QString m_currentFileName;
		bool isLoading;
	private slots:
		void setDataChanged() { if(!isLoading) emit dataChanged(); }
		void updateRPBonusWidget();
};

#endif
