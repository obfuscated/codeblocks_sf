/***************************************************************
 * Name:      matrixobejct.h
 * Purpose:   Defines Class MatrixObject
 * Author:    Christian Gräfe (info@mcs-soft.de)
 * Created:   2007-03-02
 * Copyright: Christian Gräfe (www.mcs-soft.de)
 * License:	  wxWindows licence
 **************************************************************/

#ifndef MATRIXOBJECT_H
#define MATRIXOBJECT_H

#include <wx/wx.h>
#include <string.h>

class MatrixObject
{
    public:
		MatrixObject();
		MatrixObject(const MatrixObject& mo);
        MatrixObject(const char* data, int width, int height=0);
        virtual ~MatrixObject();

        MatrixObject &operator=(const MatrixObject& mo);

        void Init(const char* data, int width, int height=0);
        void Init(const MatrixObject& mo) {Init(mo.GetData(),mo.GetWidth(),mo.GetHeight());}
        void Init(const wxImage img);

		// Größe des Objektes ausgeben
        int GetWidth()  const {return m_width;}
        int GetHeight() const {return m_height;}
        int GetLength() const {return m_length;}
        wxSize GetSize() const {return wxSize(m_width,m_height);}

		// Daten von einem bestimmtem Punkt
        char GetDataFrom(int x, int y) const;
        char GetDataFrom(int p) const;

        // the pointer to the Data
        const char* GetData() const {return m_data;}

        // Has Data
        bool HasData() const {return ((m_data==0)?(false):(true));}

		// all fields null?
        bool IsEmpty() const;

        // set all fields 0
        void Clear();

        // Data
		bool SetDataAt(int x, int y, char data);
		bool SetDataAt(int p, char data);
		bool SetDataAt(const wxPoint& punkt, char data) {return SetDataAt(punkt.x,punkt.y,data);}
		bool SetDatesAt(int x, int y, const MatrixObject &mo);
		bool SetDatesAt(const wxPoint& punkt, const MatrixObject &mo) {return SetDatesAt(punkt.x,punkt.y,mo);}

        // Get as Monochrome image
        wxImage GetAsImage() const;

    protected:
		// Deinitialisieren
        void Destroy();

		// Die Variablen
        char* m_data;
		int m_width;
		int m_height;
		int m_length;
};

#endif // MATRIXOBJECT_H
