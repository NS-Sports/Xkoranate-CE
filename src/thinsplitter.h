#ifndef THINSPLITTER_H
#define THINSPLITTER_H

#include <QSplitter>

class XkorThinSplitterHandle : public QSplitterHandle
{
	Q_OBJECT
	public:
		explicit XkorThinSplitterHandle(Qt::Orientation orientation, QSplitter * splitter) : QSplitterHandle(orientation, splitter) {;;;}

		void paintEvent(QPaintEvent *);
		QSize sizeHint() const { return QSize(1, QSplitterHandle::sizeHint().height()); }
};

class XkorThinSplitter : public QSplitter
{
	Q_OBJECT
	public:
		explicit XkorThinSplitter(QWidget * parent = 0) : QSplitter(parent) {;;;}

		QSplitterHandle * createHandle() { return new XkorThinSplitterHandle(orientation(), this); }
};

#endif // THINSPLITTER_H
