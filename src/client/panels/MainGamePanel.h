#ifndef TICHU_CLIENT_MAINGAMEPANEL_H
#define TICHU_CLIENT_MAINGAMEPANEL_H

#include <wx/wx.h>
#include "../../common/game_state/game_state.h"


class MainGamePanel : public wxPanel {

public:
    MainGamePanel(wxWindow* parent);

    void buildGameState(const game_state &gameState, player me);


private:

    void buildOtherPlayerHand(const game_state &gameState, player otherPlayer, double playerAngle);
    void buildOtherPlayerLabels(const game_state &gameState, player otherPlayer, double playerAngle, int side);
    void buildCardPiles(const game_state &gameState, player me);
    void buildTurnIndicator(const game_state &gameState, player me);
    void buildThisPlayer(const game_state &gameState, player me);

    wxStaticText* buildStaticText(std::string content, wxPoint position, wxSize size, long textAlignment, bool bold = false);

    wxSize getBoundsOfRotatedSquare(double edgeLength, double rotationAngle);
    double getEdgeLengthOfRotatedSquare(double originalEdgeLength, double rotationAngle);

    wxPoint getPointOnEllipse(double horizontalRadius, double verticalRadius, double angle);

    // define key constant layout values
    wxSize const panelSize = wxSize(960, 680); // also set in the constructor implementation
    wxPoint const tableCenter = wxPoint(480, 300);
    wxSize const cardSize = wxSize(80, 124);

    double const otherPlayerHandSize = 160.0;
    double const otherPlayerHandDistanceFromCenter = 180.0;
    double const otherPlayerLabelDistanceFromCenter = 275.0;

    wxPoint const discardPileOffset = wxPoint(-84, -42);
    wxPoint const drawPileOffset = wxPoint(4, -42);
    wxPoint const turnIndicatorOffset = wxPoint(-100, 98);

    double const twoPi = 6.28318530718;

};


#endif //TICHU_CLIENT_MAINGAMEPANEL_H
