#include "dataset.h"

#include "src/config/config.h"

#include "../utils/tools.h"
#include "../utils/errors.h"

#include <QString>
#include <QSettings>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcessEnvironment>

void dataset::createDataset
    (
        const QString& name,
        const QString& labelmapPath,
        const QString& imagesPath,
        const QString& labelsPath
        )
{
    using namespace::std;

    QJsonObject jsonDatasets = tools::getJsonObject(USER_CONFIG_PATH "/datasets.json");
    QJsonObject newDataset;

    if (jsonDatasets.contains(name))
    {
        throw error::name::DatasetNameError();
    }

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    QString datasetPath = env.value("DATASETS_PATH");

    if (datasetPath.isEmpty())
    {
        error::environment::DATASETS_PATH_Error error;

        datasetPath = DEFAULT_DATASETS_PATH;

        qInfo() << error.what() << "\033[36m"
                << "Default:" << datasetPath
                << "\033[0m";

        // set the $SA_PROFILE_PATH for debian
        qDebug() << "\033[90m[DEBUG]: Script executed with output:"
                 << tools::installProcess(APP_SCRIPTS_PATH "/set_debian_env.sh DATASETS_PATH " + datasetPath)
                 << "\033[0m";
    }

    datasetPath = datasetPath + "/" + name;

    qInfo() << "\033[32m[INFO]: Your dataset will be stored in:\033[35m" << datasetPath << "\033[0m";

    const QString newImagesPath = datasetPath + "/images";
    const QString newLabelsPath = datasetPath + "/labels";

    QSettings settings(APP_CONFIG_PATH "/config.ini", QSettings::IniFormat);

    QStringList label_formats = settings.value("dataset/supported_labeling_formats").toStringList();

    if (tools::copyFilesWithExtention(labelsPath,newLabelsPath,label_formats) == 0)
    {
        throw error::compatibility::LabelExtentionError();
    }

    QStringList img_formats = settings.value("dataset/supported_img_formats").toStringList();

    if (tools::copyFilesWithExtention(imagesPath,newImagesPath,img_formats) == 0)
    {
        throw error::compatibility::ImageExtentionError();
    }

    const QString& newLabelmapPath = datasetPath + "/annotations/labelmap.pbtxt";

    QFile::copy(labelmapPath, newLabelmapPath);

    newDataset["path"] = datasetPath;
    newDataset["images"] = newImagesPath;
    newDataset["labels"] = newLabelsPath;
    newDataset["labelmap"] = newLabelmapPath;

    jsonDatasets[name] = newDataset;

    tools::writeJson(USER_CONFIG_PATH "/datasets.json", jsonDatasets);

    qInfo() << "\033[32m[INFO]: Successfully created dataset!\033[0m";
}

void dataset::deleteDataset
    (
    const QString& name,
    bool confirmationDialog
    )
{
    QJsonObject jsonDatasets = tools::getJsonObject(USER_CONFIG_PATH "/datasets.json");

    if (!jsonDatasets.contains(name))
    {
        throw error::existence::NoSuchDatasetError();
    }

    tools::deleteFromObject(name, jsonDatasets, confirmationDialog);
}

void dataset::list()
{
    const QJsonObject& jsonDatasets = tools::getJsonObject(USER_CONFIG_PATH "/datasets.json");

    qInfo() << "\033[32m[INFO]: Deleting dataset...\033[0m";

    qInfo().noquote() << tools::list(jsonDatasets).toUtf8();
}
