#include "MainGamePanel.h"
#include "../uiElements/ImagePanel.h"
#include "../GameController.h"


MainGamePanel::MainGamePanel(wxWindow* parent) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(960, 680)) {

}

void MainGamePanel::buildGameState(game_state* gameState, player* me) {

    // remove any existing UI
    this->DestroyChildren();

    int numberOfPlayers = gameState->get_players().size();

    // find our own player object in the list of players
    int myPosition = -1;
    for(int i = 0; i < numberOfPlayers; i++) {
        player* lamaPlayer = gameState->get_players().at(i);
        if(lamaPlayer->get_id() == me->get_id()) {
            me = lamaPlayer;
            myPosition = i;
            break;
        }
    }

    // if we didn't find ourselves among the players, things have gone seriously wrong
    if(myPosition == -1) {
        GameController::showError("Game state error", "Could not find this player among players of server game.");
        return;
    }

    double anglePerPlayer = MainGamePanel::twoPi / (double) numberOfPlayers;

    // show all other players
    for(int i = 1; i < numberOfPlayers; i++) {

        // get player at i-th position after myself
        player* otherPlayer = gameState->get_players().at((myPosition + i) % numberOfPlayers);

        double playerAngle = (double) i * anglePerPlayer;
        int side = (2 * i) - numberOfPlayers; // side < 0 => right, side == 0 => center, side > 0 => left

        this->buildOtherPlayerHand(gameState, otherPlayer, playerAngle);
        this->buildOtherPlayerLabels(gameState, otherPlayer, playerAngle, side);
    }

    // show both card piles at the center
    this->buildCardPiles(gameState);

    // show our own player
    this->buildThisPlayer(gameState, me);

    // Update layout
    this->Layout();
}


void MainGamePanel::buildOtherPlayerHand(game_state* gameState, player* otherPlayer, double playerAngle) {

    // define the ellipse which represents the virtual player circle
    double horizontalRadius = MainGamePanel::otherPlayerHandDistanceFromCenter * 1.4; // 1.4 to horizontally elongate players' circle
    double verticalRadius = MainGamePanel::otherPlayerHandDistanceFromCenter;

    // get this player's position on that ellipse
    wxPoint handPosition = MainGamePanel::tableCenter;
    handPosition += this->getPointOnEllipse(horizontalRadius, verticalRadius, playerAngle);

    // add image of player's hand
    int numberOfCards = otherPlayer->get_hand()->get_cards().size();
    if(numberOfCards > 0) {

        // get new bounds of image, as they increase when image is rotated
        wxSize boundsOfRotatedHand = this->getBoundsOfRotatedSquare(MainGamePanel::otherPlayerHandSize, playerAngle);
        handPosition -= boundsOfRotatedHand / 2;

        std::string handImage = "assets/lama_hand_" + std::to_string(numberOfCards) + ".png";
        if(numberOfCards > 10) {
            handImage = "assets/lama_hand_10.png";
        }
        new ImagePanel(this, handImage, wxBITMAP_TYPE_ANY, handPosition, boundsOfRotatedHand, playerAngle);

    } else if(numberOfCards == 0) {

        wxSize nonRotatedSize = wxSize((int) MainGamePanel::otherPlayerHandSize, (int) MainGamePanel::otherPlayerHandSize);
        handPosition -= nonRotatedSize / 2;

        new ImagePanel(this, "assets/lama_hand_0.png", wxBITMAP_TYPE_ANY, handPosition, nonRotatedSize);
    }
}


void MainGamePanel::buildOtherPlayerLabels(game_state* gameState, player* otherPlayer, double playerAngle, int side) {

    long textAlignment = wxALIGN_CENTER_HORIZONTAL;
    int labelOffsetX = 0;

    if(side < 0) { // right side
        textAlignment = wxALIGN_LEFT;
        labelOffsetX = 80;

    } else if(side > 0) { // left side
        textAlignment = wxALIGN_RIGHT;
        labelOffsetX = -80;
    }

    // define the ellipse which represents the virtual player circle
    double horizontalRadius = MainGamePanel::otherPlayerLabelDistanceFromCenter * 1.25; // 1.25 to horizontally elongate players' circle (but less than the hands' circle)
    double verticalRadius = MainGamePanel::otherPlayerLabelDistanceFromCenter;

    // get this player's position on that ellipse
    wxPoint labelPosition = MainGamePanel::tableCenter;
    labelPosition += this->getPointOnEllipse(horizontalRadius, verticalRadius, playerAngle);
    labelPosition += wxSize(labelOffsetX, 0);

    // if game has not yet started, we only have two lines
    if(!gameState->is_started()) {
        this->buildStaticText(
                otherPlayer->get_player_name(),
                labelPosition + wxSize(-100, -18),
                wxSize(200, 18),
                textAlignment,
                true
        );
        this->buildStaticText(
                "waiting...",
                labelPosition + wxSize(-100, 0),
                wxSize(200, 18),
                textAlignment
        );

    } else {
        this->buildStaticText(
                otherPlayer->get_player_name(),
                labelPosition + wxSize(-100, -27),
                wxSize(200, 18),
                textAlignment,
                true
        );
        this->buildStaticText(
                std::to_string(otherPlayer->get_score()) + " minus points",
                labelPosition + wxSize(-100, -9),
                wxSize(200, 18),
                textAlignment
        );
        this->buildStaticText(
                "waiting...",
                labelPosition + wxSize(-100, 9),
                wxSize(200, 18),
                textAlignment
        );
    }
}


void MainGamePanel::buildCardPiles(game_state* gameState) {

    if(gameState->is_started()) {

        // Show discard pile
        const card* topCard = gameState->get_discard_pile()->get_top_card();
        if(topCard != nullptr) {
            std::string cardImage = "assets/lama_" + std::to_string(topCard->get_value()) + ".png";

            wxPoint discardPilePosition = MainGamePanel::tableCenter + MainGamePanel::discardPileOffset;

            ImagePanel* discardPile = new ImagePanel(this, cardImage, wxBITMAP_TYPE_ANY, discardPilePosition, MainGamePanel::cardSize);
            discardPile->SetToolTip("Discard pile");
        }

        // Show draw pile
        wxPoint drawPilePosition = MainGamePanel::tableCenter + MainGamePanel::drawPileOffset;

        ImagePanel* drawPile = new ImagePanel(this, "assets/lama_back.png", wxBITMAP_TYPE_ANY, drawPilePosition, MainGamePanel::cardSize);
        drawPile->SetToolTip("Draw pile");
        drawPile->SetCursor(wxCursor(wxCURSOR_HAND));

    } else {
        // if the game did not start yet, show a back side of a card in the center (only for the mood)
        wxPoint cardPosition = MainGamePanel::tableCenter - (MainGamePanel::cardSize / 2);
        new ImagePanel(this, "assets/lama_back.png", wxBITMAP_TYPE_ANY, cardPosition, MainGamePanel::cardSize);
    }

}


void MainGamePanel::buildThisPlayer(game_state* gameState, player* me) {

    // Setup two nested box sizers, in order to align our player's UI to the bottom center
    wxBoxSizer* outerLayout = new wxBoxSizer(wxHORIZONTAL);
    this->SetSizer(outerLayout);
    wxBoxSizer* innerLayout = new wxBoxSizer(wxVERTICAL);
    outerLayout->Add(innerLayout, 1, wxALIGN_BOTTOM);

    // Show the label with our player name
    wxStaticText* playerName = buildStaticText(
            me->get_player_name(),
            wxDefaultPosition,
            wxSize(200, 18),
            wxALIGN_CENTER_HORIZONTAL,
            true
    );
    innerLayout->Add(playerName, 0, wxALIGN_CENTER);




    // if the game has not yet started we say so
    if(!gameState->is_started()) {

        wxStaticText* playerPoints = buildStaticText(
                "waiting for game to start...",
                wxDefaultPosition,
                wxSize(200, 18),
                wxALIGN_CENTER_HORIZONTAL
        );
        innerLayout->Add(playerPoints, 0, wxALIGN_CENTER);

        // show button that allows our player to start the game
        wxButton* startGameButton = new wxButton(this, wxID_ANY, "Start Game!", wxDefaultPosition, wxSize(160, 64));
        startGameButton->Bind(wxEVT_BUTTON, &MainGamePanel::startGame, this);
        innerLayout->Add(startGameButton, 0, wxALIGN_CENTER | wxALL, 8);

    } else {

        wxStaticText* playerPoints = buildStaticText(
                "123 minus points", // TODO
                wxDefaultPosition,
                wxSize(200, 18),
                wxALIGN_CENTER_HORIZONTAL
        );
        innerLayout->Add(playerPoints, 0, wxALIGN_CENTER);
    }


    /*wxStaticText* playerStatus = buildStaticText( "FOLDED!", wxDefaultPosition, wxSize(-1, 32), wxTEXT_ALIGNMENT_CENTER);
    innerLayout->Add(playerStatus, 0, wxALIGN_CENTER | wxALL, 8);

    wxButton* foldButton = new wxButton(this, wxID_ANY, "Fold", wxDefaultPosition, wxSize(80, 32));
    //connectButton->Bind(wxEVT_BUTTON, &ConnectionPanel::connect, this);
    innerLayout->Add(foldButton, 0, wxALIGN_CENTER | wxALL, 8);

    wxBoxSizer* handLayout = new wxBoxSizer(wxHORIZONTAL);
    innerLayout->Add(handLayout, 0, wxALIGN_CENTER);

    int numberOfCards = 6;

    // Adjust card size, if we have too many cards on our hand
    int scaledCardWidth = std::min(MainGamePanel::cardSize.GetWidth(), (960 / numberOfCards) - 8);
    double cardAspectRatio = (double) MainGamePanel::cardSize.GetHeight() / (double) MainGamePanel::cardSize.GetWidth();
    int scaledCardHeight = (int) ((double) scaledCardWidth * cardAspectRatio);
    wxSize scaledCardSize = wxSize(scaledCardWidth, scaledCardHeight);

    for(int i = 0; i < numberOfCards; i++) {
        ImagePanel* card = new ImagePanel(this, "assets/lama_" + std::to_string((i % 6) + 1) + ".png", wxBITMAP_TYPE_ANY, wxDefaultPosition, scaledCardSize);
        card->SetToolTip("Play card");
        card->SetCursor(wxCursor(wxCURSOR_HAND));
        handLayout->Add(card, 0, wxLEFT | wxRIGHT, 4);
    }*/
}


wxStaticText* MainGamePanel::buildStaticText(std::string content, wxPoint position, wxSize size, long textAlignment, bool bold) {
    wxStaticText* staticText = new wxStaticText(this, wxID_ANY, content, position, size, textAlignment);
    if(bold) {
        wxFont font = staticText->GetFont();
        font.SetWeight(wxFONTWEIGHT_BOLD);
        staticText->SetFont(font);
    }
    return staticText;
}


wxSize MainGamePanel::getBoundsOfRotatedSquare(double edgeLength, double rotationAngle) {
    double newEdgeLength = this->getEdgeLengthOfRotatedSquare(edgeLength, rotationAngle);
    return wxSize(newEdgeLength, newEdgeLength);
}


double MainGamePanel::getEdgeLengthOfRotatedSquare(double originalEdgeLength, double rotationAngle) {
    return originalEdgeLength * (abs(sin(rotationAngle)) + abs(cos(rotationAngle)));
}


wxPoint MainGamePanel::getPointOnEllipse(double horizontalRadius, double verticalRadius, double angle) {
    return wxPoint((int) (sin(angle) * horizontalRadius), (int) (cos(angle) * verticalRadius));
}


void MainGamePanel::startGame(wxCommandEvent& event) {
    GameController::startGame();
}