/*
 File:       MeasurementExtractor.cpp
 
 Contains:   Definitions for class to extract from a profile measurements
						 that could have been used to create that same profile.
						 Requires caller to have chromatic adaptation matrix (usually
						 obtained from 'chad' tag) and absolute luminance information
						 usually obtained from 'lumi' tag).
 
 Version:    V1
 
 Copyright:  (c) see below
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
 * 4. The names "ICC" and "The International Color Consortium" must
 *    not be used to imply that the ICC organization endorses or
 *    promotes products derived from this software without prior
 *    written permission. For written permission, please see
 *    <http://www.color.org/>.
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
// -originally written by Joseph Goldstone fall 2006
//
//////////////////////////////////////////////////////////////////////

#include "MeasurementExtractor.h"

#include <iostream>
#include <sstream>
using namespace std;

#include "CLUT.h"
#include "IccToolException.h"

icColorSpaceSignature
MeasurementExtractor::getPCSSpace(CIccProfile* profile)
{
	return profile->m_Header.pcs;
}

icFloatNumber
MeasurementExtractor::getIlluminantY(CIccProfile* profile)
{
	CIccTagXYZ* luminanceTag
		= static_cast<CIccTagXYZ*>(profile->FindTag(icSigLuminanceTag));
	if (! luminanceTag)
		throw IccToolException("No luminance tag found");
	return icFtoD((*luminanceTag)[0].Y);
}

CAT*
MeasurementExtractor::getInverseCAT(CIccProfile* profile)
{
	CIccTagS15Fixed16* CATTag = static_cast<CIccTagS15Fixed16*>
		(profile->FindTag(icSigChromaticAdaptationTag));
	if (! CATTag)
		throw IccToolException("No chromaticAdaptation tag found");
	icFloatNumber CATContents[9];
	for(int i = 0; i < 9; ++i)
		CATContents[i] = icFtoD((*CATTag)[i]);
	CAT forward(CATContents);
	return forward.Inverse();
}

void
MeasurementExtractor::getAdaptedMediaWhite(icFloatNumber* white, CIccProfile* profile)
{
	CIccTagXYZ* mediaWhiteTag
		= static_cast<CIccTagXYZ*>(profile->FindTag(icSigMediaWhitePointTag));
	if (! mediaWhiteTag)
		throw IccToolException("No media white point tag found");
	white[0] = icFtoD((*mediaWhiteTag)[0].X);
	white[1] = icFtoD((*mediaWhiteTag)[0].Y);
	white[2] = icFtoD((*mediaWhiteTag)[0].Z);
}

MeasurementExtractor::MeasurementExtractor(const char * const profileFilename,
																					 const icFloatNumber* flare)
	: profile_    (ReadIccProfile(profileFilename)),
		isLabPCS_   (getPCSSpace(profile_) == icSigLabData),
		illuminantY_(getIlluminantY(profile_)),
		inverseCAT_ (getInverseCAT(profile_))
{
	for (unsigned int i = 0; i < 3; ++i)
		flare_[i] = flare[i];
	getAdaptedMediaWhite(adaptedMediaWhite_, profile_);
	if (cmm_.AddXform(profileFilename, icAbsoluteColorimetric))
	{
		ostringstream s;
		s << "Can't set profile `" << profileFilename
		<< "' as initial CMM profile";
		throw IccToolException(s.str());
	}
	
	if (cmm_.Begin() != icCmmStatOk)
		throw IccToolException("Error starting CMM");
}

MeasurementExtractor::MeasurementExtractor(const char * const profileFilename,
																					 icFloatNumber illuminantY,
																					 const icFloatNumber* flare)
	: profile_    (ReadIccProfile(profileFilename)),
		isLabPCS_   (getPCSSpace(profile_) == icSigLabData),
		illuminantY_(illuminantY),
		inverseCAT_ (getInverseCAT(profile_))
{
	for (unsigned int i = 0; i < 3; ++i)
		flare_[i] = flare[i];
	getAdaptedMediaWhite(adaptedMediaWhite_, profile_);
	if (cmm_.AddXform(profileFilename, icAbsoluteColorimetric) != icCmmStatOk)
	{
		ostringstream s;
		s << "Can't set profile `" << profileFilename
		<< "' as initial CMM profile";
		throw IccToolException(s.str());
	}
	
	if (cmm_.Begin() != icCmmStatOk)
		throw IccToolException("Error starting CMM");
}

MeasurementExtractor::~MeasurementExtractor()
{
	delete profile_;
	delete inverseCAT_;
}

void
MeasurementExtractor::reconstructMeasurement(icFloatNumber* measuredXYZ,
																						 icFloatNumber* RGBStimulus)
{
	icFloatNumber PCSPixel[3];
	cmm_.Apply(PCSPixel, RGBStimulus);
//	cout << "raw PCS Pixel for RGB("
//		<< RGBStimulus[0] << "," << RGBStimulus[1] << "," << RGBStimulus[2]
//		<< ") -> PCS XYZ("
//		<< PCSPixel[0] << ", " << PCSPixel[1] << ", " << PCSPixel[2]
//		<< ")";
	icFloatNumber adjustedPCSXYZ[3];
	if (isLabPCS_)
	{
//		cout << "enc("
//		<< PCSPixel[0] << ","
//		<< PCSPixel[1] << ","
//		<< PCSPixel[2] << ") then ";
		icLabFromPcs(PCSPixel);
//		cout << "decoded("
//		<< PCSPixel[0] << ","
//		<< PCSPixel[1] << ","
//		<< PCSPixel[2] << ")";
		icLabtoXYZ(adjustedPCSXYZ, PCSPixel, icD50XYZ);
	}
	else
	{
		for (unsigned int i = 0; i < 3; ++i)
			adjustedPCSXYZ[i] = PCSPixel[i];
		icXyzFromPcs(adjustedPCSXYZ);
	}
//	cout << endl;	
	icFloatNumber adaptedXYZ[3];
	for (unsigned int i = 0; i < 3; ++i)
		adaptedXYZ[i] = adjustedPCSXYZ[i] * adaptedMediaWhite_[i] / icD50XYZ[i];
	CLUT::adaptedXYZToMeasuredXYZ(measuredXYZ, adaptedXYZ, flare_, illuminantY_,
																inverseCAT_);
}
