#include "GameController.h"
#include "../game_state/player/player.h"
#include "../network/requests/join_game_request.h"

// initialize static members
GameWindow* GameController::_gameWindow = nullptr;
ConnectionPanel* GameController::_connectionPanel = nullptr;
MainGamePanel* GameController::_mainGamePanel = nullptr;
ClientNetworkThread* GameController::_networkThread = nullptr;
player* GameController::_me = nullptr;


void GameController::init(GameWindow* gameWindow) {

    GameController::_gameWindow = gameWindow;

    // Set up main panels
    GameController::_connectionPanel = new ConnectionPanel(gameWindow);
    GameController::_mainGamePanel = new MainGamePanel(gameWindow);

    // Hide all panels
    GameController::_connectionPanel->Show(false);
    GameController::_mainGamePanel->Show(false);

    // Only show connection panel at the start of the game
    GameController::_gameWindow->showPanel(GameController::_connectionPanel);

    // Set status bar
    GameController::showStatus("Not connected");
}


void GameController::connectToServer() {

    // get values form UI input fields
    wxString inputServerAddress = GameController::_connectionPanel->getServerAddress().Trim();
    wxString inputServerPort = GameController::_connectionPanel->getServerPort().Trim();
    wxString inputPlayerName = GameController::_connectionPanel->getPlayerName().Trim();

    // check that all values were provided
    if(inputServerAddress.IsEmpty()) {
        GameController::showError("Input error", "Please provide the server's address");
        return;
    }
    if(inputServerPort.IsEmpty()) {
        GameController::showError("Input error", "Please provide the server's port number");
        return;
    }
    if(inputPlayerName.IsEmpty()) {
        GameController::showError("Input error", "Please enter your desired player name");
        return;
    }

    // convert host from wxString to std::string
    std::string host = inputServerAddress.ToStdString();

    // convert port from wxString to uint16_t
    unsigned long portAsLong;
    if(!inputServerPort.ToULong(&portAsLong) || portAsLong > 65535) {
        GameController::showError("Connection error", "Invalid port");
        return;
    }
    uint16_t port = (uint16_t) portAsLong;

    // convert player name from wxString to std::string
    std::string playerName = inputPlayerName.ToStdString();

    // start network thread
    GameController::_networkThread = new ClientNetworkThread(host, port);
    if(GameController::_networkThread->Run() != wxTHREAD_NO_ERROR) {
        GameController::showError("Connection error", "Could not create client network thread");
    }

    // send request to join game
    GameController::_me = new player(playerName);
    join_game_request request = join_game_request(GameController::_me->get_id(), GameController::_me->get_player_name());
    GameController::_networkThread->sendRequest(request);

}


void GameController::updateState(game_state* gameState) {
    GameController::_gameWindow->showPanel(GameController::_mainGamePanel);
    GameController::_mainGamePanel->buildGameState(gameState, GameController::_me);
}


wxEvtHandler* GameController::getMainThreadEventHandler() {
    return GameController::_gameWindow->GetEventHandler();
}


void GameController::showError(const std::string& title, const std::string& message) {
    wxMessageBox(message, title, wxICON_ERROR);
}


void GameController::showStatus(const std::string& message) {
    GameController::_gameWindow->setStatus(message);
}