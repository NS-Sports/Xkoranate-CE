#include "treewidget.h"

QModelIndex XkorTreeWidget::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
	QModelIndex index = currentIndex();
	switch(cursorAction)
	{
		case MoveNext:
			if(index.column() < columnCount() - 1)
				return model()->index(index.row(), index.column() + 1, index.parent());
			break;
		case MovePrevious:
			if(index.column() > 0)
				return model()->index(index.row(), index.column() - 1, index.parent());
			break;
		default:
			break;
	}
	// in all other cases, let Qt do the hard work
	return QTreeWidget::moveCursor(cursorAction, modifiers);
}
