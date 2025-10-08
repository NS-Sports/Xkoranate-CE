#ifndef TABLEGENERATOR_H
#define TABLEGENERATOR_H

#include <QtGui>

#include "tablegenerator/sortcriteriawidget.h"
#include "tablegenerator/table.h"

#include <QPlainTextEdit>
#include <QComboBox>

#include <QPlainTextEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QFileDialog>

class XkorTableGenerator : public QWidget
{
	Q_OBJECT

	private:
		QPlainTextEdit * matches, * table;
		QSpinBox * pointsForWin, * pointsForDraw, * pointsForLoss, * columnWidth;
		XkorSortCriteriaWidget * scw;
		QCheckBox * showDraws, * showResultsGrid;
		QComboBox * goalName;
		XkorTable t;
		std::vector<XkorTableMatch> matchesList;
		QStringList teamsList;
		QString currentFileName;
		bool fileModified, matchesModified;

		QFileDialog * dialog, * importDialog;

		public:
		bool okayToLoad();
		int showUnsavedDialog();

	public:
		XkorTableGenerator(QWidget * parent = 0);
		~XkorTableGenerator();
	signals:
		void fileChanged(QString filename); // loaded a new file
		void fileEdited(bool isModified); // changed the current file
		void resultImportDirectoryChanged(QString dir);
		void tableDirectoryChanged(QString dir);
	public slots:
		void importResults();
		void importResults(QString filename);
		void openFile();
		void openFile(QString filename);
		void reset();
		void saveFile(QString filename = QString());
		void saveFileAs();
	protected:
		void closeEvent(QCloseEvent * event);
	protected slots:
		void generateMatches();
		void generateTable();
		std::vector<XkorTableColumn> generateTableColumns();
		void setFileModified(bool isEdited = true);
		void setMatchesModified();
		void updateTable();
};

#endif
