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
        mpFXYVector   *Vector3;
        std::vector<double>  Vector2_X;
        mpWindow* MathPlot1;
        wxPanel* Panel1;
        std::vector<double>  Vector3_X;
        std::vector<double>  Vector3_Y;
        std::vector<double>  Vector4_Y;
        std::vector<double>  Vector4_X;
        mpScaleX   *Axis1;
        std::vector<double>  Vector2_Y;
        mpFXYVector   *Vector1;
        mpFXYVector   *Vector4;
        mpScaleY   *Axis2;
        std::vector<double>  Vector1_Y;
        std::vector<double>  Vector1_X;
        mpFXYVector   *Vector2;
        mpMarker   *Marker1;
        //*)

        DECLARE_EVENT_TABLE()
};

#endif // TESTPLOTMAIN_H
