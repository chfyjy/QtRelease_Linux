#include "procreader.h"

const QString aimroot = "/proc";
const QString libspath = "maps";
const QString elfname = "cmdline";

QString getELFNameByCMDLINE(const QString& cmdline)
{
    if(cmdline == "" || !cmdline.contains("/"))
        return  "";
    return cmdline.split("/").constLast();
}

QString getELFPathByCMDLINE(const QString& cmdline)
{
    if(cmdline == "" || !cmdline.contains("/"))
        return  "";
    int lastlength =  cmdline.split("/").constLast().length();
    int length = cmdline.length();
    return  cmdline.left(length - lastlength);
}

//true name false path
QString getELFNameByIDStr(const QString& id, bool flag)
{
    QString elfnamepath = QString("%1/%2/%3").arg(aimroot).arg(id).arg(elfname);
    QFile file(elfnamepath);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return "";

    QString cmdline = file.readLine();
    file.close();

    if(flag)
        return getELFNameByCMDLINE(cmdline);
    else
        return getELFPathByCMDLINE(cmdline);
}


QString getELFPath(const QString& elfname)
{
    QDir procdir = QDir(aimroot);
    bool isnum;
    foreach(QFileInfo info, procdir.entryInfoList())
    {
        if(info.isDir())
        {
            isnum = false;
            info.fileName().toInt(&isnum);
            if(!isnum)
                continue;
            else
            {
                if(getELFNameByIDStr(info.fileName(), true) == elfname)
                    return  getELFNameByIDStr(info.fileName(), false);
            }
        }
    }
    return  "";
}

QString getIDByELFName(const QString& elfname)
{
    QDir procdir = QDir(aimroot);
    bool isnum;
    foreach(QFileInfo info, procdir.entryInfoList())
    {
        if(info.isDir())
        {
            isnum = false;
            info.fileName().toInt(&isnum);
            if(!isnum)
                continue;
            else
            {
                if(getELFNameByIDStr(info.fileName(), true) == elfname)
                    return  info.fileName();
            }
        }
    }
    return  "";
}

QString getLibPath(const QString& map)
{
    QString pattern = "/(.*)";
    QRegExp regexp(pattern);
    int pos = 0;
    pos = regexp.indexIn(map, pos);
    return regexp.cap(1);
}

QStringList Maps2Libs(const QStringList& maps)
{
    QStringList libs;
    foreach(QString str, maps)
    {
        libs.prepend(QString("/%1").arg(getLibPath(str)));
    }

    return  libs;
}

QStringList clearLibs(const QStringList& libs)
{
    QStringList clibs;

    foreach(QString str, libs)
    {
        if(str.toLower().contains("so") && !clibs.contains(str))
            clibs.append(str);
    }

    return  clibs;
}

QStringList getDependsByIDStr(const QString& id)
{
    QStringList libs, maplist;
    QString elflibmap = QString("%1/%2/%3").arg(aimroot).arg(id).arg(libspath);
    QFile file(elflibmap);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return libs;
    QString maps = file.readAll();
    file.close();

    libs = maps.split("\n");
    foreach(QString str, libs)
    {
        if(str.contains("/"))
        {
            maplist.append(str);
        }
    }

    libs.clear();
    libs = Maps2Libs(maplist);
    libs = clearLibs(libs);

    return libs;
}

QStringList qtfirst(const QStringList& libs)
{
    QStringList qtfirst;

    foreach(QString str, libs)
    {
        if(str.toLower().contains("qt"))
        qtfirst.append(str);
    }

    foreach(QString str, libs)
    {
        if(!qtfirst.contains(str))
            qtfirst.append(str);
    }

    return qtfirst;
}
