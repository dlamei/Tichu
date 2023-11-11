#include "GUI/Application.h"

int main() {

    ApplicationCreateInfo info {
        .title = "Tichu",
        .width = 1200,
        .height = 1200,
    };

    Application app(info);
    app.run();

    return 0;
}
