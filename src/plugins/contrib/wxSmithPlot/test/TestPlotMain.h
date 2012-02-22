/***************************************************************
 * Name:      TestPlotMain.h
 * Purpose:   Defines Application Frame
 * Author:     ()
 * Created:   2010-02-05
 * Copyright:  ()
 * License:
 **************************************************************/

#ifndef TESTPLOTMAIN_H
#define TESTPLOTMAIN_H

//(*Headers(TestPlotFrame)
#include <mathplot.h>
#include <wx/menu.h>
#include <wx/panel.h>
#include <mpMarker.h>
#include <wx/frame.h>
//*)

#include <mpMarker.h>

class TestPlotFrame: public wxFrame
{
    public:

        TestPlotFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~TestPlotFrame();

    private:

        //(*Handlers(TestPlotFrame)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        //*)

        //(*Identifiers(TestPlotFrame)
        static const long ID_AXIS1;
        static const long ID_AXIS2;
        static const long ID_MARKER1;
        static const long ID_VECTOR1;
        static const long ID_VECTOR2;
        static const long ID_VECTOR3;
        static const long ID_VECTOR4;
        static const long ID_MATHPLOT1;
        static const long ID_PANEL1;
        static const long idMenuQuit;
        static const long idMenuAbout;
        //*)

        //(*Declarations(TestPlotFrame)
        mpScaleX   *Axis1;
        mpMarker   *Marker1;
        mpScaleY   *Axis2;
        std::vector<float>  Vector2_Y;
        wxPanel* Panel1;
        mpWindow* MathPlot1;
        mpFXYVector   *Vector2;
        std::vector<float>  Vector3_X;
        mpFXYVector   *Vector1;
        std::vector<float>  Vector2_X;
        std::vector<float>  Vector4_Y;
        mpFXYVector   *Vector4;
        std::vector<float>  Vector1_Y;
        mpFXYVector   *Vector3;
        std::vector<float>  Vector1_X;
        std::vector<float>  Vector3_Y;
        std::vector<float>  Vector4_X;
        //*)

        DECLARE_EVENT_TABLE()
};

#endif // TESTPLOTMAIN_H
