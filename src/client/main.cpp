#include "Tichu.h"

int main() {

    ApplicationCreateInfo info {
        .title = "Tichu",
        .width = 1500,
        .height = 1000,
    };

    Application app(info);
    app.push_layer(std::make_shared<TichuGame>(TichuGame()));
    app.run();

    return 0;
}