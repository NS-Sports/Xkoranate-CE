#include <QTextCodec>

#include "application.h"

int main(int argc, char * * argv)
{
    //QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    //QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
	
	XkorApplication app(argc, argv);
    app.loadSports();
	
	return app.exec();
}
