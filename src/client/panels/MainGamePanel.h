#ifndef LAMA_CLIENT_MAINGAMEPANEL_H
#define LAMA_CLIENT_MAINGAMEPANEL_H

#include <wx/wx.h>
#include "../../game_state/game_state.h"


class MainGamePanel : public wxPanel {

public:
    MainGamePanel(wxWindow* parent);

    void buildGameState(game_state* gameState, player* me);



private:

    void buildOtherPlayerHand(game_state* gameState, player* otherPlayer, double playerAngle);
    void buildOtherPlayerLabels(game_state* gameState, player* otherPlayer, double playerAngle, int side);
    void buildCardPiles(game_state* gameState);
    void buildThisPlayer(game_state* gameState, player* me);

    wxStaticText* buildStaticText(std::string content, wxPoint position, wxSize size, long textAlignment, bool bold = false);

    wxSize getBoundsOfRotatedSquare(double edgeLength, double rotationAngle);
    double getEdgeLengthOfRotatedSquare(double originalEdgeLength, double rotationAngle);

    wxPoint getPointOnEllipse(double horizontalRadius, double verticalRadius, double angle);

    void startGame(wxCommandEvent& event);


    // define key constant layout values
    wxPoint const tableCenter = wxPoint(480, 300);
    wxSize const cardSize = wxSize(80, 124);

    double const otherPlayerHandSize = 160.0;
    double const otherPlayerHandDistanceFromCenter = 180.0;
    double const otherPlayerLabelDistanceFromCenter = 275.0;

    wxPoint const discardPileOffset = wxPoint(-84, -42);
    wxPoint const drawPileOffset = wxPoint(4, -42);

    double const twoPi = 6.28318530718;

};


#endif //LAMA_CLIENT_MAINGAMEPANEL_H
