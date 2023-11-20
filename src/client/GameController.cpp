#include "GameController.h"
#include "../common/network/client_msg.h"
#include "network/ClientNetworkManager.h"
#include <algorithm>


// initialize static members
GameWindow* GameController::_gameWindow = nullptr;
ConnectionPanel* GameController::_connectionPanel = nullptr;
MainGamePanel* GameController::_mainGamePanel = nullptr;

std::optional<player> GameController::_me = {};
std::optional<game_state> GameController::_currentGameState = {};
std::vector<int> GameController::_selected_cards = {};
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

    GameController::_gameWindow->Layout();
    
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

    GameController::_selected_cards = std::vector<int>();

}


void GameController::updateGameState(const game_state &newGameState) {



    // the existing game state is now old
    auto oldGameState = GameController::_currentGameState;

    //current Game state is now the new one
    GameController::_currentGameState = newGameState;
    
    // save the new game state as our current game state
    if (oldGameState) {
        auto old = oldGameState.value();
    }

    if(GameController::_currentGameState.value().is_trick_finished()) {
        //GameController::showNewTrickMessage();
    }
    if(GameController::_currentGameState.value().is_round_finished()) {
        GameController::showNewRoundMessage(GameController::_currentGameState.value(), newGameState);
    }
    if(GameController::_currentGameState.value().is_game_finished()) {
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

void GameController::play(const player &me) {
    std::vector<Card> cards;
    auto c = GameController::_selected_cards;
    auto hand_cards = me.get_hand().get_cards();
    for(int card_idx : GameController::_selected_cards) {
        cards.push_back(hand_cards.at(card_idx));
    }
    card_combination combi(cards);
    playCombi(combi);
}

void GameController::add_or_remove_Selected_card(int card_idx) {
    auto card = std::find(GameController::_selected_cards.begin(), GameController::_selected_cards.end(), card_idx);
    if(card == GameController::_selected_cards.end()){
        GameController::_selected_cards.push_back(card_idx);
    } else {
        GameController::_selected_cards.erase(card);
    }
}

void GameController::playCombi(const card_combination &combiToPlay) {
    send_request(play_combi_req{combiToPlay});
    _selected_cards.clear();
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
    std::string message = "The teams have the following points:\n";
    std::string buttonLabel = "Start next round";

        std::string scoreText_A = std::to_string(newGameState.get_score_team_A());
        std::string Name_team_A = "Team A: ";
        message += "\n" + Name_team_A + ":     " + scoreText_A;

        std::string scoreText_B = std::to_string(newGameState.get_score_team_B());
        std::string Name_team_B = "Team B: ";
        message += "\n" + Name_team_B + ":     " + scoreText_B;
    

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
    std::sort(players.begin(), players.end(), [](player_ptr a, player_ptr &b) -> bool {
        return ((a->get_team())?_currentGameState.value().get_score_team_B():_currentGameState.value().get_score_team_A())
               < ((b->get_team())?_currentGameState.value().get_score_team_B():_currentGameState.value().get_score_team_A());
    });
 
    // list all players
    for(int i = 0; i < players.size(); i++) {

        const player &playerState = *(players.at(i));
        std::string scoreText = std::to_string(((i % 2) == 0) ? _currentGameState.value().get_score_team_A() : 
                                                _currentGameState.value().get_score_team_B());

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
