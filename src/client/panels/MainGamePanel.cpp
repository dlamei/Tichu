#include "MainGamePanel.h"
#include "../uiElements/ImagePanel.h"





MainGamePanel::MainGamePanel(wxWindow* parent) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(960, 680)) {

}

void MainGamePanel::buildGameState(game_state* gameState, player* me) {

    // remove any existing UI
    this->DestroyChildren();

    int numberOfPlayers = gameState->get_players().size();

    // find my own player object in the list of players
    int myPosition = -1;
    for(int i = 0; i < numberOfPlayers; i++) {
        player* lamaPlayer = gameState->get_players().at(i);
        if(lamaPlayer->get_id() == me->get_id()) {
            me = lamaPlayer;
            myPosition = i;
            break;
        }
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

    ImagePanel* discardPile = new ImagePanel(this, "assets/lama_lama.png", wxBITMAP_TYPE_ANY, MainGamePanel::tableCenter + wxPoint(-84, -42), cardSize);
    discardPile->SetToolTip("Discard pile");

    ImagePanel* drawPile = new ImagePanel(this, "assets/lama_back.png", wxBITMAP_TYPE_ANY, MainGamePanel::tableCenter + wxPoint(4, -42), cardSize);
    drawPile->SetToolTip("Draw pile");
    drawPile->SetCursor(wxCursor(wxCURSOR_HAND));

    wxBoxSizer* outerLayout = new wxBoxSizer(wxHORIZONTAL);
    this->SetSizer(outerLayout);

    wxBoxSizer* innerLayout = new wxBoxSizer(wxVERTICAL);
    outerLayout->Add(innerLayout, 1, wxALIGN_BOTTOM);

    wxStaticText* playerName = new wxStaticText(this, wxID_ANY, "Playername", wxDefaultPosition, wxDefaultSize, wxTEXT_ALIGNMENT_CENTER);
    innerLayout->Add(playerName, 0, wxALIGN_CENTER);

    wxStaticText* playerPoints = new wxStaticText(this, wxID_ANY, "Minus points: 123", wxDefaultPosition, wxDefaultSize, wxTEXT_ALIGNMENT_CENTER);
    innerLayout->Add(playerPoints, 0, wxALIGN_CENTER);

    wxStaticText* playerStatus = new wxStaticText(this, wxID_ANY, "FOLDED!", wxDefaultPosition, wxSize(-1, 32), wxTEXT_ALIGNMENT_CENTER);
    innerLayout->Add(playerStatus, 0, wxALIGN_CENTER | wxALL, 8);

    wxButton* foldButton = new wxButton(this, wxID_ANY, "Fold", wxDefaultPosition, wxSize(80, 32));
    //connectButton->Bind(wxEVT_BUTTON, &ConnectionPanel::connect, this);
    innerLayout->Add(foldButton, 0, wxALIGN_CENTER | wxALL, 8);

    wxBoxSizer* handLayout = new wxBoxSizer(wxHORIZONTAL);
    innerLayout->Add(handLayout, 0, wxALIGN_CENTER);

    int numberOfCards = 6;

    // Adjust card size, if we have too many cards on our hand
    int scaledCardWidth = std::min(cardSize.GetWidth(), (960 / numberOfCards) - 8);
    double cardAspectRatio = (double) cardSize.GetHeight() / (double) cardSize.GetWidth();
    int scaledCardHeight = (int) ((double) scaledCardWidth * cardAspectRatio);
    wxSize scaledCardSize = wxSize(scaledCardWidth, scaledCardHeight);

    for(int i = 0; i < numberOfCards; i++) {
        ImagePanel* card = new ImagePanel(this, "assets/lama_" + std::to_string((i % 6) + 1) + ".png", wxBITMAP_TYPE_ANY, wxDefaultPosition, scaledCardSize);
        card->SetToolTip("Play card");
        card->SetCursor(wxCursor(wxCURSOR_HAND));
        handLayout->Add(card, 0, wxLEFT | wxRIGHT, 4);
    }

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


void MainGamePanel::buildStaticText(std::string content, wxPoint position, wxSize size, long textAlignment, bool bold) {
    wxStaticText* text = new wxStaticText(this, wxID_ANY, content, position, size, textAlignment);
    if(bold) {
        wxFont font = text->GetFont();
        font.SetWeight(wxFONTWEIGHT_BOLD);
        text->SetFont(font);
    }
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