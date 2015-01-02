#include "application.h"
#include "interface.h"
#include <assert.h>

int main(int argc, char *argv[])
{

////////////////////////////////////////////////////////////
// NOTE: Conditionally setup plugin path to be located at
// $appPackageRoot/Plugins. Something of a hack for redistributing QT
// dylib dependencies with the app package itself.
////////////////////////////////////////////////////////////
#ifndef QT_DEBUG
    QDir dir(argv[0]); //appdir/Contents/MacOS/appname
    assert(dir.cdUp());
    assert(dir.cdUp());
    assert(dir.cd("Plugins"));
    QCoreApplication::setLibraryPaths(QStringList(dir.absolutePath()));
#endif
////////////////////////////////////////////////////////////

    Application a(argc, argv);
    Q_INIT_RESOURCE(lang);
    a.loadTranslations(":/lang");

    Interface w;
    w.show();

    return a.exec();
}
