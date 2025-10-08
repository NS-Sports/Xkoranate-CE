#ifndef ABSTRACTTREEWIDGET_H
#define ABSTRACTTREEWIDGET_H

#include <QtGui>

#include "treewidget.h"

#include <QGridLayout>

#include <QTreeWidget>
#include <QAction>
#include <QGridLayout>

class XkorAbstractTreeWidget : public QTreeWidget
{
	Q_OBJECT
	
	protected:
		QAction * insertAction, * deleteAction, * moveUpAction, * moveDownAction;
		QGridLayout * layout;
		XkorTreeWidget * treeWidget;
		
		bool isInUse; // if we are editing the list, we don’t want to send a bunch of signals, and there’s no Qt signal that only emits on user edits of a tree widget
		
		virtual void initItem(QTreeWidgetItem *) {;;;}
		virtual void setupLayout(QList<QAction *> actions, bool isVertical = false);
		virtual QString insertionText() { return tr("Add item"); }
		virtual QString deletionText() { return tr("Remove items"); }
		virtual QString movingUpText() { return tr("Move item up"); }
		virtual QString movingDownText() { return tr("Move item down"); }
		
		QTreeWidgetItem * createItem();
		
	protected slots:
        virtual void deleteItems();
		void insertItem();
		void listIsEdited();
		void moveDown();
		void moveUp();
		virtual void updateButtons();
	
	public:
		XkorAbstractTreeWidget();
		void clear() { treeWidget->clear(); emit listChanged(); }
		virtual void keyPressEvent(QKeyEvent * event);
	signals:
		void listChanged();
};



#endif
