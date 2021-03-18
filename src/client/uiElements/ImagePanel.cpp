#include "ImagePanel.h"

BEGIN_EVENT_TABLE(ImagePanel, wxPanel)
// some useful events
/*
 EVT_MOTION(ImagePanel::mouseMoved)
 EVT_LEFT_DOWN(ImagePanel::mouseDown)
 EVT_LEFT_UP(ImagePanel::mouseReleased)
 EVT_RIGHT_DOWN(ImagePanel::rightClick)
 EVT_LEAVE_WINDOW(ImagePanel::mouseLeftWindow)
 EVT_KEY_DOWN(ImagePanel::keyPressed)
 EVT_KEY_UP(ImagePanel::keyReleased)
 EVT_MOUSEWHEEL(ImagePanel::mouseWheelMoved)
 */

// catch paint events
EVT_PAINT(ImagePanel::paintEvent)
//Size event
EVT_SIZE(ImagePanel::OnSize)
END_EVENT_TABLE()


// some useful events
/*
 void ImagePanel::mouseMoved(wxMouseEvent& event) {}
 void ImagePanel::mouseDown(wxMouseEvent& event) {}
 void ImagePanel::mouseWheelMoved(wxMouseEvent& event) {}
 void ImagePanel::mouseReleased(wxMouseEvent& event) {}
 void ImagePanel::rightClick(wxMouseEvent& event) {}
 void ImagePanel::mouseLeftWindow(wxMouseEvent& event) {}
 void ImagePanel::keyPressed(wxKeyEvent& event) {}
 void ImagePanel::keyReleased(wxKeyEvent& event) {}
 */

ImagePanel::ImagePanel(wxWindow* parent, wxString file, wxBitmapType format, wxPoint position, wxSize size, double rotation) :
        wxPanel(parent, wxID_ANY, position, size)
{
    if(!wxFileExists(file)) {
        wxMessageBox("Could not find file: " + file, "File error", wxICON_ERROR);
        return;
    }

    if(!this->_image.LoadFile(file, format)) {
        wxMessageBox("Could not load file: " + file, "File error", wxICON_ERROR);
        return;
    }

    this->_rotation = rotation;

    this->_width = -1;
    this->_height = -1;
}

/*
 * Called by the system or by wxWidgets when the panel needs
 * to be redrawn. You can also trigger this call by
 * calling Refresh()/Update().
 */
void ImagePanel::paintEvent(wxPaintEvent& event) {

    if(!this->_image.IsOk()) {
        return;
    }

    wxPaintDC deviceContext = wxPaintDC(this);

    int newWidth;
    int newHeight;
    deviceContext.GetSize(&newWidth, &newHeight);

    if(newWidth != this->_width || newHeight != this->_height) {

        wxImage transformed;

        if(this->_rotation == 0.0) {
            transformed = this->_image.Scale(newWidth, newHeight, wxIMAGE_QUALITY_BILINEAR);

        } else {
            wxPoint centerOfRotation = wxPoint(this->_image.GetWidth() / 2, this->_image.GetHeight() / 2);
            transformed = this->_image.Rotate(this->_rotation, centerOfRotation, true);
            transformed = transformed.Scale(newWidth, newHeight, wxIMAGE_QUALITY_BILINEAR);
        }
        this->_bitmap = wxBitmap(transformed);
        this->_width = transformed.GetWidth();
        this->_height = transformed.GetHeight();

        deviceContext.DrawBitmap(this->_bitmap, 0, 0, false);
    } else {
        deviceContext.DrawBitmap(this->_bitmap, 0, 0, false);
    }
}

/*
 * Here we call refresh to tell the panel to draw itself again.
 * So when the user resizes the image panel the image should be resized too.
 */
void ImagePanel::OnSize(wxSizeEvent& event) {
    Refresh();
    //skip the event.
    event.Skip();
}