#include "application.h"
#include "interface.h"

int main(int argc, char *argv[])
{
    Application a(argc, argv);
    Q_INIT_RESOURCE(lang);
    a.loadTranslations(":/lang");

    Interface w;
    w.show();

    return a.exec();
}
