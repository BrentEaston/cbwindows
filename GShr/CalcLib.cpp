// CalcLib.cpp -- Miscellaneous calcs
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

#include    "stdafx.h"
#include    "GMisc.h"           // To verify prototypes

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void Compute16ByteHash(LPBYTE pDataToHash, int nDataLen, LPBYTE p16ByteHashCode)
{
    MD5_CTX mdContext;
    MD5Calc(&mdContext, pDataToHash, nDataLen);
    memcpy(p16ByteHashCode, mdContext.digest, 16);
}

void MD5Calc(MD5_CTX* ctx, BYTE* pMsg, int nMsgLen)
{
    MD5Init(ctx);
    MD5Update(ctx, pMsg, nMsgLen);
    MD5Final(ctx);
}

#ifdef  GPLAY

int CalcRandomNumberUsingSeed(int nLow, UINT nRange, UINT nSeed,
    UINT* pnNextSeed /* = NULL*/)
{
    // Uses the same algorithm as the rand() runtime function.
    UINT nNextSeed = nSeed * 214013L + 2531011L;
    int nRand = ((nNextSeed >> 16) & 0x7FFF);

    // Optionally copy the current seed for reseeding follow-on
    // calls...
    if (pnNextSeed != NULL)
        *pnNextSeed = nNextSeed;

    // This should generate better results!
    // It extracts the upper bits which are said to
    // be more random than the lower obtained with mod operator
    return (nRand * nRange) / (RAND_MAX + 1) + nLow;
    // return (nRand % nRange) + nLow;
}

int CalcRandomNumber(int nLow, UINT nRange, UINT* pnNextSeed /* = NULL*/)
{
    return CalcRandomNumberUsingSeed(nLow, nRange, (UINT)GetTickCount(),
        pnNextSeed);
}

// This routine will fill a caller specified vector of random
// indices with a given zero based range. All indices will
// be unique.
void CalcRandomIndexVector(int nNumIndices, int nRange, UINT nSeed, int* pnIndices,
    UINT* pnNextSeed /* = NULL*/)
{
    ASSERT(nNumIndices <= nRange);
    ASSERT(pnIndices != NULL);
    ASSERT(nSeed != 0);

    UINT nSeedHolder;
    if (pnNextSeed == NULL)
        pnNextSeed = &nSeedHolder;
    *pnNextSeed = nSeed;

    for (int i = 0; i < nNumIndices; i++)
    {
        while (TRUE)
        {
            int nRandVal = CalcRandomNumberUsingSeed(0, nRange, *pnNextSeed, pnNextSeed);
            int j;
            for (j = 0; j < i; j++)         // make sure not already calculated
            {
                if (pnIndices[j] == nRandVal)
                    break;
            }
            if (i == j)
            {
                pnIndices[i] = nRandVal;
                break;
            }
        }
    }
}

// This routine will allocate and return a vector of random indices with a given
// zero based range. The caller must 'delete' the returned vector.
// All indices will be unique.

int* AllocateAndCalcRandomIndexVector(int nNumIndices, int nRange, UINT nSeed,
    UINT* pnNextSeed /* = NULL*/)
{
    ASSERT(nNumIndices <= nRange);
    ASSERT(nSeed != 0);

    int* pnIndices = new int[nNumIndices];
    CalcRandomIndexVector(nNumIndices, nRange, nSeed, pnIndices, pnNextSeed);

    return pnIndices;
}

#endif      // GPLAY

////////////////////////////////////////////////////////////////////////
// Force a value to the nearest regular grid location.
// The coord sys is scaled by 1000.  nMultiple and nOffset are
// already scaled by 1000.

int GridizeClosest1000(int nVal, int nMultiple, int nOffset)
{
    return GridizeClosest(1000 * nVal, nMultiple, nOffset) / 1000;
}

// Force a value to the nearest regular grid location.
int GridizeClosest(int nVal, int nMultiple, int nOffset)
{
    nVal -= nOffset;
    div_t rslt;
    rslt = div(nVal, nMultiple);
    if (rslt.rem * 2 > nMultiple)
        rslt.quot++;

    return rslt.quot * nMultiple + nOffset;
}

////////////////////////////////////////////////////////////////////////
//  StrToInt() - convert string to integer and advance pointer.
//      Returns advanced pointer.

char *StrToInt(const char *sp, int *iVal)
{
    *iVal = (int)strtol(sp, (char**)&sp, 10);
    return (char *)sp;
}

////////////////////////////////////////////////////////////////////////
//  StrDecimalChecked() - convert decimal string to integer, and
//      power of 10 scaling divisor. Advances character pointer. Returns
//      FALSE if number not converted.

static int tblPow10[] = { 1, 10, 100, 1000, 10000 };

BOOL StrDecimalChecked(const char **psp, int *pnVal, int *pnScale)
{
    int nWholePart = 0;
    int nFracPart = 0;
    int nFracDigs = 0;

    BOOL bMinus = **psp == '-';
    if (bMinus) (*psp)++;       // Step past minus character

    const char *sp = *psp;
    *psp = StrToInt(*psp, &nWholePart); // Convert whole part
    BOOL bNumFound = *psp != sp;

    if (**psp == '.')           // Check for decimal point
    {
        (*psp)++;               // Step past decimal character
        sp = *psp;              // Mark start of string
        *psp = StrToInt(*psp, &nFracPart);  // Convert fractional part
        nFracDigs = *psp - sp;
        if (nFracDigs > 4) return FALSE;    // No scale > 10000
        bNumFound = nFracDigs > 0 ? TRUE : bNumFound;
    }
    if (bNumFound)
    {
        *pnScale = tblPow10[nFracDigs];
        *pnVal = nWholePart * tblPow10[nFracDigs] + nFracPart;
        if (bMinus)
            *pnVal = -(*pnVal);
    }
    return bNumFound;
}

// ----------------------------------------------- //
// Helper for calculation allocation increments.

UINT CalcAllocSize(UINT uiDesired, UINT uiBaseSize, UINT uiExtendSize)
{
    if (uiDesired < uiBaseSize)
        return uiBaseSize;
    UINT uiVal = uiExtendSize * ((uiDesired - uiBaseSize) / uiExtendSize) +
        uiBaseSize;
    if (uiVal < uiDesired)
        uiVal += uiExtendSize;
    return uiVal;
}

// ----------------------------------------------- //
// Point scaler

void ScalePoint(POINT& pnt, const CSize& numSize, const CSize& denSize)
{
    pnt.x = MulDiv(pnt.x, numSize.cx, denSize.cx);
    pnt.y = MulDiv(pnt.y, numSize.cy, denSize.cy);
}

// ----------------------------------------------- //
// Rect scaler

void ScaleRect(RECT& rct, const CSize& numSize, const CSize& denSize)
{
    rct.left = MulDiv(rct.left, numSize.cx, denSize.cx);
    rct.right = MulDiv(rct.right, numSize.cx, denSize.cx);
    rct.top = MulDiv(rct.top, numSize.cy, denSize.cy);
    rct.bottom = MulDiv(rct.bottom, numSize.cy, denSize.cy);
}

// ----------------------------------------------- //

int NumInWordArray(CWordArray& array, int val)
{
    for (int i = 0; i < array.GetSize(); i++)
    {
        if (array[i] == (WORD)val)
            return i;
    }
    return -1;
}

// ----------------------------------------------- //
// Return a random number from 0 ... 65535.
WORD GetTimeBasedRandomNumber(BOOL bZeroAllowed)
{
    srand((unsigned)time(NULL));
    WORD wRandVal;
    while ((wRandVal = (WORD)rand()) == 0 && !bZeroAllowed) ;
    return wRandVal;
}

// ----------------------------------------------- //
// Return a random number from 0 ... 65535.
WORD GetStringBasedRandomNumber(LPCSTR str)
{
    UINT nSeed = 0;
    while (*str)
        nSeed = (nSeed << 4) + *str++;      // Use shifted sum
    srand(nSeed);
    return (WORD)rand();
}

// ----------------------------------------------- //
// return a hash of string
DWORD GetStringHash(LPCSTR str)
{
    DWORD dwHash = 0;
    while (*str)
        dwHash = (dwHash << 4) + *str++;    // Use shifted sum
    return dwHash;
}

// ----------------------------------------------- //

CPoint GetMidRect(CRect& rct)
{
    return CPoint(MidPnt(rct.left, rct.right), MidPnt(rct.top, rct.bottom));
}

///////////////////////////////////////////////////////////////////////
// Rotate point around specified origin point.

CPoint RotatePointAroundPoint(CPoint pntOrigin, CPoint pntXlate, int nAngleDeg)
{
    CPoint pntRelative = pntXlate - pntOrigin;
    int nCosA = Cos10K(nAngleDeg);
    int nSinA = Sin10K(nAngleDeg);
    int xRotated = (pntRelative.x * nCosA - pntRelative.y * nSinA) / 10000;
    int yRotated = (pntRelative.x * nSinA + pntRelative.y * nCosA) / 10000;
    CPoint pntFinal = CPoint(xRotated, yRotated) + pntOrigin;
    return pntFinal;
}

// ----------------------------------------------- //
// This probably belongs elsewhere.

CWnd* GetWindowFromPoint(CPoint pntScreen)
{
    CWnd* pWnd = CWnd::WindowFromPoint(pntScreen);

    if (pWnd == NULL)
        return NULL;

    pWnd->ScreenToClient(&pntScreen);
    pWnd = pWnd->ChildWindowFromPoint(pntScreen);

    if (pWnd == NULL)
        return NULL;

    return pWnd->IsWindowVisible() ? pWnd : NULL;
}

// ----------------------------------------------- //
// Ditto for this (This probably belongs elsewhere.)

void PushRectOntoScreen(RECT& rct)
{
    HDC hDC = GetDC(NULL);
    int xWidth = GetDeviceCaps(hDC, HORZRES);
    int yHeight = GetDeviceCaps(hDC, VERTRES);
    ReleaseDC(NULL, hDC);
    if (rct.right >= xWidth)
        OffsetRect(&rct, -(rct.right - xWidth), 0);
    if (rct.bottom >= yHeight)
        OffsetRect(&rct, 0, -(rct.bottom - yHeight));
}

////////////////////////////////////////////////////////////////////
// Integer Sine and Cosine. Input is in degrees.
// Returned values are multiplied by 10000.

int sineTbl[91] =
{
       0,
     175,
     349,
     523,
     698,
     872,
    1045,
    1219,
    1392,
    1564,
    1736,
    1908,
    2079,
    2250,
    2419,
    2588,
    2756,
    2924,
    3090,
    3256,
    3420,
    3584,
    3746,
    3907,
    4067,
    4226,
    4384,
    4540,
    4695,
    4848,
    5000,
    5150,
    5299,
    5446,
    5592,
    5736,
    5878,
    6018,
    6157,
    6293,
    6428,
    6561,
    6691,
    6820,
    6947,
    7071,
    7193,
    7314,
    7431,
    7547,
    7660,
    7771,
    7880,
    7986,
    8090,
    8192,
    8290,
    8387,
    8480,
    8572,
    8660,
    8746,
    8829,
    8910,
    8988,
    9063,
    9135,
    9205,
    9272,
    9336,
    9397,
    9455,
    9511,
    9563,
    9613,
    9659,
    9703,
    9744,
    9781,
    9816,
    9848,
    9877,
    9903,
    9925,
    9945,
    9962,
    9976,
    9986,
    9994,
    9998,
    10000,
};

int Sin10K(int angle)
{
    if (angle < 0)
        angle = 360 + angle;        // Assume within 360 degrees
    else if (angle >= 360)
        angle %= 360;

    BOOL bNeg = FALSE;
    if (angle > 270)
    {
        bNeg = TRUE;
        angle = 360 - angle;
    }
    else if (angle > 180)
    {
        bNeg = TRUE;
        angle -= 180;
    }
    else if (angle > 90)
        angle = 180 - angle;
    return bNeg ? -sineTbl[angle] : sineTbl[angle];
}

int Cos10K(int angle)
{
    if (angle < 0)
        angle = 360 + angle;        // Assume within 360 degrees
    else if (angle >= 360)
        angle %= 360;

    BOOL bNeg = FALSE;
    if (angle > 270)
        angle = 360 - angle;
    else if (angle > 180)
    {
        bNeg = TRUE;
        angle -= 180;
    }
    else if (angle > 90)
    {
        bNeg = TRUE;
        angle = 180 - angle;
    }
    return bNeg ? -sineTbl[90 - angle] : sineTbl[90 - angle];
}


