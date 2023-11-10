#include "GameController.h"
#include "../common/network/client_msg.h"
#include "network/ClientNetworkManager.h"


// initialize static members
GameWindow* GameController::_gameWindow = nullptr;
ConnectionPanel* GameController::_connectionPanel = nullptr;
MainGamePanel* GameController::_mainGamePanel = nullptr;

std::optional<player> GameController::_me = {};
std::optional<game_state> GameController::_currentGameState = {};
//player* GameController::_me = nullptr;
//game_state GameController::_currentGameState = nullptr;



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
    auto port = (uint16_t) portAsLong;

    // convert player name from wxString to std::string
    std::string playerName = inputPlayerName.ToStdString();

    // connect to network
    ClientNetworkManager::init(host, port);

    // send request to join game
    GameController::_me = player(playerName);
    auto req = join_game_req{GameController::_me.value().get_player_name()};
    auto request = client_msg(GameController::_me.value().get_id(), UUID(), req);
    ClientNetworkManager::sendRequest(request);

}


void GameController::updateGameState(const game_state &newGameState) {

    // the existing game state is now old
    auto oldGameState = GameController::_currentGameState;

    //current Game state is now the new one
    GameController::_currentGameState = newGameState;
    
    // save the new game state as our current game state
    if (oldGameState) {
        auto old = oldGameState.value();

        // check if a new round started, and display message accordingly
        if (old.get_round_number() > 0 && old.get_round_number() < newGameState.get_round_number()) {
            GameController::showNewRoundMessage(old, newGameState);
        }
    }

    if(GameController::_currentGameState.value().is_finished()) {
        GameController::showGameOverMessage();
    }

    // make sure we are showing the main game panel in the window (if we are already showing it, nothing will happen)
    GameController::_gameWindow->showPanel(GameController::_mainGamePanel);

    // command the main game panel to rebuild itself, based on the new game state
    GameController::_mainGamePanel->buildGameState(GameController::_currentGameState.value(), GameController::_me.value());
}

void send_request(const client_msg_variant& req) {
    auto request = client_msg(GameController::get_me().value().get_id(), GameController::get_game_state().value().get_id(), req);
    ClientNetworkManager::sendRequest(request);
}

void GameController::startGame() {
    send_request(start_game_req());
}


void GameController::fold() {
    send_request(fold_req());
}


void GameController::playCard(const card &cardToPlay) {
    send_request(play_card_req{cardToPlay});
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


void GameController::showNewRoundMessage(const game_state &oldGameState, const game_state &newGameState) {
    std::string title = "Round Completed";
    std::string message = "The players gained the following minus points:\n";
    std::string buttonLabel = "Start next round";

    // add the point differences of all players to the messages
    for(int i = 0; i < oldGameState.get_players().size(); i++) {

        const player &oldPlayerState = oldGameState.get_players().at(i);
        const player &newPlayerState = newGameState.get_players().at(i);

        int scoreDelta = newPlayerState.get_score() - oldPlayerState.get_score();
        std::string scoreText = std::to_string(scoreDelta);
        if(scoreDelta > 0) {
            scoreText = "+" + scoreText;
        }

        std::string playerName = newPlayerState.get_player_name();
        if(newPlayerState.get_id() == GameController::_me.value().get_id()) {
            playerName = "You";
        }
        message += "\n" + playerName + ":     " + scoreText;
    }

    wxMessageDialog dialogBox = wxMessageDialog(nullptr, message, title, wxICON_NONE);
    dialogBox.SetOKLabel(wxMessageDialog::ButtonLabel(buttonLabel));
    dialogBox.ShowModal();
}


void GameController::showGameOverMessage() {
    std::string title = "Game Over!";
    std::string message = "Final score:\n";
    std::string buttonLabel = "Close Game";

    // sort players by score
    auto players = GameController::_currentGameState.value().get_players();
    std::sort(players.begin(), players.end(), [](const player &a, const player &b) -> bool {
        return a.get_score() < b.get_score();
    });

    // list all players
    for(int i = 0; i < players.size(); i++) {

        const player &playerState = players.at(i);
        std::string scoreText = std::to_string(playerState.get_score());

        // first entry is the winner
        std::string winnerText;
        if(i == 0) {
            winnerText = "     Winner!";
        }

        std::string playerName = playerState.get_player_name();
        if(playerState.get_id() == GameController::_me.value().get_id()) {
            playerName = "You";

            if(i == 0) {
                winnerText = "     You won!!!";
            }
        }
        message += "\n" + playerName + ":     " + scoreText + winnerText;
    }

    wxMessageDialog dialogBox = wxMessageDialog(nullptr, message, title, wxICON_NONE);
    dialogBox.SetOKLabel(wxMessageDialog::ButtonLabel(buttonLabel));
    int buttonClicked = dialogBox.ShowModal();
    if(buttonClicked == wxID_OK) {
        GameController::_gameWindow->Close();
    }
}
