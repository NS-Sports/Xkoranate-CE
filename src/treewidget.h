#ifndef TREEWIDGET_H
#define TREEWIDGET_H

#include <QTreeWidget>

#include <QTreeWidget>
#include <QWidget>

class XkorTreeWidget : public QTreeWidget
{
	public:
		XkorTreeWidget(QWidget * parent = 0) : QTreeWidget(parent) {;;;}
		QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers);
};

#endif
