#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCloseEvent>
#include <QMainWindow>

class XkorMainWindow : public QMainWindow
{
	protected:
		void closeEvent(QCloseEvent * event);
};

#endif // MAINWINDOW_H
