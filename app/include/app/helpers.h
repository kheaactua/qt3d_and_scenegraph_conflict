#include <QString>
#include <QUrl>

namespace helpers
{

/**
 * In QML, paths are prefixed with 'qrc:/', and stuff works well.  I can't
 * figure how to do this on the C++ side.  QUrl seems not to be able to convert
 * qrc schemes to files.  This function does that transformation for us.
 *
 * This is pretty much copied from /opt/Qt5.8.0/5.8/Src/qt3d/src/render/io/qurlhelper.cpp
 */
auto urlToLocalFileOrQrc(const QUrl& url) -> QString;

} // /helpers

/* vim: set ts=4 sw=4 sts=4 expandtab ffs=unix,dos : */
