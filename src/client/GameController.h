#ifndef TICHUUI_GAMECONTROLLER_H
#define TICHUUI_GAMECONTROLLER_H

#include "windows/GameWindow.h"
#include "panels/ConnectionPanel.h"
#include "panels/MainGamePanel.h"
#include "network/ResponseListenerThread.h"
#include "../common/game_state/game_state.h"

class GameController {

public:
    static void init(GameWindow* gameWindow);

    static void connectToServer();
    static void updateGameState(const game_state &newGameState);
    static void startGame();
    static void drawCard();
    static void fold();
    static void playCombi(const card_combination &CombiToPlay);

    static wxEvtHandler* getMainThreadEventHandler();
    static void showError(const std::string& title, const std::string& message);
    static void showStatus(const std::string& message);
    static void showNewRoundMessage(const game_state &oldGameState, const game_state &newGameState);
    static void showGameOverMessage();

    static const std::optional<player>& get_me() { return _me; }
    static const std::optional<game_state>& get_game_state() { return _currentGameState; }

private:
    static GameWindow* _gameWindow;
    static ConnectionPanel* _connectionPanel;
    static MainGamePanel* _mainGamePanel;

    static std::optional<player> _me;
    static std::optional<game_state> _currentGameState;

};


#endif //TICHUUI_GAMECONTROLLER_H
