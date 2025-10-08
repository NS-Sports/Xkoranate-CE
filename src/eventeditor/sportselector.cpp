#include "eventeditor/sportselector.h"
#include <QLabel>

#include "sportmodel.h"
#include "xml/xmlindex.h"
#include "xml/xmlsportreader.h"
#include "paradigms/paradigmfactory.h"

XkorSportSelector::XkorSportSelector(QWidget * parent) : QGroupBox(parent)
{
    paradigmOptionsWidget = 0;
    selectionModel = 0;
    isLoading = false;

    sportView = new QTreeView;
    sportView->setHeaderHidden(true);

    QFont headingFont;
    headingFont.setWeight(QFont::Bold);

    layout = new QGridLayout(this);
    QLabel * label = new QLabel(tr("Select sport"));
    label->setFont(headingFont);
    layout->addWidget(label, 0, 0, Qt::AlignCenter);
    layout->addWidget(sportView, 1, 0);
    layout->setContentsMargins(0, 0, 0, 0);
}

void XkorSportSelector::setParadigmOptions(QHash<QString, QVariant> paradigmOptions)
{
    currentParadigmOptions = paradigmOptions;
    if(paradigmOptionsWidget)
        paradigmOptionsWidget->setOptions(paradigmOptions);
}

void XkorSportSelector::setSelectedSport(QString fullName)
{
    QModelIndex index = static_cast<XkorSportModel *>(sportView->model())->findSport(fullName);
    setSelectedSport(index);
}

void XkorSportSelector::setSelectedSport(QModelIndex index)
{
    if(index.isValid())
    {
        sportView->scrollTo(index);
        selectionModel->select(index, QItemSelectionModel::ClearAndSelect);
        if(index.parent() != QModelIndex())
        {
            sportView->setExpanded(index.parent(), true);
            if(index.parent().parent() != QModelIndex())
                sportView->setExpanded(index.parent().parent(), true);
        }
    }
    else
        sportView->clearSelection();
    updateSport();
}

XkorSport XkorSportSelector::sport()
{
    XkorSport rval;
    if(selectionModel)
    {
        QModelIndexList l  = selectionModel->selectedRows();
        for(QModelIndexList::iterator i = l.begin(); i != l.end(); ++i)
            if(i->data(SPORT_NAME) != QVariant())
                rval = i->data(SPORT_DATA).value<XkorSport>();
    }
    else
        rval = XkorSport();
    return rval;
}

void XkorSportSelector::updateParadigmOptions(QHash<QString, QVariant> paradigmOptions)
{
    currentParadigmOptions = paradigmOptions;
    emit paradigmOptionsChanged(paradigmOptions);
}

void XkorSportSelector::updateSport()
{
    XkorSport s = sport();
    if(!isLoading)
        emit sportChanged(s);

    if(paradigmOptionsWidget)
        layout->removeWidget(paradigmOptionsWidget);
    delete paradigmOptionsWidget;
    paradigmOptionsWidget = 0;

    // check whether the paradigm has an options dialog
    XkorAbstractParadigm * p = XkorParadigmFactory::newParadigm(s.paradigm());

    if(s.name() != "" && p->hasOptionsWidget())
    {
        paradigmOptionsWidget = p->newOptionsWidget(currentParadigmOptions);
        layout->addWidget(paradigmOptionsWidget);
        paradigmOptionsWidget->show();
        connect(paradigmOptionsWidget, &XkorAbstractOptionsWidget::optionsChanged, this, &XkorSportSelector::updateParadigmOptions);
    }
}

void XkorSportSelector::updateSportList(const QString &sportPath)
{
    isLoading = true;
    try
    {
        QString sportName = sport().name();

        // create a model
        XkorSportModel * model = new XkorSportModel;
        sportView->setModel(model);

        delete selectionModel;
        selectionModel = new QItemSelectionModel(model);
        sportView->setSelectionModel(selectionModel);
        connect(selectionModel, &QItemSelectionModel::selectionChanged, this, &XkorSportSelector::updateSport);

        // load the sports
        XkorXmlIndex x;
        x.traverse(sportPath);

        std::map<QString, QString> s = x.getAllFiles();
        for(std::map<QString, QString>::iterator i = s.begin(); i != s.end(); ++i)
        {
            XkorXmlSportReader r(i->second);
            model->insertSport(r.sport());
        }
        sportView->reset();

        setSelectedSport(sportName);
    }
    catch(XkorFileNotFoundException ex)
    {
        qDebug() << "caught XkorFileNotFoundException in XkorSportSelector::updateSportList for " << ex.fileType() << " file " << ex.fileName();
    }
    isLoading = false;
}
