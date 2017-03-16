#include <future>
#include <memory>
#include <tuple>

#include <QGuiApplication>
#include <QtQuick/QQuickView>
#include <QQmlEngine>
#include <QQmlContext>
#include <QDebug>
#include <QComponent>
#include <QQmlComponent>
#include <QEntity>
#include <QQuickItem>
#include <QCommandLineParser>
#include <QFileInfo>

#include "app/qobjs/GroundModel.h"
#include "app/qobjs/Settings.h"
#include "app/qobjs/Viewport.h"

#include "app/Memory.hpp"

namespace {

    /**
     * Parse command line options.  Return results as a tuple.  Exit process on
     * failure.
     */
    auto parseCLI(QGuiApplication& app) -> const std::tuple<bool, bool>
    {

        QCommandLineParser parser;
        parser.setApplicationDescription("Point viewer");
        parser.addHelpOption();
        parser.addVersionOption();

        QCommandLineOption deactivate_points_option(
            "p", QCoreApplication::translate("main", "(Debugging) Disable the groun points displayed with OpenGL"));
        parser.addOption(deactivate_points_option);

        QCommandLineOption not_full_screen_option(
            "n", QCoreApplication::translate("main", "(Debugging) Load app in a window rather than full screen"));
        parser.addOption(not_full_screen_option);

        // Process the actual command line arguments given by the user
        parser.process(app);

        return std::make_tuple(
            parser.isSet(not_full_screen_option),
            parser.isSet(deactivate_points_option)
        );
    }

}

auto main(int argc, char *argv[]) -> int
{

    // Instantiate our application.  This needs to happen before most other Qt
    // stuff (especially anything dealing with application paths)
    QGuiApplication app(argc, argv);

    // Recall, this function will exit the program upon failure.
    auto opts = parseCLI(app);
    auto not_full_screen   = std::get<0>(opts);
    auto deactivate_points = std::get<1>(opts);


    // ######################################
    // Start setting up program
    // ######################################


    // Register our custom types
    qmlRegisterType<Viewport>("matt", 1, 0, "Viewport");
    qmlRegisterType<GroundModel>("matt", 1, 0, "Ground");

    // and C++ files for all configuration options.
    auto settings = App::Memory::make_unique<Settings>();
    settings->setValue("deactivate_points", deactivate_points);

    // Discussion about QQuickView vs QQmlEngine:
    // http://doc.qt.io/qt-5/qtquick-deployment.html
    QQuickView view;

    // Give QML the settings object
    view.rootContext()->setContextProperty("settings", settings.get());

    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:///qml/main.qml"));

    // Grab the root object
    auto root = view.rootObject();

    auto gm = root->findChild<GroundModel*>("ground");
    if (!gm)
    {
        qWarning("Could not get ground model");
    }

    if (not_full_screen) {
        // This is only used in development.
        view.resize(1200, 800);
        view.show();
    }
    else
    {
        view.showFullScreen();
    }

    auto result = app.exec();
    return result;
}

#ifdef _WIN32
auto WINAPI WinMain(HINSTANCE hinst, HINSTANCE, LPSTR argv, int argc) -> int
{
    return main(argc, &argv);
}
#endif

/* vim: set ts=4 sw=4 sts=4 expandtab ffs=unix,dos : */
