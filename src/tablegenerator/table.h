#ifndef TABLE_H
#define TABLE_H

#include <QString>
#include <vector>

#include "exceptions.h"
#include "tablegenerator/tablecolumn.h"
#include "tablegenerator/tablerow.h"
#include "tablegenerator/tablesorter.h"

class XkorTable
{
	private:
		QString tableName;
		std::vector<XkorTableColumn> columns;
		std::vector<XkorTableMatch> matches;
		QHash<QString, XkorTableRow> unsortedData;
		std::vector<std::vector<XkorTableRow> > data;
		std::vector<QString> sortCriteria;
		XkorTableSorter sorter;
		double pointsForWin, pointsForDraw, pointsForLoss;
		int columnWidth;
		bool showDraws, showResultsGrid;
		QString goalName;
		
		void collapse(std::vector<std::vector<XkorTableRow> > & target, const std::vector<std::vector<XkorTableRow> > & source);
		QHash<QString, XkorTableRow>::iterator findTeam(QString name);
		template<typename Comparator> std::vector<std::vector<XkorTableRow> > sort(std::vector<XkorTableRow> rows, Comparator gr, Comparator eq);
		std::vector<std::vector<XkorTableRow> > sortTable(const std::vector<XkorTableRow> & rows);
		void updateMatches();
		
		double getPoints(const XkorTableRow & a);
		void addMatchToData(XkorTableMatch m);
		
	public:
		XkorTable() {;;;}
		void generate();
		std::vector<XkorTableColumn> getColumns() { return columns; }
		std::vector<XkorTableMatch> getMatches() { return matches; }
		int getColumnWidth() { return columnWidth; }
		QString getGoalName() { return goalName; }
		double getPointsForWin() { return pointsForWin; }
		double getPointsForDraw() { return pointsForDraw; }
		double getPointsForLoss() { return pointsForLoss; }
		bool getShowDraws() { return showDraws; }
		bool getShowResultsGrid() { return showResultsGrid; }
		std::vector<QString> getSortCriteria() { return sortCriteria; }
		void insertMatch(QString t1, QString t2, double score1, double score2) { insertMatch(XkorTableMatch(t1, t2, score1, score2)); }
		void insertMatch(XkorTableMatch m);
		void setColumns(std::vector<XkorTableColumn> c) { columns = c; }
		void setColumnWidth(int width) { columnWidth = width; }
		void setGoalName(QString name) { goalName = name; }
		void setMatches(std::vector<XkorTableMatch> m) { matches = m; updateMatches(); }
		void setPointsForWin(double pts) { pointsForWin = pts; sorter.setPointsForWin(pts); }
		void setPointsForDraw(double pts) { pointsForDraw = pts; sorter.setPointsForDraw(pts); }
		void setPointsForLoss(double pts) { pointsForLoss = pts; sorter.setPointsForLoss(pts); }
		void setShowDraws(bool value) { showDraws = value; }
		void setShowResultsGrid(bool value) { showResultsGrid = value; }
		void setSortCriteria(std::vector<QString> sc) { sortCriteria = sc; }
		QString toText();
};

#endif
