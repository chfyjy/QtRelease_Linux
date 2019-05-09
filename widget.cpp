#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    setWindowTitle("QtRelease");
    setMinimumWidth(800);
    setMinimumHeight(600);
    initControls();
    copytPool = new QThreadPool(this);
    copytPool->setMaxThreadCount(5);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::qtcboxToggled(bool toggled)
{
    if(toggled)
    {
        for(int i = 0; i < qtlibviewmodel->rowCount(); i++)
            qtlibviewmodel->item(i)->setCheckState(Qt::Checked);

    }
    else
    {
        for(int i = 0; i < qtlibviewmodel->rowCount(); i++)
            qtlibviewmodel->item(i)->setCheckState(Qt::Unchecked);
    }
}

void Widget::qtfirstcboxToggled(bool toggled)
{
    Q_UNUSED(toggled);
    initLibView();
}

void Widget::analyzeClicked()
{
    infol->setText("");
    exenameEdit->setEnabled(false);
    if(exenameEdit->text() == "")
    {
        QMessageBox::information(this, "Tip", "Enter \"xxx\" to lineedit, and run it(no full path)");
        exenameEdit->setText("");
        exenameEdit->setEnabled(true);
        return;
    }
    librarys = getDependsByIDStr(getIDByELFName(exenameEdit->text()));
    if(librarys.isEmpty())
    {
        QMessageBox::information(this, "Tip", "Enter \"xxx\" to lineedit, and run it(no full path)");
        exenameEdit->setText("");
        exenameEdit->setEnabled(true);
        return;
    }
    librarys_qtfirst = qtfirst(librarys);
    initLibView();
    infol->setText("Geted");
    exenameEdit->setDisabled(false);
}

void Widget::getNeedLibrarys(void)
{
    libneed.clear();
    elfpath = getELFPath(exenameEdit->text());
    for(int i = 0; i < qtlibviewmodel->rowCount(); i++)
    {
        if(qtlibviewmodel->item(i, 0)->checkState() == Qt::Checked)
            libneed << qtlibviewmodel->index(i,0).data().toString();
    }
    QString tmp, tmp_dir;
    foreach(QString str, libneed)
    {
        if(str.contains("/plugins/"))
        {
            tmp = elfpath + str.split("/plugins/").constLast();
            tmp_dir = str.split("/plugins/").constLast();
            tmp_dir = tmp_dir.split("/").constFirst();
            libneeddirs.append(elfpath+tmp_dir);
        }
        else
            tmp = elfpath + str.split("/").constLast();
        libcopyto.append(tmp);
    }

}

void Widget::createNeededDir(void)
{
    infol->setText("start create some dir for need");
    for(int i =0; i < libneeddirs.count(); i++)
    {
        QDir dir(libneeddirs.at(i));
        if(dir.exists())
          continue;
        else
           dir.mkdir(libneeddirs.at(i));
    }
    infol->setText("created dir");
}

void Widget::startCopyClicked()
{
    if(librarys.isEmpty())
    {
            QMessageBox::information(this, "Tip", "Enter \"xxx\" to lineedit, and run it(no full path)");
            exenameEdit->setEnabled(true);
            return;
    }
    getNeedLibrarys();
    createNeededDir();

    for(int i= 0; i < libneed.count(); i++)
        copytPool->start(new CopyTask(libneed.at(i), libcopyto.at(i)));

    copytPool->waitForDone();
    createBash();
    infol->setText("copy success");
}


QString getPlugins(const QString& libpath)
{
    QString pattern = "/plugins/(.*)/";
    QRegExp regexp(pattern);
    int pos = 0;
    pos = regexp.indexIn(libpath, pos);
    return regexp.cap(1);
}

void Widget::createBash(void)
{
    QStringList bashdata, ldconf;
    QString tmpdata, strdata;
    int needlength, length;
    bashdata.append("#!/bin/bash\n");

    QString installpath;
    foreach(QString str, libneed)
    {
        if(str.contains("libQt"))
        {
            installpath = str.split("/lib/").constFirst();
            break;
        }
    }
    bashdata.append(QString("mkdir -p %1/lib\n").arg(installpath));
    ldconf = getldsoconf();

    foreach(QString str, ldconf)
        bashdata.append(QString("echo \"%1/lib\n\" >> %2\n").arg(installpath).arg(str));

    foreach(QString str, libneed)
    {
        if(str.contains("/plugins/"))
        {
            strdata = getPlugins(str);
            tmpdata = QString("mkdir -p %1/plugins/%2\n").arg(installpath).arg(strdata);
            if(!bashdata.contains(tmpdata))
            {
                bashdata.append(tmpdata);
                foreach(QString str1, ldconf)
                bashdata.append(QString("echo \"%1/plugins/%2\n\" >> %3\n").arg(installpath).arg(strdata).arg(str1));

            }
        }
    }

    strdata = elfpath + '/';
    for(int i = 0; i < libneed.count(); i++)
    {
        //qDebug() << libcopyto.at(i);
        length = libcopyto.at(i).length();
        needlength = length - strdata.length();
        tmpdata = libcopyto.at(i).right(needlength+1);
        //qDebug() << tmpdata;
        tmpdata = QString("mv %1 %2\n").arg(tmpdata).arg(libneed.at(i));
        bashdata.append(tmpdata);
    }

//    QString exportstr = QString("echo \"export LD_LIBRARY_PATH=%1/lib:$LD_LIBRARY_PATH\" >> /etc/profile\n").arg(installpath);
//    bashdata.append(exportstr);
//    //bashdata.append("source ~/.bashrc");//for this not '#!/bin/sh'

    QFile bashfile(elfpath + "/install.sh");

    if(bashfile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        foreach(QString data, bashdata)
            bashfile.write(data.toLocal8Bit());

        bashfile.close();
    }
}

void Widget::initLibView(void)
{
    qtlibviewmodel->clear();
    QStringList tmplist;
    if(qtfirstcbox->isChecked())
        tmplist = librarys_qtfirst;
    else
        tmplist = librarys;
    foreach(QString str, tmplist)
    {
        QStandardItem* item = new QStandardItem(str);
        item->setCheckable( true );
        if(str.toLower().contains("qt"))
            item->setCheckState(Qt::Checked);
        else
            item->setCheckState(Qt::Unchecked);
        qtlibviewmodel->appendRow(item);
    }
}

void Widget::initControls()
{
    QLabel *exenamel = new QLabel(this);
    exenamel->setText("target");
    exenameEdit = new QLineEdit(this);
    analyze = new QPushButton(this);
    analyze->setText("get so info");
    connect(analyze, SIGNAL(clicked()), this, SLOT(analyzeClicked()));
    startCopy = new QPushButton(this);
    startCopy->setMaximumWidth(40);
    startCopy->setText("copy");
    connect(startCopy, SIGNAL(clicked()), this, SLOT(startCopyClicked()));
    QHBoxLayout *exeNamelayout = new QHBoxLayout();
    exeNamelayout->addWidget(exenamel);
    exeNamelayout->addWidget(exenameEdit);
    exeNamelayout->addWidget(analyze);
    exeNamelayout->addWidget(startCopy);

    qtfirstcbox = new  QCheckBox(this);
    qtfirstcbox->setText("Qt first");
    qtfirstcbox->setMaximumWidth(150);
    qtfirstcbox->setMinimumWidth(150);
    qtfirstcbox->setChecked(false);
    connect(qtfirstcbox, SIGNAL(toggled(bool)), this, SLOT(qtfirstcboxToggled(bool)));
    qtcbox = new QCheckBox(this);
    qtcbox->setText("select all");
    qtcbox->setMaximumWidth(150);
    qtcbox->setMinimumWidth(150);
    connect(qtcbox, SIGNAL(toggled(bool)), this, SLOT(qtcboxToggled(bool)));
    QHBoxLayout *cboxLayout = new QHBoxLayout();
    infol = new QLabel(this);
    infol->setText("");
    cboxLayout->addWidget(qtcbox);
    cboxLayout->addWidget(qtfirstcbox);
    cboxLayout->addWidget(infol);

    qtlibview = new QListView(this);
    qtlibviewmodel = new QStandardItemModel(this);
    qtlibview->setModel(qtlibviewmodel);
    qtlibview->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QVBoxLayout *viewLayout = new QVBoxLayout();
    viewLayout->addWidget(qtlibview);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addLayout(exeNamelayout);
    layout->addLayout(cboxLayout);
    layout->addLayout(viewLayout);
    setLayout(layout);
    setGeometry(100,100,400, 300);
}

