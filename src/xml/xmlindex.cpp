#include "xml/xmlindex.h"

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include "xmlsportreader.h"

using namespace std;

void XkorXmlIndex::insert(QString filename)
{
    qDebug() << "XkorXmlIndex::insert called with filename:" << filename;
	// parse the XML file
	XkorXmlSportReader r(filename);
    if(r.error() != QString())
    {
        qDebug() << "Error reading sport XML:" << r.error();
    }
    else
    {
        qDebug() << "Successfully read sport:" << r.sport().name() << "(" << r.sport().alphabetizedName() << ")";
        // the key controls alphabetization in the sport list
        // I don’t know why the sport list can’t be sorted explicitly
        index[r.sport().alphabetizedName()] = filename;
    }
}

void XkorXmlIndex::traverse(QString dir) noexcept(false)
{
    qDebug() << "XkorXmlIndex::traverse called with dir:" << dir;
	// initialize a QDir to the desired directory
	QDir d;
	d.setPath(dir);
	d.setNameFilters(QStringList() << "*.xml");
    qDebug() << "QDir path after setPath and setNameFilters:" << d.absolutePath();
	if(!d.exists())
	{
		string err = "Directory ‘";
		err += d.path().toStdString();
		err += "’ not found by XkorXmlIndex::traverse(QString)";
		throw XkorFileNotFoundException(err, QObject::tr("sport directory"), d.path());
	}
	QDirIterator i(d, QDirIterator::Subdirectories);
	// iterate
	while(i.hasNext())
	{
		QString f = i.next();
        qDebug() << "File found by QDirIterator:" << f;
		if(i.fileInfo().isFile())
			this->insert(f);
	}
}

QString XkorXmlIndex::lookup(QString name) noexcept(false)
{
	map<QString, QString>::iterator i = index.find(name);
	if(i == index.end())
	{
		string err = "Unknown object ‘";
		err += name.toStdString();
		err += "’ in XkorXmlIndex::lookup(QString)";
		throw XkorSearchFailedException(err);
	}
	return (i->second);
}

std::map<QString, QString> XkorXmlIndex::getAllFiles()
{
	return index;
}
