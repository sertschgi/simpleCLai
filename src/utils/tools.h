#ifndef TOOLS_H
#define TOOLS_H

#include <string>
#include <iostream>
#include <exception>

#include <QString>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

namespace tools
{
class NoSuchProfileError : public std::exception {
public:
    const char* what() const noexcept override;
};

QJsonObject getJsonObject
    (
    const QString& filename
    );

void writeJson
    (
    const QString& filename,
    QJsonObject jsonObject
    );

int copyFilesWithExtention
    (
    const QString& sourceDir,
    const QString& destDir,
    const QStringList& extensions
    );

void updateProgressBar
    (
    int progress,
    int total
    );

const QString& installProcess
    (
    const QJsonObject& object
    );

const QString& interpretPath
    (
        const QString& path,
        const QMap<QString, QString>& replacements
    );
}

#endif // TOOLS_H