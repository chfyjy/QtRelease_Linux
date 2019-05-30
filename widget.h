#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QLayout>
#include <QIcon>
#include <QPixmap>
#include <QMessageBox>
#include <QFileDialog>
#include <QMap>
#include <QCheckBox>
#include <QStandardItemModel>
#include <QListView>
#include <QThreadPool>
#include <QThread>
#include <QRunnable>
#include <QApplication>
#include <QEventLoop>

#include "procreader.h"

namespace Ui {
class Widget;
}

class CopyTask : public QObject, public QRunnable
{
    Q_OBJECT

public:
    CopyTask(const QString& a, const QString& b)
    {
        oldfilename = a;
        newfilename = b;
    }
    ~CopyTask()
    {}
protected:
    void run()
    {
        QFile::copy(oldfilename,newfilename);
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }

signals:
private:
    QString oldfilename,newfilename;
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();
private slots:
    void analyzeClicked();
    void startCopyClicked();
    void qtcboxToggled(bool toggled);
    void qtfirstcboxToggled(bool toggled);

private:
    void initControls(void);
    void initLibView(void);
    void getNeedLibrarys(void);
    void createNeededDir(void);
    void createBash(void);


private:
    Ui::Widget *ui;
    QThreadPool *copytPool;
    CopyTask *runcopy;
    QLineEdit *elfnameEdit, *viewEdit, *logoEdit;
    QPushButton *analyze, *startCopy;
    QCheckBox *qtcbox, *qtfirstcbox;
    QListView *qtlibview;
    QStringList librarys, librarys_qtfirst;
    QStandardItemModel *qtlibviewmodel;
    QStringList libneed, libneeddirs,libcopyto;
    QLabel *infol, *logol;
    QString elfpath;
};

#endif // WIDGET_H
