/** @file
    File:       IccCmm.h

    Contains:   Header file for implementation of the CIccCmm class.

    Version:    V1

    Copyright:  � see ICC Software License
*/

/*
 * The ICC Software License, Version 0.1
 *
 *
 * Copyright (c) 2003-2006 The International Color Consortium. All rights 
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
//////////////////////////////////////////////////////////////////////

#if !defined(_ICCCMM_H)
#define _ICCCMM_H

#include "IccProfile.h"
#include "IccTag.h"
#include "IccUtil.h"
#include <list>

#ifdef USESAMPLEICCNAMESPACE
namespace sampleICC {
#endif

/// CMM return status values
typedef enum {
  icCmmStatBad                = -1,
  icCmmStatOk                 = 0,
  icCmmStatCantOpenProfile    = 1,
  icCmmStatBadSpaceLink       = 2,
  icCmmStatInvalidProfile     = 3,
  icCmmStatBadXform           = 4,
  icCmmStatInvalidLut         = 5,
  icCmmStatProfileMissingTag  = 6,
  icCmmStatColorNotFound      = 7,
  icCmmStatIncorrectApply     = 8,
  icCmmStatBadColorEncoding   = 9,
  icCmmStatAllocErr           = 10,
  icCmmStatBadLutType         = 11,
} icStatusCMM;

/// CMM Interpolation types
typedef enum {
  icInterpLinear               = 0,
  icInterpTetrahedral          = 1,
} icXformInterp;

/// CMM Xform LUT types
typedef enum {
  icXformLutColor              = 0,
  icXformLutNamedColor         = 1,
  icXformLutPreview            = 2,
  icXformLutGamut              = 3,
} icXformLutType;

#define icPerceptualRefBlackX 0.00336
#define icPerceptualRefBlackY 0.0034731
#define icPerceptualRefBlackZ 0.00287

#define icPerceptualRefWhiteX 0.9642
#define icPerceptualRefWhiteY 1.0000
#define icPerceptualRefWhiteZ 0.8249

// CMM Xform types
typedef enum {
  icXformTypeMatrixTRC  = 0,
  icXformType3DLut      = 1,
  icXformType4DLut      = 2,
  icXformTypeNDLut      = 3,
  icXformTypeNamedColor = 4,
} icXformType;

/**
 **************************************************************************
 * Type: Class
 * 
 * Purpose: 
 *  This is the base CMM xform object.  A general static creation function,
 *  base behavior, and data are defined.  The Create() function will assign
 *  a profile to the class.  The CIccProfile object will then be owned by the
 *  xform object and later deleted when the IccXform is deleted.
 **************************************************************************
 */
class ICCPROFLIB_API CIccXform
{
public:
  CIccXform();
  virtual ~CIccXform();

  virtual icXformType GetXformType()=0;

  ///Note: The returned CIccXform will own the profile.
  static CIccXform *Create(CIccProfile *pProfile, bool bInput=true, icRenderingIntent nIntent=icUnknownIntent, 
                           icXformInterp nInterp=icInterpLinear, icXformLutType nLutType=icXformLutColor);

  ///Note: Provide an interface to work profile references.  The IccProfile is copied, and the copy's ownership
  ///is turned over to the Returned CIccXform object.
  static CIccXform *Create(CIccProfile &pProfile, bool bInput=true, icRenderingIntent nIntent=icUnknownIntent, 
                           icXformInterp nInterp=icInterpLinear, icXformLutType nLutType=icXformLutColor);

  virtual icStatusCMM Begin();
  virtual void Apply(icFloatNumber *DstPixel, const icFloatNumber *SrcPixel)=0;

  ///Returns the source color space of the transform
  virtual icColorSpaceSignature GetSrcSpace() const { return m_bInput ? m_pProfile->m_Header.colorSpace :
                                                          m_pProfile->m_Header.pcs; }
  ///Returns the destination color space of the transform
  virtual icColorSpaceSignature GetDstSpace() const { return m_bInput ? m_pProfile->m_Header.pcs :
                                                          m_pProfile->m_Header.colorSpace; }

  ///Checks if version 2 PCS is to be used
  virtual bool UseLegacyPCS() const { return false; }
  ///Checks if the profile is version 2
  virtual bool IsVersion2() const { return !m_pProfile || m_pProfile->m_Header.version < icVersionNumberV4; }

  ///Checks if the profile is to be used as input profile
  bool IsInput() const { return m_bInput; }

  //The following function is for Overriden create functinon
  void SetParams(CIccProfile *pProfile, bool bInput, icRenderingIntent nIntent, icXformInterp nInterp);

protected:
  
  const icFloatNumber *CheckSrcAbs(const icFloatNumber *Pixel);
  void CheckDstAbs(icFloatNumber *Pixel);

  CIccProfile *m_pProfile;
  bool m_bInput;
  icRenderingIntent m_nIntent;

  icFloatNumber m_AbsLab[3];
  icXYZNumber m_MediaXYZ;

  icXformInterp m_nInterp;
};

/**
 **************************************************************************
 * Type: Class
 * 
 * Purpose: Pointer to the Cmm Xform object
 **************************************************************************
 */
class ICCPROFLIB_API CIccXformPtr {
public:
  CIccXform *ptr;
};


/**
 **************************************************************************
 * Type: List Class
 * 
 * Purpose: List of CIccXformPtr which is updated on addition of Xforms
 ************************************************************************** 
 */
typedef std::list<CIccXformPtr> CIccXformList;

/**
 **************************************************************************
 * Type: Class
 * 
 * Purpose: This is the general Matrix-TRC Xform
 * 
 **************************************************************************
 */
class ICCPROFLIB_API CIccXformMatrixTRC : public CIccXform
{
public:
  CIccXformMatrixTRC();
  virtual ~CIccXformMatrixTRC();

  virtual icXformType GetXformType() { return icXformTypeMatrixTRC; }

  virtual icStatusCMM Begin();
  virtual void Apply(icFloatNumber *DstPixel, const icFloatNumber *SrcPixel);

protected:
  icFloatNumber m_e[9];
  CIccCurve *m_Curve[3];
  CIccCurve *GetCurve(icSignature sig) const;
  CIccCurve *GetInvCurve(icSignature sig) const;

  CIccTagXYZ *GetColumn(icSignature sig) const;
  bool m_bFreeCurve;
};

/**
 **************************************************************************
 * Type: Class
 * 
 * Purpose: This is the general 3D-LUT Xform
 * 
 **************************************************************************
 */
class ICCPROFLIB_API CIccXform3DLut : public CIccXform
{
public:
  CIccXform3DLut(CIccTag *pTag);
  virtual ~CIccXform3DLut();

  virtual icXformType GetXformType() { return icXformType3DLut; }

  virtual icStatusCMM Begin();
  virtual void Apply(icFloatNumber *DstPixel, const icFloatNumber *SrcPixel);

  virtual bool UseLegacyPCS() const { return m_pTag->UseLegacyPCS(); }
protected:
  CIccMBB *m_pTag;
};

/**
 **************************************************************************
 * Type: Class
 * 
 * Purpose: This is the general 4D-LUT Xform
 * 
 **************************************************************************
 */
class ICCPROFLIB_API CIccXform4DLut : public CIccXform
{
public:
  CIccXform4DLut(CIccTag *pTag);
  virtual ~CIccXform4DLut();

  virtual icXformType GetXformType() { return icXformType4DLut; }

  virtual icStatusCMM Begin();
  virtual void Apply(icFloatNumber *DstPixel, const icFloatNumber *SrcPixel);

  virtual bool UseLegacyPCS() const { return m_pTag->UseLegacyPCS(); }
protected:
  CIccMBB *m_pTag;
};

/**
 **************************************************************************
 * Type: Class
 * 
 * Purpose: This is the general ND-LUT Xform
 * 
 **************************************************************************
 */
class ICCPROFLIB_API CIccXformNDLut : public CIccXform
{
public:
  CIccXformNDLut(CIccTag *pTag);
  virtual ~CIccXformNDLut();

  virtual icXformType GetXformType() { return icXformTypeNDLut; }

  virtual icStatusCMM Begin();
  virtual void Apply(icFloatNumber *DstPixel, const icFloatNumber *SrcPixel);

  virtual bool UseLegacyPCS() const { return m_pTag->UseLegacyPCS(); }
protected:
  CIccMBB *m_pTag;
  int m_nNumInput;
};

/**
 **************************************************************************
 * Type: Enum
 * 
 * Purpose: Defines the interface to be used when applying Named Color
 *  Profiles.
 * 
 **************************************************************************
 */
typedef enum {
  icApplyPixel2Pixel = 0,
  icApplyNamed2Pixel = 1,
  icApplyPixel2Named = 2,
  icApplyNamed2Named = 3,
} icApplyInterface;


/**
 **************************************************************************
 * Type: Class
 * 
 * Purpose: This is the general Xform for Named Color Profiles.
 * 
 **************************************************************************
 */
class ICCPROFLIB_API CIccXformNamedColor : public CIccXform
{
public:
  CIccXformNamedColor(CIccTag *pTag, icColorSpaceSignature csPCS, icColorSpaceSignature csDevice);
  virtual ~CIccXformNamedColor();

  virtual icXformType GetXformType() { return icXformTypeNamedColor; }

  virtual icStatusCMM Begin();

  ///Returns the type of interface that will be applied
  icApplyInterface GetInterface() const {return m_nApplyInterface;}

  virtual void Apply(icFloatNumber *DstPixel, const icFloatNumber *SrcPixel) {}
  void Apply(icChar *DstColorName, const icFloatNumber *SrcPixel);
  icStatusCMM Apply(icFloatNumber *DstPixel, const icChar *SrcColorName);

  virtual bool UseLegacyPCS() const { return m_pTag->UseLegacyPCS(); }

  icStatusCMM SetSrcSpace(icColorSpaceSignature nSrcSpace);
  icStatusCMM SetDestSpace(icColorSpaceSignature nDestSpace);

  ///Returns the source color space of the transform
  icColorSpaceSignature GetSrcSpace() const { return m_nSrcSpace; }
  ///Returns the destination color space of the transform
  icColorSpaceSignature GetDstSpace() const { return m_nDestSpace; }

  ///Checks if the source space of the transform is PCS
  bool IsSrcPCS() const {return m_nSrcSpace == m_pTag->GetPCS();}
  ///Checks if the destination space of the transform is PCS
  bool IsDestPCS() const {return m_nDestSpace == m_pTag->GetPCS();}

protected:
  CIccTagNamedColor2 *m_pTag;
  icApplyInterface m_nApplyInterface;
  icColorSpaceSignature m_nSrcSpace;
  icColorSpaceSignature m_nDestSpace;
};


/**
 **************************************************************************
 * Type: Class
 * 
 * Purpose: Independant PCS class to do PCS based calculations.
 *  This is a class for managing PCS colorspace transformations.  There
 *  are two important categories V2 <-> V4, and Lab <-> XYZ.
 * 
 **************************************************************************
 */
class ICCPROFLIB_API CIccPCS
{
public:
  CIccPCS();
  virtual ~CIccPCS() {}

  void Reset(icColorSpaceSignature StartSpace, bool bUseLegacyPCS = false);

  virtual const icFloatNumber *Check(const icFloatNumber *SrcPixel, CIccXform *pXform);
  void CheckLast(icFloatNumber *SrcPixel, icColorSpaceSignature Space);

  static void LabToXyz(icFloatNumber *Dst, const icFloatNumber *Src, bool bNoClip=false);
  static void XyzToLab(icFloatNumber *Dst, const icFloatNumber *Src, bool bNoClip=false);
  static void Lab2ToXyz(icFloatNumber *Dst, const icFloatNumber *Src, bool bNoClip=false);
  static void XyzToLab2(icFloatNumber *Dst, const icFloatNumber *Src, bool bNoClip=false);
  static icFloatNumber NegClip(icFloatNumber v);
  static icFloatNumber UnitClip(icFloatNumber v);

  static void Lab2ToLab4(icFloatNumber *Dst, const icFloatNumber *Src);
  static void Lab4ToLab2(icFloatNumber *Dst, const icFloatNumber *Src);
protected:

  bool m_bIsV2Lab;
  icColorSpaceSignature m_Space;

  icFloatNumber m_Convert[3];
};

/**
 **************************************************************************
  Color data passed to/from the CMM is encoded as floating point numbers ranging from 0.0 to 1.0
  Often data is encoded using other ranges.  The icFloatColorEncoding enum is used by the
  ToInternalEncoding() and FromInternalEncoding() functions to convert to/from the internal
  encoding.  The valid encoding transforms for the following color space signatures are given
  below.

  'CMYK', 'RGB ', 'GRAY', 'CMY ', 'XCLR', 'Luv ', 'YCbr', 'Yxy ', 'HSV ', 'HLS ', 'gamt'
    icEncodePercent: 0.0 <= value <= 100.0
    icEncodeFloat: 0.0 <= value <= 1.0
    icEncode8Bit: 0.0 <= value <= 255
    icEncode16Bit: 0.0 <= value <= 65535

  'Lab '
    icEncodeValue: 0.0 <= L <= 100.0; -128.0 <= a,b <= 127.0
    icEncodeFloat: 0.0 <= L,a,b <= 1.0 - ICC PCS encoding (See ICC Specification)
    icEncode8BIt: ICC 8 bit Lab Encoding - See ICC Specification
    icEncode16Bit: ICC 16 bit Lab Encoding - See ICC Specification

  'XYZ '
    icEncodeValue: 0.0 <= X,Y,Z < 1.999969482421875
    icEncodePercent: 0.0 <= X,Y,Z < 199.9969482421875
    icEncodeFloat: 0.0 <= L,a,b <= 1.0 - ICC PCS encoding (See ICC Specification
    icEncode16Bit: ICC 16 bit XYZ Encoding - (icU1Fixed15) See ICC Specification
 **************************************************************************
*/

typedef enum
{
  icEncodeValue=0,
  icEncodePercent,
  icEncodeFloat,
  icEncode8Bit,
  icEncode16Bit,
  icEncodeUnknown,
} icFloatColorEncoding;

/**
 **************************************************************************
 * Type: Class 
 * 
 * Purpose: Defines a class that allows one or more profiles to be applied
 *  in order that they are Added.
 * 
 **************************************************************************
 */
class ICCPROFLIB_API CIccCmm 
{
public:
  CIccCmm(icColorSpaceSignature nSrcSpace=icSigUnknownData,
          icColorSpaceSignature nDestSpace=icSigUnknownData,
          bool bFirstInput=true);
  virtual ~CIccCmm();

  virtual CIccPCS *GetPCS() { return new CIccPCS(); }

  ///Must make at least one call to some form of AddXform() before calling Begin()
  virtual icStatusCMM AddXform(const icChar *szProfilePath, icRenderingIntent nIntent=icUnknownIntent,
                               icXformInterp nInterp=icInterpLinear, icXformLutType nLutType=icXformLutColor);
  virtual icStatusCMM AddXform(icUInt8Number *pProfileMem, icUInt32Number nProfileLen,
                               icRenderingIntent nIntent=icUnknownIntent, icXformInterp nInterp=icInterpLinear,
                               icXformLutType nLutType=icXformLutColor);
  virtual icStatusCMM AddXform(CIccProfile *pProfile, icRenderingIntent nIntent=icUnknownIntent,
                               icXformInterp nInterp=icInterpLinear, icXformLutType nLutType=icXformLutColor);  //Note: profile will be owned by the CMM
  virtual icStatusCMM AddXform(CIccProfile &Profile, icRenderingIntent nIntent=icUnknownIntent,
                               icXformInterp nInterp=icInterpLinear, icXformLutType nLutType=icXformLutColor);  //Note the profile will be copied

  ///Must be called before calling Apply()
  virtual icStatusCMM Begin(); 

  virtual icStatusCMM Apply(icFloatNumber *DstPixel, const icFloatNumber *SrcPixel);

  ///Returns the number of profiles/transforms added 
  virtual icUInt32Number GetNumXforms() const;

  ///Returns the source color space
  icColorSpaceSignature GetSourceSpace() const { return m_nSrcSpace; }
  ///Returns the destination color space
  icColorSpaceSignature GetDestSpace() const { return m_nDestSpace; }
  ///Returns the rendering intent of the last profile added
  icRenderingIntent GetLastIntent() const { return m_nLastIntent; }

  ///Returns the number of samples in the source color space
  icUInt16Number GetSourceSamples() const {return (icUInt16Number)icGetSpaceSamples(m_nSrcSpace);}
  ///Returns the number of samples in the destination color space
  icUInt16Number GetDestSamples() const {return (icUInt16Number)icGetSpaceSamples(m_nDestSpace);}

  ///Checks if this is a valid CMM object
  bool Valid() const { return m_bValid; }

  //Function to convert check if Internal representation of 'gamt' color is in gamut.
  static bool IsInGamut(icFloatNumber *pData);

  ///Functions for converting to Internal representation of pixel colors
  static icStatusCMM ToInternalEncoding(icColorSpaceSignature nSpace, icFloatColorEncoding nEncode, 
                                        icFloatNumber *pInternal, const icFloatNumber *pData, bool bClip=true);
  static icStatusCMM ToInternalEncoding(icColorSpaceSignature nSpace, icFloatNumber *pInternal, 
                                        const icUInt8Number *pData);
  static icStatusCMM ToInternalEncoding(icColorSpaceSignature nSpace, icFloatNumber *pInternal, 
                                        const icUInt16Number *pData);
  icStatusCMM ToInternalEncoding(icFloatNumber *pInternal, const icUInt8Number *pData) {return ToInternalEncoding(m_nSrcSpace, pInternal, pData);}
  icStatusCMM ToInternalEncoding(icFloatNumber *pInternal, const icUInt16Number *pData) {return ToInternalEncoding(m_nSrcSpace, pInternal, pData);}

  
  ///Functions for converting from Internal representation of pixel colors
  static icStatusCMM FromInternalEncoding(icColorSpaceSignature nSpace, icFloatColorEncoding nEncode, 
                                          icFloatNumber *pData, const icFloatNumber *pInternal, bool bClip=true);
  static icStatusCMM FromInternalEncoding(icColorSpaceSignature nSpace, icUInt8Number *pData, 
                                          const icFloatNumber *pInternal);
  static icStatusCMM FromInternalEncoding(icColorSpaceSignature nSpace, icUInt16Number *pData, 
                                          const icFloatNumber *pInternal);
  icStatusCMM FromInternalEncoding(icUInt8Number *pData, icFloatNumber *pInternal) {return FromInternalEncoding(m_nDestSpace, pData, pInternal);}
  icStatusCMM FromInternalEncoding(icUInt16Number *pData, icFloatNumber *pInternal) {return FromInternalEncoding(m_nDestSpace, pData, pInternal);}

  static const icChar *GetFloatColorEncoding(icFloatColorEncoding val);
  static icFloatColorEncoding GetFloatColorEncoding(const icChar* val);

protected:
  bool m_bValid;

  bool m_bLastInput;
  icColorSpaceSignature m_nSrcSpace;
  icColorSpaceSignature m_nDestSpace;

  icColorSpaceSignature m_nLastSpace;
  icRenderingIntent m_nLastIntent;

  CIccXformList *m_Xforms;

  CIccPCS *m_pPCS;
};

/**
 **************************************************************************
 * Type: Class
 * 
 * Purpose: A Slower Named Color Profile compatible CMM
 * 
 **************************************************************************
 */
class ICCPROFLIB_API CIccNamedColorCmm : public CIccCmm
{
public:
  ///nSrcSpace cannot be icSigUnknownData if first profile is named color
  CIccNamedColorCmm(icColorSpaceSignature nSrcSpace=icSigUnknownData, 
                    icColorSpaceSignature nDestSpace=icSigUnknownData,
                    bool bFirstInput=true);
  virtual ~CIccNamedColorCmm();

  ///Must make at least one call to some form of AddXform() before calling Begin()
  virtual icStatusCMM AddXform(const icChar *szProfilePath, icRenderingIntent nIntent=icUnknownIntent,
                               icXformInterp nInterp=icInterpLinear, icXformLutType nLutType=icXformLutColor);
  virtual icStatusCMM AddXform(CIccProfile *pProfile, icRenderingIntent nIntent=icUnknownIntent,
                               icXformInterp nInterp=icInterpLinear, icXformLutType nLutType=icXformLutColor);  //Note: profile will be owned by the CMM

  ///Must be called before calling Apply()
  virtual icStatusCMM Begin(); 

  ///Define 4 apply interfaces that are used depending upon the source and destination xforms
  virtual icStatusCMM Apply(icFloatNumber *DstPixel, const icFloatNumber *SrcPixel);
  virtual icStatusCMM Apply(icChar* DstColorName, const icFloatNumber *SrcPixel);
  virtual icStatusCMM Apply(icFloatNumber *DstPixel, const icChar *SrcColorName);
  virtual icStatusCMM Apply(icChar* DstColorName, const icChar *SrcColorName);

  ///Returns the type of interface that will be applied
  icApplyInterface GetInterface() const {return m_nApplyInterface;}

  icStatusCMM SetLastXformDest(icColorSpaceSignature nDestSpace);  

protected:
  icApplyInterface m_nApplyInterface;
};

#ifdef USESAMPLEICCNAMESPACE
}; //namespace sampleICC
#endif

#endif // !defined(_ICCCMM_H)