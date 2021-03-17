#ifndef LAMAUI_GAMECONTROLLER_H
#define LAMAUI_GAMECONTROLLER_H

#include "windows/GameWindow.h"
#include "panels/ConnectionPanel.h"
#include "panels/MainGamePanel.h"
#include "network/ClientNetworkThread.h"
#include "../game_state/game_state.h"


class GameController {

public:
    static void init(GameWindow* gameWindow);
    static void connectToServer();
    static void updateGameState(game_state* gameState);
    static void startGame();

    static wxEvtHandler* getMainThreadEventHandler();
    static void showError(const std::string& title, const std::string& message);
    static void showStatus(const std::string& message);

private:
    static GameWindow* _gameWindow;
    static ConnectionPanel* _connectionPanel;
    static MainGamePanel* _mainGamePanel;

    static ClientNetworkThread* _networkThread;

    static player* _me;
    static game_state* _currentGameState;

};


#endif //LAMAUI_GAMECONTROLLER_H
