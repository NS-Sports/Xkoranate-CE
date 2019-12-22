#include "mainwindow.h"

void XkorMainWindow::closeEvent(QCloseEvent * event)
{
	if(centralWidget()->close())
		QMainWindow::closeEvent(event);
	else
		event->ignore();
}
