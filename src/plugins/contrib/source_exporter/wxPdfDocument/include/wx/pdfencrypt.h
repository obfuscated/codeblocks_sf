///////////////////////////////////////////////////////////////////////////////
// Name:        pdfencrypt.h
// Purpose:     
// Author:      Ulrich Telle
// Modified by:
// Created:     2005-08-16
// Copyright:   (c) Ulrich Telle
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/// \file pdfencrypt.h Interface of the wxPdfFont class

#ifndef _PDFENCRYPT_H_
#define _PDFENCRYPT_H_

// wxWidgets headers
#include "wx/pdfdocdef.h"

#include "wx/string.h"
#include "wx/wfstream.h"

/// Class representing PDF encryption methods. (For internal use only)
class WXDLLIMPEXP_PDFDOC wxPdfEncrypt
{
public:
  /// Default constructor
  wxPdfEncrypt();

  /// Default destructor
  virtual ~wxPdfEncrypt();

  /// Generate encryption key from user and owner passwords and protection key
  void GenerateEncryptionKey(const wxString& userPassword,
                             const wxString& ownerPassword,
                             int protection);

  /// Get the U object value (user)
  unsigned char* GetUvalue() { return m_Uvalue; }

  /// Get the O object value (owner)
  unsigned char* GetOvalue() { return m_Ovalue; }

  /// Get the P object value (protection)
  int GetPvalue() { return m_Pvalue; }

  /// Encrypt a character string
  void Encrypt(int n, unsigned char* str, int len);

protected:
  /// Pad a password to 32 characters
  void PadPassword(const wxString& password, unsigned char pswd[32]);

  /// RC4 encryption
  void RC4(unsigned char* key, int keylen,
           unsigned char* textin, int textlen,
           unsigned char* textout);

  /// Calculate the binary MD5 message digest of the given data
  void GetMD5Binary(const unsigned char* data, int length, unsigned char* digest);

private:
  unsigned char m_Uvalue[32];         ///< U entry in pdf document
  unsigned char m_Ovalue[32];         ///< O entry in pdf document
  int           m_Pvalue;             ///< P entry in pdf document
  unsigned char m_encryptionKey[5];   ///< Encryption key
  unsigned char m_rc4key[5];          ///< last RC4 key
  unsigned char m_rc4last[256];       ///< last RC4 state table
};

#endif
