#ifndef TABLECOLUMN_H
#define TABLECOLUMN_H

#include <QString>

struct XkorTableColumn
{
	QString columnType;
	QString heading;
	unsigned int width;
	
	XkorTableColumn() {;;;}
	XkorTableColumn(QString ct, QString h, unsigned int w) : columnType(ct), heading(h), width(w) {;;;}
};

#endif
