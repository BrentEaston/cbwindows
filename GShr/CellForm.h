// CellForm.h
//
// Copyright (c) 1994-2020 By Dale L. Larson, All Rights Reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#ifndef _CELLFORM_H
#define _CELLFORM_H

// Radio buttons in dialog match the ordering in this
// enumeration.

// NOTE: These constants are stored in the game box files!
// DO NOT mess with their values!

enum CellFormType { cformRect = 0, cformBrickHorz, cformBrickVert,
    cformHexFlat, cformHexPnt };

enum CellStagger { staggerOut = 0, staggerIn = 1 };

enum CellNumStyle { cnsRowCol = 0, cnsColRow = 1, cns0101ByRows = 2,
    cns0101ByCols = 3, cnsAA01ByRows = 4, cnsAA01ByCols = 5};

////////////////////////////////////////////////////////////////
// Various create forms:
// Rectangle --> pCell.CreateCell(cformRect, height, width);
// Bricks --> pCell.CreateCell(cformBrickHorz, height, width, cellStagger);
// Bricks --> pCell.CreateCell(cformBrickVert, height, width, cellStagger);
// HexFlatUp --> pCell.CreateCell(cformHexFlat, height, 0, cellStagger);
// HexPointUp --> pCell.CreateCell(cformHexPnt, width, 0, cellStagger);

class CCellForm
{
// Constructors / Destructors
public:
    CCellForm();
    ~CCellForm() { Clear(); }

// Attributes
public:
    CBitmap* GetMask() { return m_pMask; }
    BITMAP*  GetMaskMemoryInfo() { return m_pMask != NULL ? &m_bmapMask : NULL; }
    BOOL     HasMask() { return m_pMask != NULL; }

    CRect* GetRect(int row, int col, CRect* pRct);
    CSize GetCellSize() { return CSize(m_rct.right, m_rct.bottom); }
    CellFormType GetCellType() { return m_eType; }
    BOOL GetCellStagger() { return m_nStagger != 0; }

// Operations
public:
    void Clear();
    void CreateCell(CellFormType eType, int nParm1, int nParm2 = 0,
        int nStagger = 0);
    void FindCell(int x, int y, int& row, int& col);
    void FillCell(CDC* pDC, int xPos, int yPos);
    void FrameCell(CDC* pDC, int xPos, int yPos);
    CSize CalcBoardSize(int nRows, int nCols);
    BOOL CalcTrialBoardSize(int nRows, int nCols);

    BOOL CompareEqual(CCellForm& cf);

    // ------- //
    static void GetCellNumberStr(CellNumStyle eStyle, int row, int col,
        CString& str);
    // ------- //
    void Serialize(CArchive& ar);

// Implementation
protected:
    CellFormType m_eType;       // Format of cell
    int          m_nStagger;    // 0 = normal. 1 = alternate
    CRect        m_rct;         // Rectangle enclosing cell
    POINT*       m_pPoly;       // Zero based set of points
    CBitmap*     m_pMask;       // Only defined for hexes
    BITMAP       m_bmapMask;    // Only filled if m_pMask is defined
    // ------- //
    POINT*       m_pWrk;        // Scratch copy of m_pPoly
    // ------- //
    void OffsetPoly(POINT* pPoly, int nPts, int xOff, int yOff);
    // ------- //
    int CellPhase(int val) { return ((val ^ m_nStagger) & 1); }
    void CreateHexMask();
};

#endif

