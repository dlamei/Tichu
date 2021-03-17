#ifndef IMAGEPANEL_H
#define IMAGEPANEL_H

#include <wx/wx.h>
#include <wx/sizer.h>

class ImagePanel : public wxPanel
{
    wxImage _image;
    wxBitmap _bitmap;

    double _rotation;

    int _width;
    int _height;

public:
    ImagePanel(wxWindow* parent, wxString file, wxBitmapType format, wxPoint position = wxDefaultPosition, wxSize size = wxDefaultSize, double rotation = 0.0);

    void paintEvent(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);

    // some useful events
    /*
     void mouseMoved(wxMouseEvent& event);
     void mouseDown(wxMouseEvent& event);
     void mouseWheelMoved(wxMouseEvent& event);
     void mouseReleased(wxMouseEvent& event);
     void rightClick(wxMouseEvent& event);
     void mouseLeftWindow(wxMouseEvent& event);
     void keyPressed(wxKeyEvent& event);
     void keyReleased(wxKeyEvent& event);
     */

    DECLARE_EVENT_TABLE()
};

#endif // IMAGEPANEL_H
