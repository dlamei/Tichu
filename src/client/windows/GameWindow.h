#ifndef LAMAUI_GAMEWINDOW_H
#define LAMAUI_GAMEWINDOW_H

#include "../panels/ConnectionPanel.h"


class GameWindow : public wxFrame
{
public:
    GameWindow(const wxString& title, const wxPoint& pos, const wxSize& size);

    void showPanel(wxPanel* panel);
    void setStatus(const std::string& message);

private:
    wxBoxSizer* _mainLayout;
    wxStatusBar* _statusBar;

    wxPanel* _currentPanel;

};


#endif //LAMAUI_GAMEWINDOW_H
