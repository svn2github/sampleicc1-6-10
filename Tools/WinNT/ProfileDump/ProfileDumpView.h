/*
    File:       ProfileDumpView.h

    Contains:   interface of the CProfileDumpView class

    Version:    V1

    Copyright:  � see below
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
// -Initial implementation by Max Derhak 5-15-2003
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROFILEDUMPVIEW_H__1003B493_1A8D_40F7_A2C4_7F3AC8F352DF__INCLUDED_)
#define AFX_PROFILEDUMPVIEW_H__1003B493_1A8D_40F7_A2C4_7F3AC8F352DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TagListCtrl.h"

class CProfileDumpView : public CFormView
{
protected: // create from serialization only
  CProfileDumpView();
  DECLARE_DYNCREATE(CProfileDumpView)

public:
  //{{AFX_DATA(CProfileDumpView)
  enum { IDD = IDD_PROFILEDUMP_FORM };
  CTagListCtrl  m_listTags;
  CString  m_sHdrAttributes;
  CString  m_sHdrCmm;
  CString  m_sHdrCreationDate;
  CString  m_sHdrCreator;
  CString  m_sHdrDataColorspace;
  CString  m_sHdrFlags;
  CString  m_sHdrIlluminant;
  CString  m_sHdrPcsColorSpace;
  CString  m_sHdrPlatform;
  CString  m_sHdrRenderingIntent;
  CString  m_sHdrSize;
  CString  m_sHdrType;
  CString  m_sHdrVersion;
  CString  m_sHdrProfileID;
  //}}AFX_DATA

// Attributes
public:
  CProfileDumpDoc* GetDocument();

// Operations
public:
  void ShowTag(icTagSignature sigTag);

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CProfileDumpView)
  public:
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual void OnInitialUpdate(); // called first time after construct
  //}}AFX_VIRTUAL

// Implementation
public:
  virtual ~CProfileDumpView();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
  //{{AFX_MSG(CProfileDumpView)
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedProfileValidate();
};

#ifndef _DEBUG  // debug version in ProfileDumpView.cpp
inline CProfileDumpDoc* CProfileDumpView::GetDocument()
   { return (CProfileDumpDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROFILEDUMPVIEW_H__1003B493_1A8D_40F7_A2C4_7F3AC8F352DF__INCLUDED_)