#ifndef PROCREADER_H
#define PROCREADER_H

#include <QString>
#include <QStringList>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QDebug>
#include <QRegExp>

QString getELFPath(const QString& elfname);
QString getIDByELFName(const QString& elfname);
QStringList getDependsByIDStr(const QString& id);
QStringList qtfirst(const QStringList& libs);
QStringList getldsoconf();


#endif // PROCREADER_H
