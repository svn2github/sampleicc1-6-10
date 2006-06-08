/** @file
    File:       IccTagLut.h

    Contains:   Header for implementation of the Multi-Dimensional
                Lut tag classes classes

    Version:    V1

    Copyright:  � see ICC Software License
*/

/*
 * The ICC Software License, Version 0.1
 *
 *
 * Copyright (c) 2005 The International Color Consortium. All rights 
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:  
 *       "This product includes software developed by the
 *        The International Color Consortium (www.color.org)"
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. In the absence of prior written permission, the names "ICC" and "The
 *    International Color Consortium" must not be used to imply that the
 *    ICC organization endorses or promotes products derived from this
 *    software.
 *
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE INTERNATIONAL COLOR CONSORTIUM OR
 * ITS CONTRIBUTING MEMBERS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the The International Color Consortium. 
 *
 *
 * Membership in the ICC is encouraged when this software is used for
 * commercial purposes. 
 *
 *  
 * For more information on The International Color Consortium, please
 * see <http://www.color.org/>.
 *  
 * 
 */

////////////////////////////////////////////////////////////////////// 
// HISTORY:
//
// -Initial implementation by Max Derhak 5-15-2003
//
// -Moved LUT tags to separate file 4-30-2005
//
//////////////////////////////////////////////////////////////////////

#if !defined(_ICCTAGLUT_H)
#define _ICCTAGLUT_H

#ifdef USESAMPLEICCNAMESPACE
namespace sampleICC {
#endif

/**
****************************************************************************
* Class: CIccCurve
* 
* Purpose: The base curve class
*****************************************************************************
*/
class ICCPROFLIB_API CIccCurve : public CIccTag
{
public:
  CIccCurve() {}
  virtual CIccTag *NewCopy() const { return new CIccCurve; } 
  virtual ~CIccCurve() {}

  virtual void DumpLut(std::string &sDescription, const icChar *szName,
    icColorSpaceSignature csSig, int nIndex) {}

    virtual void Begin() {}
    virtual icFloatNumber Apply(icFloatNumber v) { return v; }

    icFloatNumber Find(icFloatNumber v) { return Find(v, 0, Apply(0), 1.0, Apply(1.0)); }

protected:
  icFloatNumber Find(icFloatNumber v,
    icFloatNumber p0, icFloatNumber v0,
    icFloatNumber p1, icFloatNumber v1);

};
typedef CIccCurve* LPIccCurve;

typedef enum {
  icInitNone,
  icInitZero,
  icInitIdentity,
} icTagCurveSizeInit;

/**
****************************************************************************
* Class: CIccTagCurve
* 
* Purpose: The curveType tag
*****************************************************************************
*/
class ICCPROFLIB_API CIccTagCurve : public CIccCurve
{
public:
  CIccTagCurve(int nSize=0);
  CIccTagCurve(const CIccTagCurve &ITCurve);
  CIccTagCurve &operator=(const CIccTagCurve &CurveTag);
  virtual CIccTag *NewCopy() const { return new CIccTagCurve(*this);}
  virtual ~CIccTagCurve();

  virtual icTagTypeSignature GetType() { return icSigCurveType; }
  virtual const icChar *GetClassName() { return "CIccTagCurve"; }

  virtual void Describe(std::string &sDescription);
  virtual void DumpLut(std::string &sDescription, const icChar *szName, 
    icColorSpaceSignature csSig, int nIndex);

  virtual bool Read(icUInt32Number size, CIccIO *pIO);
  virtual bool Write(CIccIO *pIO);

  icFloatNumber &operator[](icUInt32Number index) {return m_Curve[index];}
  icFloatNumber *GetData(icUInt32Number index) {return &m_Curve[index];}
  icUInt32Number GetSize() const { return m_nSize; }
  void SetSize(icUInt32Number nSize, icTagCurveSizeInit nSizeOpt=icInitZero);

  virtual void Begin() {m_nMaxIndex = (icUInt16Number)m_nSize - 1;}
  virtual icFloatNumber Apply(icFloatNumber v);
  virtual icValidateStatus Validate(icTagSignature sig, std::string &sReport, const CIccProfile* pProfile=NULL) const;

protected:
  icFloatNumber *m_Curve;
  icUInt32Number m_nSize;
  icUInt16Number m_nMaxIndex;
};

/**
****************************************************************************
* Class: CIccTagParametricCurve
* 
* Purpose: The parametric curve type tag
*****************************************************************************
*/
class ICCPROFLIB_API CIccTagParametricCurve : public CIccCurve
{
public:
  CIccTagParametricCurve();
  CIccTagParametricCurve(const CIccTagParametricCurve &ITPC);
  CIccTagParametricCurve &operator=(const CIccTagParametricCurve &ParamCurveTag);
  virtual CIccTag *NewCopy() const { return new CIccTagParametricCurve(*this);}
  virtual ~CIccTagParametricCurve();

  virtual icTagTypeSignature GetType() { return icSigParametricCurveType; }
  virtual const icChar *GetClassName() { return "CIccTagParametricCurve"; }

  virtual void Describe(std::string &sDescription);
  virtual void DumpLut(std::string &sDescription, const icChar *szName,
    icColorSpaceSignature csSig, int nIndex);

  virtual bool Read(icUInt32Number size, CIccIO *pIO);
  virtual bool Write(CIccIO *pIO);


  bool SetFunctionType(icUInt16Number nFunctionType); //# parameters set by 
  icUInt16Number GetFunctionType() const {return m_nFunctionType; }

  icUInt16Number GetNumParam() const { return m_nNumParam; }
  icS15Fixed16Number *GetParams() const { return m_Param; }
  icS15Fixed16Number Param(int index) const { return m_Param[index]; }
  icS15Fixed16Number& operator[](int index) { return m_Param[index]; }

  virtual void Begin();
  virtual icFloatNumber Apply(icFloatNumber v);
  virtual icValidateStatus Validate(icTagSignature sig, std::string &sReport, const CIccProfile* pProfile=NULL);


protected:
  icUInt16Number      m_nReserved2;
  icUInt16Number      m_nFunctionType;
  icS15Fixed16Number *m_Param;
  icUInt16Number      m_nNumParam;

  icFloatNumber *m_dParam;
};


/**
****************************************************************************
* Class: CIccMatrix
* 
* Purpose: The base matrix class
*****************************************************************************
*/
class ICCPROFLIB_API CIccMatrix
{
public:
  CIccMatrix(bool bUseConstants=true);
  CIccMatrix(const CIccMatrix &MatrixClass);
  CIccMatrix &operator=(const CIccMatrix &MatrixClass);
  virtual ~CIccMatrix() {}

  void DumpLut(std::string &sDescription, const icChar *szName);

  icFloatNumber m_e[12]; //e = element
  bool m_bUseConstants;

  virtual void Apply(icFloatNumber *Pixel);
  icValidateStatus Validate(icTagTypeSignature sig, std::string &sReport, const CIccProfile* pProfile=NULL) const;
};

/**
****************************************************************************
* Interface Class: IIccCLUTExec
* 
* Purpose: Interface class that is useful to populate CLUTs
*****************************************************************************
*/
class ICCPROFLIB_API IIccCLUTExec 
{
public:
  virtual ~IIccCLUTExec() {}

  virtual void PixelOp(icFloatNumber* pGridAdr, icFloatNumber* pData)=0;
};

typedef icFloatNumber (*icCLUTCLIPFUNC)(icFloatNumber v);

/**
****************************************************************************
* Class: CIccCLUT
* 
* Purpose: The base multidimensional color look-up table (CLUT) class
*****************************************************************************
*/
class ICCPROFLIB_API CIccCLUT
{
public:
  CIccCLUT(icUInt8Number nInputChannels, icUInt8Number nOutputChannels, icUInt8Number nPrecision=2);
  CIccCLUT(const CIccCLUT &ICLUT);
  CIccCLUT &operator=(const CIccCLUT &CLUTClass);
  virtual ~CIccCLUT();

  void Init(icUInt8Number nGridPoints);
  void Init(icUInt8Number *pGridPoints);

  bool ReadData(icUInt32Number size, CIccIO *pIO, icUInt8Number nPrecision);
  bool WriteData(CIccIO *pIO, icUInt8Number nPrecision);

  bool Read(icUInt32Number size, CIccIO *pIO);
  bool Write(CIccIO *pIO);

  void DumpLut(std::string  &sDescription, const icChar *szName,
               icColorSpaceSignature csInput, icColorSpaceSignature csOutput);

  icFloatNumber& operator[](int index) { return m_pData[index]; }
  icFloatNumber* GetData(int index) { return &m_pData[index]; }
  icUInt32Number NumPoints() const { return m_nNumPoints; }
  icUInt8Number GridPoints() const { return m_GridPoints[0]; }
  icUInt8Number GridPoint(int index) const { return m_GridPoints[index]; }

  icUInt32Number GetDimSize(icUInt8Number nIndex) const { return m_DimSize[nIndex]; }

  icUInt8Number GetInputDim() const { return m_nInput; }
  icUInt8Number GetOutputChannels() const { return m_nOutput; }

  void Begin();
  void Interp3dTetra(icFloatNumber *destPixel, const icFloatNumber *srcPixel);
  void Interp3d(icFloatNumber *destPixel, const icFloatNumber *srcPixel);
  void Interp4d(icFloatNumber *destPixel, const icFloatNumber *srcPixel);
  void Interp5d(icFloatNumber *destPixel, const icFloatNumber *srcPixel);
  void Interp6d(icFloatNumber *destPixel, const icFloatNumber *srcPixel);
  void InterpND(icFloatNumber *destPixel, const icFloatNumber *srcPixel);

  void Iterate(IIccCLUTExec* pExec);
  icValidateStatus Validate(icTagTypeSignature sig, std::string &sReport, const CIccProfile* pProfile=NULL)  const;

  void SetClipFunc(icCLUTCLIPFUNC ClipFunc) { UnitClip = ClipFunc; }

protected:
  void Iterate(std::string &sDescription, icUInt8Number nIndex, icUInt32Number nPos);
  void SubIterate(IIccCLUTExec* pExec, icUInt8Number nIndex, icUInt32Number nPos);

  icCLUTCLIPFUNC UnitClip;

  icUInt8Number m_nReserved2[3];
  
  icUInt8Number m_nInput;
  icUInt8Number m_nOutput;
  icUInt8Number m_nPrecision;

  icUInt8Number m_GridPoints[16];
  icUInt32Number m_nNumPoints;

  icUInt32Number m_DimSize[16];
  icFloatNumber *m_pData;

  //Iteration temporary variables
  icUInt8Number m_GridAdr[16];
  icFloatNumber m_fGridAdr[16];
  icChar *m_pOutText, *m_pVal;
  icColorSpaceSignature m_csInput, m_csOutput;

  //Tetrahedral interpolation variables
  icUInt8Number m_MaxGridPoint[16];
  icUInt32Number n000, n001, n010, n011, n100, n101, n110, n111, n1000, n10000, n100000;

  //ND Interpolation
  icUInt32Number *m_nOffset;
  // Temporary ND Interp Variables
  icFloatNumber *m_g, *m_s, *m_df;
  icUInt32Number* m_ig;
  icUInt32Number m_nNodes, m_nPower[16];
};


/**
****************************************************************************
* Class: CIccMBB
*
* Purpose: The Multi-dimensional Black Box (MBB) base class for lut8, lut16,
*  lutA2B and lutB2A tag types
*****************************************************************************
*/
class ICCPROFLIB_API CIccMBB : public CIccTag
{
  friend class ICCPROFLIB_API CIccXform3DLut;
  friend class ICCPROFLIB_API CIccXform4DLut;
  friend class ICCPROFLIB_API CIccXformNDLut;
public:
  CIccMBB();
  CIccMBB(const CIccMBB &IMBB);
  CIccMBB &operator=(const CIccMBB &IMBB);
  virtual CIccTag* NewCopy() const {return new CIccMBB(*this);}
  virtual ~CIccMBB();

  virtual bool IsMBBType() { return true;}

  virtual icUInt8Number GetPrecision() { return 2; }
  virtual bool IsInputMatrix() { return m_bInputMatrix; } //Is matrix on input side of CLUT?
  virtual bool UseLegacyPCS() { return false; } //Treat Lab Encoding differently?

  bool IsInputB() { return IsInputMatrix(); }

  void Cleanup();
  void Init(icUInt8Number nInputChannels, icUInt8Number nOutputChannels);

  icUInt8Number InputChannels() const { return m_nInput; }
  icUInt8Number OutputChannels() const { return m_nOutput; }

  virtual void Describe(std::string &sDescription);

  virtual void SetColorSpaces(icColorSpaceSignature csInput, icColorSpaceSignature csOutput);
  virtual icValidateStatus Validate(icTagSignature sig, std::string &sReport, const CIccProfile* pProfile=NULL);

  LPIccCurve* NewCurvesA();
  CIccCLUT*   NewCLUT(icUInt8Number nGridPoints, icUInt8Number nPrecision=2);
  CIccCLUT*   NewCLUT(icUInt8Number *pGridPoints, icUInt8Number nPrecision=2);
  CIccMatrix* NewMatrix();
  LPIccCurve* NewCurvesM();
  LPIccCurve* NewCurvesB();

  CIccMatrix *GetMatrix() const  {return m_Matrix; }
  CIccCLUT   *GetCLUT() const    {return m_CLUT;}
  LPIccCurve *GetCurvesA() const {return m_CurvesA;}
  LPIccCurve *GetCurvesB() const {return m_CurvesB;}
  LPIccCurve *GetCurvesM() const {return m_CurvesM;}



protected:
  bool m_bInputMatrix;

  icUInt8Number m_nInput;
  icUInt8Number m_nOutput;

  icColorSpaceSignature m_csInput;
  icColorSpaceSignature m_csOutput;

  LPIccCurve *m_CurvesA;
  CIccCLUT  *m_CLUT;
  CIccMatrix *m_Matrix;
  LPIccCurve *m_CurvesM;
  LPIccCurve *m_CurvesB;

};

/**
****************************************************************************
* Class: CIccTagLutAtoB
* 
* Purpose: The LutA2B tag type
*****************************************************************************
*/
class ICCPROFLIB_API CIccTagLutAtoB : public CIccMBB
{
public:
  CIccTagLutAtoB();
  CIccTagLutAtoB(const CIccTagLutAtoB &ITLA2B);
  CIccTagLutAtoB &operator=(const CIccTagLutAtoB &ITLA2B);
  virtual CIccTag* NewCopy() { return new CIccTagLutAtoB(*this); }
  virtual ~CIccTagLutAtoB();

  virtual icTagTypeSignature GetType() { return icSigLutAtoBType; }

  bool Read(icUInt32Number size, CIccIO *pIO);
  bool Write(CIccIO *pIO);
  virtual icValidateStatus Validate(icTagSignature sig, std::string &sReport, const CIccProfile* pProfile=NULL);

protected:
  icUInt16Number m_nReservedWord;
};

/**
****************************************************************************
* Class: CIccTagLutBtoA
* 
* Purpose: The LutB2A tag type
*****************************************************************************
*/
class ICCPROFLIB_API CIccTagLutBtoA : public CIccTagLutAtoB
{
public:
  CIccTagLutBtoA();
  CIccTagLutBtoA(const CIccTagLutBtoA &ITLB2A);
  CIccTagLutBtoA &operator=(const CIccTagLutBtoA &ITLB2A);
  virtual CIccTag* NewCopy() { return new CIccTagLutBtoA(*this); }

  virtual icTagTypeSignature GetType() { return icSigLutBtoAType; }
  virtual icValidateStatus Validate(icTagSignature sig, std::string &sReport, const CIccProfile* pProfile=NULL);
};


/**
****************************************************************************
* Class: CIccTagLut8
* 
* Purpose: The Lut8 tag type
*****************************************************************************
*/
class ICCPROFLIB_API CIccTagLut8 : public CIccMBB
{
public:
  CIccTagLut8();
  CIccTagLut8(const CIccTagLut8 &ITL);
  CIccTagLut8 &operator=(const CIccTagLut8 &ITL);
  virtual CIccTag* NewCopy() const {return new CIccTagLut8(*this);}
  virtual ~CIccTagLut8();

  virtual icTagTypeSignature GetType() { return icSigLut8Type; }

  bool Read(icUInt32Number size, CIccIO *pIO);
  bool Write(CIccIO *pIO);

  virtual void SetColorSpaces(icColorSpaceSignature csInput, icColorSpaceSignature csOutput);
  virtual icValidateStatus Validate(icTagSignature sig, std::string &sReport, const CIccProfile* pProfile=NULL);

protected:
  icUInt8Number m_nReservedByte;
  icS15Fixed16Number m_XYZMatrix[9];
};

/**
****************************************************************************
* Class: CIccTagLut16
* 
* Purpose: The Lut16 tag type
*****************************************************************************
*/
class ICCPROFLIB_API CIccTagLut16 : public CIccMBB
{
public:
  CIccTagLut16();
  CIccTagLut16(const CIccTagLut16 &ITL);
  CIccTagLut16 &operator=(const CIccTagLut16 &ITL);
  virtual CIccTag* NewCopy() const {return new CIccTagLut16(*this);}
  virtual ~CIccTagLut16();
  
  virtual icTagTypeSignature GetType() { return icSigLut16Type; }
  virtual bool UseLegacyPCS() { return true; } //Treat Lab Encoding differently?

  bool Read(icUInt32Number size, CIccIO *pIO);
  bool Write(CIccIO *pIO);

  virtual void SetColorSpaces(icColorSpaceSignature csInput, icColorSpaceSignature csOutput);
  virtual icValidateStatus Validate(icTagSignature sig, std::string &sReport, const CIccProfile* pProfile=NULL);

protected:
  icUInt8Number m_nReservedByte;
  icS15Fixed16Number m_XYZMatrix[9];
};


#ifdef USESAMPLEICCNAMESPACE
} //namespace sampleICC
#endif

#endif // !defined(_ICCTAG_H)