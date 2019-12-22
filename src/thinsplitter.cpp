#include "thinsplitter.h"

#include <QPainter>

void XkorThinSplitterHandle::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.setPen(palette().dark().color());
	painter.drawLine(0, 0, 0, height());
}
