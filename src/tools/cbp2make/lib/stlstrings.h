/*
    cbp2make : Makefile generation tool for the Code::Blocks IDE
    Copyright (C) 2010-2013 Mirai Computing (mirai.computing@gmail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

//------------------------------------------------------------------------------
#ifndef STL_STRINGS_H
#define STL_STRINGS_H
//------------------------------------------------------------------------------
#include <cstring>
#include <ostream>
#include <vector>
#include <algorithm>
//------------------------------------------------------------------------------

#define MAX_SHORTSTRING_LENGTH 256
#define INVALID_INDEX -1

class CString
{
  enum eol_t { eol_cr, eol_lf, eol_crlf };
 protected:
  std::string m_String;
  eol_t m_eol_type;
 public:
  std::string& GetString(void);
  std::string GetString(void) const;
  char *GetCString(void) const;
  int GetLength(void) const;
  bool IsEmpty(void) const;
  CString& SetLength(void);
  CString& SetLength(const int Length);
  CString& Clear(void);
  char GetChar(const int Index) const;
  char GetFirstChar(void) const;
  char GetLastChar(void) const;
  void SetChar(const int Index, const char Value);
  CString& Assign(const char AChar);
  CString& Assign(const char *AString);
  CString& Assign(const std::string& AString);
  CString& Assign(const CString& AString);
  CString& Append(const char AChar);
  CString& Append(const char *AString);
  CString& Append(const std::string& AString);
  CString& Append(const CString& AString);
  CString& AppendCR(void);
  CString& AppendLF(void);
  CString& AppendCRLF(void);
  CString& AppendEOL(void);
  CString  GetEOL(void);
  CString& SetEOL(const eol_t EndOfLine);
  char& operator [](const int Index);
  char operator [](const int Index) const;
  CString& operator =(const char AChar);
  CString& operator =(const char *AString);
  CString& operator =(const std::string& AString);
  CString& operator =(const CString& AString);
  CString& operator =(const int AInteger);
  CString& operator =(const long long int AInteger);
  CString& operator =(const float AFloat);
  CString& operator =(const double AFloat);
  CString& operator +=(const char AChar);
  CString& operator +=(const char *CString);
  CString& operator +=(const CString& AString);
  CString& operator +=(const int AInteger);
  CString& operator +=(const long long int AInteger);
  CString& operator +=(const float AFloat);
  CString& operator +=(const double AFloat);
  bool operator ==(const char *AString) const;
  bool operator ==(const std::string& AString) const;
  bool operator ==(const CString& AString) const;
  int GetInteger(void) const;
  double GetFloat(void) const;
  void Fill(const char AChar);
  void Print(std::ostream& out);
 public:
  CString(void);
  CString(const char AChar);
  CString(const char *AString);
  CString(const std::string& AString);
  CString(const CString& AString);
  ~CString(void);
};

class CStringList
{
 protected:
  std::vector<CString *> m_Strings;
  CString m_NullString;
 protected:
  bool ValidIndex(const int Index) const;
 public:
  bool IsEmpty(void) const;
  int GetCount(void) const;
  int GetLength(void) const;
  CString& GetString(const int Index);
  CString GetString(const int Index) const;
  CString Join(const CString& Delimiter) const;
  int FindString(const CString& AString) const;
  int Insert(const CString *AString);
  int Insert(const CString& AString);
  CStringList& Insert(const CStringList& AStringList);
  CStringList& InsertAt(const int Index, const CStringList& AStringList);
  CStringList& InsertAt(const int Index, const CString *AString);
  CStringList& InsertAt(const int Index, const CString& AString);
  CStringList& RemoveAt(const int Index);
  CStringList& RemoveEmpty(void);
  CStringList& RemoveDuplicates(void);
  CStringList& Clear(void);
  bool AppendFromFile(const CString& FileName);
  bool LoadFromFile(const CString& FileName);
  bool SaveToFile(const CString& FileName);
  void Print(std::ostream& out);
  CStringList& operator =(const CString& AString);
  CStringList& operator =(const CStringList& AStringList);
  CString& operator [](const int Index);
  CString operator [](const int Index) const;
  CStringList& operator <<(const CString& AString);
 public:
  CStringList(void);
  CStringList(const CStringList& AStringList);
  ~CStringList(void);
};

#define CHARSET_SIZE (1<<(8*sizeof(char)))

class CCharset
{
 protected:
  bool m_Charset[CHARSET_SIZE];
  CString m_Alphabet;
 protected:
  void UpdateCharset(void);
  void UpdateAlphabet(void);
 public:
  bool Isset(const char AChar) const;
  char GetChar(const char AChar) const;
  void SetChar(const char AChar);
  void UnsetChar(const char AChar);
  CString GetAlphabet(void) const;
  void SetAlphabet(const CString& Alphabet);
  void Print(std::ostream& out);
 public:
  CCharset(void);
  CCharset(const CCharset& Charset);
  CCharset(const CString& Alphabet);
  ~CCharset(void);
};

class CCharIterator
{
 protected:
  char m_NullChar;
 public:
  virtual int FirstPosition(void) { return 0; }
  virtual int ThisPosition(void) { return 0; }
  virtual int LastPosition(void) { return 0; }
  //
  virtual char& First(void) { return m_NullChar; }
  virtual char& Prev(void) { return m_NullChar; }
  virtual char& This(void) { return m_NullChar; }
  virtual char& Next(void) { return m_NullChar; }
  virtual char& Last(void) { return m_NullChar; }
  virtual void Reset(void) {};
  virtual bool Match(const CString& Pattern, const bool Move = false);
  virtual void Print(std::ostream& out) {};
 public:
  CCharIterator(void) { m_NullChar = 0; };
  CCharIterator(const CCharIterator& ACharIterator) { m_NullChar = 0; };
  virtual ~CCharIterator(void) {};
};

class CStringIterator: public CCharIterator
{
 protected:
  CString *m_String;
  int m_Position;
 public:
  virtual int FirstPosition(void);
  virtual int ThisPosition(void);
  virtual int LastPosition(void);
  //
  virtual char& First(void);
  virtual char& Prev(void);
  virtual char& This(void);
  virtual char& Next(void);
  virtual char& Last(void);
  virtual void Reset(void);
  virtual bool Match(const CString& Pattern, const bool Move = false);
 public:
  CStringIterator(void);
  CStringIterator(const CString *AString);
  CStringIterator(const CStringIterator& AStringIterator);
  virtual ~CStringIterator(void) {};
};

class CStringListIterator: public CCharIterator
{
 protected:
  CStringList *m_StringList;
  int m_Line;
  int m_Position;
 public:
  virtual int FirstPosition(void);
  virtual int ThisPosition(void);
  virtual int LastPosition(void);
  //
  virtual char& First(void);
  virtual char& Prev(void);
  virtual char& This(void);
  virtual char& Next(void);
  virtual char& Last(void);
  virtual void Reset(void);
  virtual bool Match(const CString& Pattern, const bool Move = false);
 public:
  CStringListIterator(void);
  CStringListIterator(const CStringList *AStringList);
  CStringListIterator(const CStringListIterator& AStringListIterator);
  virtual ~CStringListIterator(void) {};
};

static const unsigned int MAX_CHAR_FREQUENCY = ((1<<(7*sizeof(unsigned int)))-1);

class CCharHistogram
{
 private:
  static const unsigned int FLAG_PURE_NUMERIC  = 0x00000001;
  static const unsigned int FLAG_PURE_INTEGER  = 0x00000002;
  static const unsigned int FLAG_ASCII_TEXT    = 0x00000004;
  static const unsigned int FLAG_CUSTOM_BINARY = 0x00000008;
 protected:
  unsigned int m_Histogram[CHARSET_SIZE];
  unsigned int m_Flags;
 protected:
  void Analyze(void);
 public:
  void Reset(void);
  void Insert(const char AChar);
  void Insert(const CString& AString);
  void Insert(const CStringList& AStringList);
  void Remove(const char AChar);
  void Remove(const CString& AString);
  void Remove(const CStringList& AStringList);
  bool IsPureNumeric(void);
  bool IsPureInteger(void);
  bool IsAsciiText(void);
  bool IsCustomBinary(void);
  unsigned int GetAt(const char AChar) const;
  void SetAt(const char AChar, const unsigned int Frequency);
  CString GetAlphabet(void) const;
  void Print(std::ostream& out);
 public:
  CCharHistogram(void);
  CCharHistogram(const CCharHistogram& AHistogram);
  ~CCharHistogram(void);
};

static const CString ALPHABET_NULL_CHAR      = char(0x00);
static const CString ALPHABET_TAB_CHAR       = char(0x09);
static const CString ALPHABET_CR_CHAR        = char(0x0d);
static const CString ALPHABET_LF_CHAR        = char(0x0a);
static const CString ALPHABET_NUMBER         = "0123456789";
static const CString ALPHABET_INTEGER_NUMBER = "+-0123456789";
static const CString ALPHABET_FLOAT_NUMBER   = "+-.,0123456789eE";
static const CString ALPHABET_DOUBLE_NUMBER  = "+-.,0123456789dD";
static const CString ALPHABET_REAL_NUMBER    = "+-.,0123456789eEdD";
static const CString ALPHABET_HEX_NUMBER     = "0123456789abcdefABCDEF";
static const CString ALPHABET_CTL_CHARS      = "\'\"\?\\\a\b\f\n\r\t\v\0";
static const CString ALPHABET_ESC_SEQUENCE   = "\'\"?\\abfnrtv0";
static const CString ALPHABET_SHELL_CHARS    = " !@#$%^&*(){}[];\'\"\\|<>?";

static const char HEX_CHARS_UCASE[17] = "0123456789ABCDEF";
static const char HEX_CHARS_LCASE[17] = "0123456789abcdef";

inline CString operator +(const CString& AString, const CString& BString)
{ CString result(AString); result+=BString; return result; }

inline CString operator +(const CString& AString, const char BChar)
{ CString result(AString); result+=BChar; return result; }

inline CString operator +(const char AChar, const CString& BString)
{ CString result(AChar); result+=BString; return result; }

inline CString operator +(const CString& AString, const int BInteger)
{ CString result(AString); result+=BInteger; return result; }

inline CString operator +(const int AInteger, const CString& BString)
{ CString result; result+=AInteger; result+=BString; return result; }

inline CString operator +(const CString& AString, const float BFloat)
{ CString result(AString); result+=BFloat; return result; }

inline CString operator +(const float AFloat, const CString& BString)
{ CString result; result+=AFloat; result+=BString; return result; }

inline CString operator +(const CString& AString, const double BFloat)
{ CString result(AString); result+=BFloat; return result; }

inline CString operator +(const double AFloat, const CString& BString)
{ CString result; result+=AFloat; result+=BString; return result; }

inline CString operator +(const CString& AString, const char *BCString)
{ CString result(AString); result+=BCString; return result; }

inline CString operator +(const char *AString, const CString& BString)
{ CString result(AString); result+=BString; return result; }
/*
inline CString UpperCase(const CString& AString)
{
 CString result(AString); strupr(result.GetCString()); return result;
 //CString result(AString); std::strupr(result.GetCString()); return result;
}

inline CString LowerCase(const CString& AString)
{
 CString result(AString); std::strlwr(result.GetCString()); return result;
 //CString result(AString); std::strlwr(result.GetCString()); return result;
}
*/

// this might solve compatibility issues // "islower(c) ? toupper(c) : c" //

inline CString UpperCase(const CString& AString)
{
 CString result; result.SetLength(AString.GetLength());
 for (int i = 0, n = AString.GetLength(); i < n; i++)
 {
  char c = AString[i];
  result[i] = islower(c) ? toupper(c) : c;
 }
 return result;
}

inline CString LowerCase(const CString& AString)
{
 CString result; result.SetLength(AString.GetLength());
 for (int i = 0, n = AString.GetLength(); i < n; i++)
 {
  char c = AString[i];
  result[i] = isupper(c) ? tolower(c) : c;
 }
 return result;
}

inline char hexChar(const unsigned char Value)
{
 return HEX_CHARS_LCASE[Value&0xF];
}

inline char HexChar(const unsigned char Value)
{
 return HEX_CHARS_UCASE[Value&0xF];
}

inline CString hex(const unsigned int AInteger, const unsigned int Length=8)
{
 CString result; result.SetLength(Length);
 unsigned int value = AInteger;
 for (unsigned int i = 0, j = Length; i < Length; i++)
 {
  result[--j] = HEX_CHARS_LCASE[value & 0xF];
  value >>= 4;
 }
 return result;
}

inline CString Hex(const unsigned int AInteger, const unsigned int Length=8)
{
 CString result; result.SetLength(Length);
 unsigned int value = AInteger;
 for (unsigned int i = 0, j = Length; i < Length; i++)
 {
  result[--j] = HEX_CHARS_UCASE[value & 0xF];
  value >>= 4;
 }
 return result;
}

inline CString hex8(const unsigned char AInteger)
{
 return hex(AInteger,2*sizeof(unsigned char));
}

inline CString Hex8(const unsigned char AInteger)
{
 return Hex(AInteger,2*sizeof(unsigned char));
}

inline CString hex16(const unsigned short int AInteger)
{
 return hex(AInteger,2*sizeof(unsigned short int));
}

inline CString Hex16(const unsigned short int AInteger)
{
 return Hex(AInteger,2*sizeof(unsigned short int));
}

inline CString hex32(const unsigned int AInteger)
{
 return hex(AInteger,2*sizeof(unsigned int));
}

inline CString Hex32(const unsigned int AInteger)
{
 return Hex(AInteger,2*sizeof(unsigned int));
}

inline CString hex64(const long long int AInteger, const unsigned int Length=16)
{
 CString result; result.SetLength(Length);
 long long int value = AInteger;
 for (unsigned int i = 0, j = Length; i < Length; i++)
 {
  result[--j] = HEX_CHARS_LCASE[(unsigned int)value & 0xF];
  value >>= 4;
 }
 return result;
}

inline CString Hex64(const long long int AInteger, const unsigned int Length=16)
{
 CString result; result.SetLength(Length);
 long long int value = AInteger;
 for (unsigned int i = 0, j = Length; i < Length; i++)
 {
  result[--j] = HEX_CHARS_UCASE[(unsigned int)value & 0xF];
  value >>= 4;
 }
 return result;
}

inline CString hex(const unsigned char *Buffer, const unsigned int Length=0)
{
 unsigned int length = Length*2;
 CString result; result.SetLength(length);
 for (unsigned int i = 0, j = length; i < Length; i++)
 {
  result[--j] = HEX_CHARS_UCASE[Buffer[i] & 0xF];
  result[--j] = HEX_CHARS_UCASE[(Buffer[i]>>4) & 0xF];
 }
 return result;
}

inline CString Hex(const unsigned char *Buffer, const unsigned int Length=0)
{
 unsigned int length = Length*2;
 CString result; result.SetLength(length);
 for (unsigned int i = 0, j = length; i < Length; i++)
 {
  result[--j] = HEX_CHARS_UCASE[Buffer[i] & 0xF];
  result[--j] = HEX_CHARS_UCASE[(Buffer[i]>>4) & 0xF];
 }
 return result;
}

CString StringOfChars(const char AChar, const int Count);
CString FillStr(const CString& Prefix, const char AChar, const CString& Suffix, const int Length);
CString EnumStr(const CString& Prefix, const int Counter,
                const CString& Suffix, const unsigned int Length);
CString SignStr(const int Signature);
CString SubStr(const CString& AString, const int FirstPos, const int LastPos);
CString ShiftStr(const CString& AString, const int Shift);
void SplitStr(const CString& AString, const int Position, CString& LString, CString& RString);
CString JoinStr(const CString& AString, const CString& BString, const char Separator);
CString LeftStr(const CString& AString, const int Position);
CString RightStr(const CString& AString, const int Position);
CString FilterStr(const CString& AString, const CCharset& ValidChars,
                  const CCharset& InvalidChars, const bool ValidateUndefined);
int CountChars(const CString& AString, const char AChar);
int NextCharPos(const CString& AString, const char AChar, const int Position);
int PrevCharPos(const CString& AString, const char AChar, const int Position);
int FirstCharPos(const CString& AString, const char AChar);
int LastCharPos(const CString& AString, const char AChar);
CString TrimStr(const CString& AString);
CString LeadStr(const CString& AString, const unsigned int RLength);
void ParseStr(const CString& AString, const CString& Delimiters, CStringList& SubStrings);
void ParseStr(const CString& AString, const char Delimiter, CStringList& SubStrings);
CString CollectStr(const CStringList& SubStrings, const CString& Delimiters);
CString StripDupChars(const CString& AString, const char AChar);
CString StripDupChars(const CString& AString);
CString CheckFirstChar(const CString& AString, const char AChar);
CString CheckLastChar(const CString& AString, const char AChar);
int FindStr(const CString& AString, const CString& BString);
CString FindReplaceChar(const CString& AString, const char SearchChar,
                        const char ReplaceChar);
CString FindReplaceStr(const CString& AString, const CString& SearchString,
                       const CString& ReplaceString);
int ParseMemSizeStr(const CString& AString, int& UnitSize, char& UnitChar);
int ParseMemSizeStr(const CString& AString);
long long int ParseMemSizeStr64(const CString& AString,
 long long int& UnitSize, char& UnitChar);
long long int ParseMemSizeStr64(const CString& AString);
int GuessStr(const CString& AString, const CStringList& Variants,
             CString& Guessed, const bool TryFuzzy = false);
int GuessStr(const CString& AString, const CString& Variants, CString& Guessed,
             const bool TryFuzzy = false);
CString VariantStr(const int Index, const CString& Variants, const char Delimiter = ' ');
CString MemSizeStr(const long long int Value, long long int& UnitSize, CString& UnitSuffix);
CString MemSizeStr(const long long int Value);
CString SI_UnitStr(const double Value, double& UnitSize,
                   CString& UnitPrefix, CString& UnitSymbol);
CString SI_UnitStr(const double Value);
bool IsQuoted(const CString& AString);
CString QuoteStr(const CString& AString);
CString UnquoteStr(const CString& AString);
CString ShieldSpaces(const CString& AString);
CString UnshieldSpaces(const CString& AString);
CString ShieldChars(const CString& AString, const CString& Chars);
CString UnshieldChars(const CString& AString, const CString& Chars);

//---------------------------------------------------------------------------
#endif

