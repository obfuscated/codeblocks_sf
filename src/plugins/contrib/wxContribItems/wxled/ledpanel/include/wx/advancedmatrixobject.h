/***************************************************************
 * Name:      advancedmatrixobejct.h
 * Purpose:   Defines Class AdvancedMatrixObject
 * Author:    Christian Gräfe (info@mcs-soft.de)
 * Created:   2007-03-02
 * Copyright: Christian Gräfe (www.mcs-soft.de)
 * License:	  wxWindows licence
 **************************************************************/

#ifndef ADVANCEDMATRIXOBJECT_H
#define ADVANCEDMATRIXOBJECT_H

#include "wx/matrixobject.h"

class AdvancedMatrixObject : public MatrixObject
{
	public:
		AdvancedMatrixObject();
		AdvancedMatrixObject(const MatrixObject& mo);
		AdvancedMatrixObject(const char* data, int width, int height=0);
		virtual ~AdvancedMatrixObject();

		// Rotierefunktionen
		void RotateLeft();
		void RotateRight();

		// Shiftfunktionen
		void ShiftLeft();
		void ShiftRight();
		void ShiftUp();
		void ShiftDown();

		// Fitfunktionen
		void FitLeft();
		void FitRight();
		void FitTop();
		void FitBottom();

		// Füllfunktionen
		void FillLine(int y, char d);
		void FillRow(int x, char d);
		void FillAll(char d);

		// Löschfunktionen
		void ClearLine(int y);
		void ClearRow(int x);

		//

};



#endif // ADVANCEDMATRIXOBJECT_H
