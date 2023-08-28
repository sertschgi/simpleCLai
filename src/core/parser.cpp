#include "parser.h"

#include "../commands/dataset.h"
#include "../commands/model.h"
#include "../commands/profile.h"
#include "../commands/project.h"
#include "../commands/frameworks.h"

#include "../utils/errors.h"

#include <QDebug>
#include <QCommandLineParser>
#include <QCoreApplication>

void clparser::parseArgs
    (
    const QCoreApplication& app
    )
{
    QCommandLineParser parser;
    parser.setApplicationDescription("SimpleClAi parser");
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addPositionalArgument("command", "The command to execute.");

    parser.parse(app.arguments());

    QStringList args = parser.positionalArguments();
    QString command = args.isEmpty() ? QString() : args.first();

    if (command == "create")
    {
        parser.clearPositionalArguments();
        parser.addPositionalArgument("create", "Create a project, profile, dataset or model.", "create ...");

        parser.parse(app.arguments());

        args = parser.positionalArguments();
        command = args.isEmpty() ? QString() : args[1];

        if (command == "dataset")
        {
            parser.clearPositionalArguments();
            parser.addPositionalArgument("dataset", "Create a dataset.", "create dataset ...");

            QCommandLineOption datasetNameOption({"n", "name"}, "Specify the name of your dataset.", "name");
            QCommandLineOption datasetLabelOption({"l", "labels"}, "Specify the path of your labelmap.pbtxt", "labels");
            QCommandLineOption datsetLabelPathOption({"a", "labels_path"}, "Specify the label path.", "label_path");
            QCommandLineOption datasetImagePathOption({"m", "images_path"}, "Specify the images path.", "image_path");

            QList<QCommandLineOption> optionsList;
            optionsList << datasetNameOption << datasetLabelOption << datsetLabelPathOption << datasetImagePathOption;

            parser.addOptions(optionsList);

            parser.process(app);

            clparser::_checkRequiredOptions(parser, optionsList);

            qInfo() << "\033[32m[INFO]: Creating dataset...\033[0m";

            try
            {
            dataset::createDataset
                (
                parser.value(datasetNameOption),
                parser.value(datasetLabelOption),
                parser.value(datasetImagePathOption),
                parser.value(datsetLabelPathOption)
                );
            }
            catch (const error::GeneralError& Error)
            {
            qFatal() << Error.what();
            }
        }

        else if (command == "profile")
        {
            parser.clearPositionalArguments();
            parser.addPositionalArgument("profile", "Create a profile.", "create profile ...");

            QCommandLineOption profileNameOption({"n", "name"}, "Specify the name of your profile.", "name");
            QCommandLineOption profileFrameworkOption({"f", "framework"}, "Specify the framework you want to use.", "framework");
            QCommandLineOption profileScopeOption({"s", "scope"}, "Specify which field of machine lerning you want to use.", "scope");

            QList<QCommandLineOption> optionsList;
            optionsList << profileNameOption << profileFrameworkOption << profileScopeOption;

            parser.addOptions(optionsList);

            parser.process(app.arguments());

            clparser::_checkRequiredOptions(parser, optionsList);

            qInfo() << "\033[32m[INFO]: Creating profile...\033[0m";

            try
            {
            profile::createProfile
                (
                parser.value(profileNameOption),
                parser.value(profileFrameworkOption),
                parser.value(profileScopeOption)
                );
            }
            catch (const error::GeneralError& Error)
            {
            qFatal() << Error.what();
            }
        }

        else if (command == "project")
        {
            parser.clearPositionalArguments();
            parser.addPositionalArgument("project", "Create a project.", "create project ...");

            QCommandLineOption projectNameOption({"n", "name"}, "Specify the name of your project.", "name");
            QCommandLineOption projectProfileOption({"p", "profile"}, "Specify the profile you want to use for your project.", "profile");
            QCommandLineOption projectDatasetOption({"d", "dataset"}, "Specify the dataset you want to use.", "dataset");

            QList<QCommandLineOption> optionsList;
            optionsList << projectNameOption << projectProfileOption << projectDatasetOption;

            parser.addOptions(optionsList);

            parser.process(app.arguments());

            clparser::_checkRequiredOptions(parser, optionsList);

            qInfo() << "\033[32m[INFO]: Creating project...\033[0m";

            try
            {
            project::createProject
                (
                parser.value(projectNameOption),
                parser.value(projectProfileOption),
                parser.value(projectDatasetOption)
                );
            }
            catch (const error::GeneralError& Error)
            {
            qFatal() << Error.what();
            }

        }

        else if (command == "model")
        {
            parser.clearPositionalArguments();
            parser.addPositionalArgument("model", "Create a model.", "create model ...");

            QCommandLineOption modelNameOption({"n", "name"}, "Specify the name of your model.", "name");
            QCommandLineOption modelProjectOption({"p", "project"}, "Specify the project you want to use for your model.", "project");
            QCommandLineOption modelModelOption({"m", "model"}, "Specify the model you want to use.", "model");

            QList<QCommandLineOption> optionsList;
            optionsList << modelNameOption << modelProjectOption << modelModelOption;

            parser.addOptions(optionsList);

            parser.process(app.arguments());

            clparser::_checkRequiredOptions(parser, optionsList);

            qInfo() << "\033[32m[INFO]: Creating model...\033[0m";

            try
            {
            model::createModel
                (
                parser.value(modelNameOption),
                parser.value(modelProjectOption),
                parser.value(modelModelOption)
                );
            }
            catch (const error::GeneralError& Error)
            {
            qFatal() << Error.what();
            }
        }

        else
        {
            parser.process(app);
            parser.showHelp(1);
        }
    }

    else if (command == "train")
    {
        parser.clearPositionalArguments();
        parser.addPositionalArgument("train", "Train your model.", "train ...");

        QCommandLineOption trainModelOption({"m", "model"}, "Specify the name of the model.", "name");
        QCommandLineOption trainProjectOption({"p", "project"}, "Specify the name of the project.", "project");

        QList<QCommandLineOption> optionsList;
        optionsList << trainModelOption << trainProjectOption;

        parser.addOptions(optionsList);

        parser.process(app.arguments());

        clparser::_checkRequiredOptions(parser, optionsList);

        qInfo() << "\033[32m[INFO]: Initializing training...\033[0m";

        // try
        // {
        model::trainModel
            (
            parser.value(trainModelOption),
            parser.value(trainProjectOption)
            );
        // }
        // catch (const error::GeneralError& Error) <-- todo: implement this
        // {
        // qFatal() << Error.what();
        // }
    }

    else if (command == "list")
    {
        parser.clearPositionalArguments();
        parser.addPositionalArgument("list", "List your projects, profiles or datasets.", "list [...]");

        QCommandLineOption listDatasetsOption({"d", "datasets"}, "List the datasets.");
        QCommandLineOption listProfilesOption({"r", "profiles"}, "List the profiles.");
        QCommandLineOption listProjectsOption({"p", "projects"}, "List the projects.");
        QCommandLineOption listModelsOption({"m", "models"}, "List the models of a framework and scope.");
        QCommandLineOption listFrameworksOption({"f", "frameworks"}, "List the frameworks.");

        QList<QCommandLineOption> optionsList;
        optionsList << listDatasetsOption << listProfilesOption << listProjectsOption << listModelsOption << listFrameworksOption;

        parser.addOptions(optionsList);

        parser.process(app.arguments());

        const QStringList& posArgs = parser.positionalArguments();

        if (parser.isSet(listDatasetsOption))
        {
            dataset::list();
        }

        if (parser.isSet(listProfilesOption))
        {
            profile::list();
        }

        if (parser.isSet(listFrameworksOption))
        {
            frameworks::list();
        }

        if (parser.isSet(listModelsOption) && posArgs.size() > 2)
        {
            model::list(posArgs[1], posArgs[2]);
        }

        if (! parser.isSet(listDatasetsOption) ||
            ! parser.isSet(listProfilesOption) ||
            ! parser.isSet(listFrameworksOption) ||
            ! parser.isSet(listModelsOption))
        {
            project::list();
        }
    }

    else
    {
        parser.process(app);
        parser.showHelp(1);
    }
}

void clparser::_checkRequiredOptions
    (
    QCommandLineParser& parser,
    QList<QCommandLineOption>& optionsList
    )
{
    for (QCommandLineOption& option : optionsList)
    {
        if (!parser.isSet(option))
        {
            qCritical() << "\033[33m[ERROR] <CRITICAL>: Option Required!\033[0m";
            parser.showHelp(1);
        }
    }
}
