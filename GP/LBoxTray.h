// LBoxTray.cpp
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

#ifndef _LBOXTRAY_H
#define _LBOXTRAY_H

#ifndef     _PIECES_H
#include    "Pieces.h"
#endif

#ifndef     _LBOXTILE_H
#include    "LBoxTileBase.h"
#endif

#ifndef     _DRAGDROP_H
#include    "DragDrop.h"
#endif

/////////////////////////////////////////////////////////////////////////////

class CGamDoc;
enum  TrayViz;

/////////////////////////////////////////////////////////////////////////////

class CTrayListBox : public CTileBaseListBox
{
// Construction
public:
    CTrayListBox();

// Attributes
public:
    virtual CTileManager* GetTileManager();

    TrayViz GetTrayContentVisibility() { return m_eTrayViz; }

    BOOL IsShowingTileImages();

// Operations
public:
    void DeselectAll();
    int  SelectTrayPiece(PieceID pid);
    void ShowListIndex(int nPos);

    void SetDocument(CGamDoc *pDoc);
    void SetTrayContentVisibility(TrayViz eTrayViz, LPCTSTR pszHiddenString = NULL);
    void SetTipsAllowed(BOOL bTipsAllowed)
    {
        m_bAllowTips = bTipsAllowed;
        SetTipMarkVisibility(m_bAllowTips);
    }

// Implementation
protected:
    void GetPieceTileIDs(int nIndex, TileID& tid1, TileID& tid2);

// Implementation
protected:
    CGamDoc*    m_pDoc;

    TrayViz     m_eTrayViz;
    CString     m_strHiddenString;
    BOOL        m_bAllowTips;

    // Overrides
    virtual int OnItemHeight(int nIndex);
    virtual void OnItemDraw(CDC* pDC, int nIndex, UINT nAction, UINT nState,
        CRect rctItem);
    virtual BOOL OnDragSetup(DragInfo* pDI);

    // Tool tip processing
    virtual BOOL OnIsToolTipsEnabled();
    virtual int  OnGetHitItemCodeAtPoint(CPoint point, CRect& rct);
    virtual void OnGetTipTextForItemCode(int nItemCode, CString& strTip, CString& strTitle);
    virtual BOOL OnDoesItemHaveTipText(int nItem);

    // Misc

    //{{AFX_MSG(CTrayListBox)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#endif

