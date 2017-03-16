#include "app/helpers.h"


namespace helpers
{

auto urlToLocalFileOrQrc(const QUrl& url) -> QString
{
    const QString scheme(url.scheme().toLower());
    if (QLatin1String("qrc") == scheme)
    {
        if (url.authority().isEmpty())
        {
            return QLatin1Char(':') + url.path();
        }
        return QString();
    }

#if defined(Q_OS_ANDROID)
    if (scheme == QLatin1String("assets")) {
        if (url.authority().isEmpty())
            return url.toString();
        return QString();
    }
#endif

    return url.toLocalFile();
}

} // /helpers

/* vim: set ts=4 sw=4 sts=4 expandtab ffs=unix,dos : */
