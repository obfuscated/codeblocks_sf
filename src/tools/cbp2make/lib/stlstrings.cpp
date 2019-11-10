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
#include <cstdlib>
#include <cmath>
#include <iostream>
//------------------------------------------------------------------------------
#include "stlstrings.h"
#include "stlconvert.h"
//------------------------------------------------------------------------------

std::string& CString::GetString(void)
{
 return m_String;
}

std::string CString::GetString(void) const
{
 return m_String;
}

char *CString::GetCString(void) const
{
 return (char *)m_String.c_str();
}

int CString::GetLength(void) const
{
 return m_String.size();
}

bool CString::IsEmpty(void) const
{
 return (GetLength()==0);
}

CString& CString::SetLength(void)
{
 return SetLength(strlen(GetCString()));
}

CString& CString::SetLength(const int Length)
{
 m_String.resize(Length);
 return *this;
}

CString& CString::Clear(void)
{
 m_String.clear();
 return *this;
}

char CString::GetChar(const int Index) const
{
 if ((Index>=0)&&(Index<GetLength())) return m_String[Index]; else return 0;
}

char CString::GetFirstChar(void) const
{
 return m_String[0];
}

char CString::GetLastChar(void) const
{
 if (m_String.size()) return m_String[GetLength()-1]; else return 0;
}

void CString::SetChar(const int Index, const char Value)
{
 if ((Index>=0)&&(Index<GetLength())) m_String[Index] = Value;
}

CString& CString::Assign(const char AChar)
{
 m_String.resize(1);
 m_String[0]=AChar;
 return *this;
}

CString& CString::Assign(const char *AString)
{
 m_String = AString;
 return *this;
}

CString& CString::Assign(const std::string& AString)
{
 m_String = AString;
 return *this;
}

CString& CString::Assign(const CString& AString)
{
 m_String = AString.GetString();
 return *this;
}

CString& CString::Append(const char AChar)
{
 SetLength(GetLength()+1);
 m_String[GetLength()-1]=AChar;
 return *this;
}

CString& CString::Append(const char *AString)
{
 m_String.append(AString);
 return *this;
}

CString& CString::Append(const CString& AString)
{
 m_String.append(AString.GetString());
 return *this;
}

CString& CString::AppendCR(void)
{
 return Append(char(0xd));
}

CString& CString::AppendLF(void)
{
 return Append(char(0xa));
}

CString& CString::AppendCRLF(void)
{
 const char *_CRLF = "\r\n";
 return Append(_CRLF);
}

CString& CString::AppendEOL(void)
{
 switch (m_eol_type)
 {
  case eol_cr: { AppendCR(); break; }
  case eol_lf: { AppendLF(); break; }
  case eol_crlf: { AppendCRLF(); break; }
 }
 return *this;
}

CString  CString::GetEOL(void)
{
 CString result;
 result.AppendEOL();
 return result;
}

CString& CString::SetEOL(const eol_t EOL)
{
 m_eol_type = EOL;
 return *this;
}

char& CString::operator [](const int Index)
{
 return m_String[Index];
}

char CString::operator [](const int Index) const
{
 return m_String[Index];
}

CString& CString::operator =(const char AChar)
{
 return Assign(AChar);
}

CString& CString::operator =(const char *AString)
{
 return Assign(AString);
}

CString& CString::operator =(const std::string& AString)
{
 return Assign(AString);
}

CString& CString::operator =(const CString& AString)
{
 return Assign(AString);
}

CString& CString::operator =(const int AInteger)
{
 char buffer[MAX_SHORTSTRING_LENGTH];
 sprintf(buffer,"%d",AInteger);
 *this = buffer;
 return *this;
}

CString& CString::operator =(const long long int AInteger)
{
 char buffer[MAX_SHORTSTRING_LENGTH];
 sprintf(buffer,"%lld",AInteger);
 *this = buffer;
 return *this;
}

CString& CString::operator =(const float AFloat)
{
 char buffer[MAX_SHORTSTRING_LENGTH];
 sprintf(buffer,"%e",AFloat);
 *this = buffer;
 return *this;
}

CString& CString::operator =(const double AFloat)
{
 char buffer[MAX_SHORTSTRING_LENGTH];
 sprintf(buffer,"%le",AFloat);
 *this = buffer;
 return *this;
}

CString& CString::operator +=(const char AChar)
{
 return Append(AChar);
}

CString& CString::operator +=(const char *AString)
{
 return Append(AString);
}

CString& CString::operator +=(const CString& AString)
{
 return Append(AString);
}

CString& CString::operator +=(const int AInteger)
{
 char buffer[MAX_SHORTSTRING_LENGTH];
 sprintf(buffer,"%d",AInteger);
 Append(buffer);
 return *this;
}

CString& CString::operator +=(const long long int AInteger)
{
 char buffer[MAX_SHORTSTRING_LENGTH];
 sprintf(buffer,"%lld",AInteger);
 Append(buffer);
 return *this;
}

CString& CString::operator +=(const float AFloat)
{
 char buffer[MAX_SHORTSTRING_LENGTH];
 sprintf(buffer,"%e",AFloat);
 Append(buffer);
 return *this;
}

CString& CString::operator +=(const double AFloat)
{
 char buffer[MAX_SHORTSTRING_LENGTH];
 sprintf(buffer,"%le",AFloat);
 Append(buffer);
 return *this;
}

bool CString::operator ==(const char *AString) const
{
 return (0==strcmp(GetCString(),AString));
}

bool CString::operator ==(const std::string& AString) const
{
 return (0==strcmp(GetCString(),AString.c_str()));
}

bool CString::operator ==(const CString& AString) const
{
 return (0==strcmp(GetCString(),AString.GetCString()));
}

int CString::GetInteger(void) const
{
 return atoi(GetCString());
}

double CString::GetFloat(void) const
{
 return atof(GetCString());
}

void CString::Fill(const char AChar)
{
 for (int i = 0, n = GetLength(); i < n; i++)
 {
  m_String[i] = AChar;
 }
}

void CString::Print(std::ostream& out)
{
 out<<m_String;
}

CString::CString(void)
{
 m_eol_type = eol_lf;
}

CString::CString(const char AChar)
{
 Assign(AChar);
 m_eol_type = eol_lf;
}

CString::CString(const char *AString)
{
 Assign(AString);
 m_eol_type = eol_lf;
}

CString::CString(const std::string& AString)
{
 Assign(AString);
 m_eol_type = eol_lf;
}

CString::CString(const CString& AString)
{
 Assign(AString);
 m_eol_type = eol_lf;
}

CString::~CString(void)
{
 //Clear();
}

//------------------------------------------------------------------------------

CStringList::CStringList(void)
{
 //Clear();
}

CStringList::CStringList(const CStringList& AStringList)
{
 Clear();
 Insert(AStringList);
}

CStringList::~CStringList(void)
{
 Clear();
}

bool CStringList::ValidIndex(const int Index) const
{
 return ((Index>=0)&&(Index<GetCount()));
}

bool CStringList::IsEmpty(void) const
{
 if (0==GetCount()) return true;
 if (0==GetLength()) return true;
 return false;
}

int CStringList::GetCount(void) const
{
 return m_Strings.size();
}

int CStringList::GetLength(void) const
{
 int result = 0;
 for (int i = GetCount()-1; i > 0; i--) result += m_Strings[i]->GetLength();
 return result;
}

CString& CStringList::GetString(const int Index)
{
 if (ValidIndex(Index))
 {
  return *m_Strings[Index];
 }
 else
 {
  return m_NullString;
 }
}

CString CStringList::GetString(const int Index) const
{
 if (ValidIndex(Index))
 {
  return *m_Strings[Index];
 }
 else
 {
  return m_NullString;
 }
}

CString CStringList::Join(const CString& Delimiter) const
{
 CString result;
 if (GetCount() > 0)
 {
  result.Append(*m_Strings[0]);
  for (int i = 1, n = GetCount(); i < n; i++)
  {
   result.Append(Delimiter).Append(*m_Strings[i]);
  }
 }
 return result;
}

int CStringList::FindString(const CString& AString) const
{
 for (int i = 0, n = GetCount(); i < n; i++)
 {
  if (GetString(i)==AString) return i;
 }
 return INVALID_INDEX;
}

int CStringList::Insert(const CString *AString)
{
 m_Strings.push_back((CString *)AString);
 return GetCount()-1;
}

int CStringList::Insert(const CString& AString)
{
 m_Strings.push_back(new CString(AString));
 return GetCount()-1;
}

CStringList& CStringList::Insert(const CStringList& AStringList)
{
 for (int i = 0; i < AStringList.GetCount(); i++)
 {
  Insert(AStringList[i]);
 }
 return *this;
}

CStringList& CStringList::InsertAt(const int Index, const CStringList& AStringList)
{
 for (int i = 0; i < AStringList.GetCount(); i++)
 {
  InsertAt(Index+i,AStringList[i]);
 }
 return *this;
}

CStringList& CStringList::InsertAt(const int Index, const CString *AString)
{
 if (ValidIndex(Index))
 {
  m_Strings.insert(m_Strings.begin()+Index,(CString *)AString);
 }
 return *this;
}

CStringList& CStringList::InsertAt(const int Index, const CString& AString)
{
 if (ValidIndex(Index))
 {
  m_Strings.insert(m_Strings.begin()+Index,new CString(AString));
 }
 return *this;
}

CStringList& CStringList::RemoveAt(const int Index)
{
 if (ValidIndex(Index))
 {
  CString *string = m_Strings[Index];
  m_Strings.erase(m_Strings.begin()+Index);
  delete string;
 }
 return *this;
}

CStringList& CStringList::RemoveEmpty(void)
{
 for (int i = m_Strings.size()-1; i >= 0; i--)
 {
  if (GetString(i).IsEmpty())
  {
   RemoveAt(i);
 }}
 return *this;
}

CStringList& CStringList::RemoveDuplicates(void)
{
 for (size_t j = 0; j < m_Strings.size(); j++)
 {
  for (size_t i = m_Strings.size()-1; i > j; i--)
  {
   if (GetString(j)==GetString(i))
   {
    RemoveAt(i);
 }}}
 return *this;
}

CStringList& CStringList::Clear(void)
{
 for (int i = m_Strings.size()-1; i >= 0; i--)
 {
  RemoveAt(i);
 }
 return *this;
}

bool CStringList::AppendFromFile(const CString& FileName)
{
 FILE* stream = fopen(FileName.GetCString(),"rb");
 CString buffer; char c,c2; int buf_pos;
 if (NULL!=stream)
 {
  buffer.SetLength(0x100);
  buf_pos = 0; c = 0;
  while (!feof(stream))
  {
   c2 = c;
   size_t result = fread(&c,1,1,stream);
   if (result != 1)
    break;
   switch (c)
   {
    case 0x00: case 0x0d:
    {
     buffer.SetLength(buf_pos);
     Insert(buffer);
     buf_pos = 0;
     break;
    }
    case 0x0a:
    {
     if (0x0d!=c2)
     {
      buffer.SetLength(buf_pos);
      Insert(buffer);
      buf_pos = 0;
     }
     break;
    }
    default:
    {
     if (buf_pos>=buffer.GetLength()) buffer.SetLength(buffer.GetLength()+0x100);
     buffer[buf_pos++] = c;
     break;
  }}}
  if (buf_pos>0)
  {
   buffer.SetLength(buf_pos-1);
   Insert(buffer);
  }
  fclose(stream);
  return true;
 }
 return false;
}

bool CStringList::LoadFromFile(const CString& FileName)
{
 Clear();
 return AppendFromFile(FileName);
}

bool CStringList::SaveToFile(const CString& FileName)
{
 FILE* stream = fopen(FileName.GetCString(),"wb");
 if (NULL!=stream)
 {
  CString buffer; buffer.AppendEOL();
  for (int i = 0; i < GetCount(); i++)
  {
   CString& string = GetString(i);
   fwrite(string.GetCString(),string.GetLength(),1,stream);
   fwrite(buffer.GetCString(),buffer.GetLength(),1,stream);
  }
  fclose(stream);
  return true;
 }
 return false;
}

void CStringList::Print(std::ostream& out)
{
 for (int i = 0; i < GetCount(); i++)
 {
  out<<GetString(i).GetCString()<<std::endl;
 }
}

CStringList& CStringList::operator =(const CString& AString)
{
 Clear();
 Insert(AString);
 return *this;
}

CStringList& CStringList::operator =(const CStringList& AStringList)
{
 Clear();
 Insert(AStringList);
 return *this;
}

CString& CStringList::operator [](const int Index)
{
 return GetString(Index);
}

CString CStringList::operator [](const int Index) const
{
 return GetString(Index);
}

CStringList& CStringList::operator <<(const CString& AString)
{
 Insert(AString);
 return *this;
}

//---------------------------------------------------------------------------

CCharset::CCharset(void)
{
 UpdateCharset();
}

CCharset::CCharset(const CCharset& Charset)
{
 SetAlphabet(Charset.GetAlphabet());
}

CCharset::CCharset(const CString& Alphabet)
{
 SetAlphabet(Alphabet);
}

CCharset::~CCharset(void)
{
}

void CCharset::UpdateCharset(void)
{
 for (int i = 0; i < CHARSET_SIZE; i++) m_Charset[i] = false;
 for (int i = 0; i < m_Alphabet.GetLength(); i++) m_Charset[(unsigned char)m_Alphabet[i]] = true;
}

void CCharset::UpdateAlphabet(void)
{
 m_Alphabet.SetLength(CHARSET_SIZE); int charset_size = 0;
 for (int i = 0; i < CHARSET_SIZE; i++)
 {
  if (m_Charset[i])
  {
   m_Alphabet[charset_size++] = char(i);
  }
 }
 m_Alphabet.SetLength(charset_size);
}

bool CCharset::Isset(const char AChar) const
{
 return m_Charset[*(unsigned char *)&AChar];
}

char CCharset::GetChar(const char AChar) const
{
 if (m_Charset[*(unsigned char *)&AChar]) return AChar; else return char(0);
}

void CCharset::SetChar(const char AChar)
{
 m_Charset[*(unsigned char *)&AChar] = true;
 UpdateAlphabet();
}

void CCharset::UnsetChar(const char AChar)
{
 m_Charset[*(unsigned char *)&AChar] = false;
 UpdateAlphabet();
}

CString CCharset::GetAlphabet(void) const
{
 return m_Alphabet;
}

void CCharset::SetAlphabet(const CString& Alphabet)
{
 m_Alphabet = Alphabet;
 UpdateCharset();
 /*UpdateAlphabet();*/
}

void CCharset::Print(std::ostream& out)
{
 out<<m_Alphabet.GetCString();
}

//------------------------------------------------------------------------------

bool CCharIterator::Match(const CString& Pattern, const bool Move)
{
 (void)Move;
 bool result = true;
 for (int i = 0; i < Pattern.GetLength(); i++)
 {
  if (This()!=Pattern[i]) return false;
  Next();
 }
 return result;
}

//------------------------------------------------------------------------------

CStringIterator::CStringIterator(void)
{
 m_String = NULL;
 m_Position = 0;
}

CStringIterator::CStringIterator(const CString *AString)
{
 m_String = (CString *)AString;
 m_Position = 0;
}

CStringIterator::CStringIterator(const CStringIterator& AStringIterator) :
  CCharIterator()
{
  (void)AStringIterator;
}

int CStringIterator::FirstPosition(void)
{
 return 0;
}

int CStringIterator::ThisPosition(void)
{
 return m_Position;
}

int CStringIterator::LastPosition(void)
{
 return m_String->GetLength()-1;
}

char& CStringIterator::First(void)
{
 m_Position = FirstPosition();
 return This();
}

char& CStringIterator::Prev(void)
{
 if (m_Position > 0) m_Position--;
 return This();
}

char& CStringIterator::This(void)
{
 return (*m_String)[m_Position];
}

char& CStringIterator::Next(void)
{
 if (m_Position < (int)m_String->GetLength()) m_Position++;
 return This();
}

char& CStringIterator::Last(void)
{
 m_Position = LastPosition();
 return This();
}

void CStringIterator::Reset(void)
{
 First();
}

bool CStringIterator::Match(const CString& Pattern, const bool Move)
{
 int position = m_Position;
 bool result = CCharIterator::Match(Pattern,false);
 if (!(result && Move)) m_Position = position;
 return result;
}

//------------------------------------------------------------------------------

CStringListIterator::CStringListIterator(void)
{
 m_StringList = NULL;
 m_Line = 0;
 m_Position = 0;
}

CStringListIterator::CStringListIterator(const CStringList *AStringList)
{
 m_StringList = (CStringList *)AStringList;
}

CStringListIterator::CStringListIterator(const CStringListIterator& AStringListIterator) :
  CCharIterator()
{
 m_StringList = AStringListIterator.m_StringList;
 m_Line = AStringListIterator.m_Line;
 m_Position = AStringListIterator.m_Position;
}

int CStringListIterator::FirstPosition(void)
{
 return 0;
}

int CStringListIterator::ThisPosition(void)
{
 return m_Position;
}

int CStringListIterator::LastPosition(void)
{
 if (NULL==m_StringList) return m_NullChar;
 return m_StringList->GetLength() + m_StringList->GetCount(); // length of all strings + null chars
}

char& CStringListIterator::First(void)
{
 if (NULL==m_StringList) return m_NullChar;
 m_Line = 0;
 m_Position = 0;
 return This();
}

char& CStringListIterator::Prev(void)
{
 if (NULL==m_StringList) return m_NullChar;
 if (m_Position > 0) { m_Position--; return This(); }
 else
 {
  if (m_Line > 0)
  {
   m_Line--;
   m_Position = std::max(0,m_StringList->GetString(m_Line).GetLength()-1);
 }}
 return This();
}

char& CStringListIterator::This(void)
{
 if (NULL==m_StringList) return m_NullChar;
 return m_StringList->GetString(m_Line)[m_Position];
}

char& CStringListIterator::Next(void)
{
 if (NULL==m_StringList) return m_NullChar;
 if (m_Position < (int)m_StringList->GetString(m_Line).GetLength()) { m_Position++; return This(); }
 else
 {
  if (m_Line < (m_StringList->GetCount()-1))
  {
   m_Line++;
   m_Position = 0;
 }}
 return This();
}

char& CStringListIterator::Last(void)
{
 if (NULL==m_StringList) return m_NullChar;
 m_Line = m_StringList->GetCount()-1;
 m_Position = m_StringList->GetString(m_Line).GetLength();
 return This();
}

void CStringListIterator::Reset(void)
{
 First();
}

bool CStringListIterator::Match(const CString& Pattern, const bool Move)
{
 int line = m_Line;
 int position = m_Position;
 bool result = CCharIterator::Match(Pattern,false);
 if (!(result && Move))
 {
  m_Line = line;
  m_Position = position;
 }
 return result;
}

//------------------------------------------------------------------------------

CCharHistogram::CCharHistogram(void)
{
 Reset();
}

CCharHistogram::CCharHistogram(const CCharHistogram& AHistogram)
{
 (void)AHistogram;
 Reset();
}

CCharHistogram::~CCharHistogram(void)
{
 // do nothing //
}

void CCharHistogram::Analyze(void)
{
 m_Flags = 0;
 CCharset numeric_charset(ALPHABET_NUMBER);
 CCharset realnum_charset(ALPHABET_REAL_NUMBER);
 CCharset integer_charset(ALPHABET_INTEGER_NUMBER);
 CCharset textnum_charset(ALPHABET_REAL_NUMBER+ALPHABET_TAB_CHAR+" "+ALPHABET_CR_CHAR+ALPHABET_LF_CHAR);
 unsigned int total_chars = 0, ascii_chars = 0, textnum_chars = 0,
              realnum_chars = 0, integer_chars = 0, numeric_chars = 0;
 for (int i = 0; i < CHARSET_SIZE; i++)
 {
  unsigned int _CharCount = m_Histogram[i];
  total_chars += _CharCount;
  if (i<0x80) ascii_chars += _CharCount;
  if (textnum_charset.Isset(char(i)))
  {
   textnum_chars += _CharCount;
   if (realnum_charset.Isset(char(i)))
   {
    realnum_chars += _CharCount;
    if (integer_charset.Isset(char(i)))
    {
     integer_chars += _CharCount;
     if (numeric_charset.Isset(char(i))) numeric_chars += _CharCount;
 }}}}
 // if there are characters with codes >= 0x80 then content is not an ascii text
 if (total_chars>ascii_chars) m_Flags |= FLAG_CUSTOM_BINARY;
 else
 {
  // if there are letters other than exponent specifiers
  if (total_chars>textnum_chars) m_Flags |= FLAG_ASCII_TEXT;
  else
  {
   // if there is at least one digit character
   if (numeric_chars>0)
   {
    if (realnum_chars>integer_chars) m_Flags |= FLAG_PURE_NUMERIC;
    else m_Flags |= FLAG_PURE_INTEGER;
   }
   else m_Flags |= FLAG_ASCII_TEXT;
  }
 }
}

void CCharHistogram::Reset(void)
{
 m_Flags = 0; for (int i = 0; i < CHARSET_SIZE; i++) m_Histogram[i] = 0;
}

void CCharHistogram::Insert(const char AChar)
{
 if (m_Histogram[(unsigned char)AChar]<MAX_CHAR_FREQUENCY) m_Histogram[(unsigned char)AChar]++;
 m_Flags = 0;
}

void CCharHistogram::Insert(const CString& AString)
{
 for (int i = 0, n = AString.GetLength(); i < n; i++)
 {
  char c = AString[i];
  if (m_Histogram[(unsigned char)c]<MAX_CHAR_FREQUENCY) m_Histogram[(unsigned char)c]++;
 }
 m_Flags = 0;
}

void CCharHistogram::Insert(const CStringList& AStringList)
{
 for (int j = 0, nj = AStringList.GetCount(); j < nj; j++)
 {
  CString s = AStringList.GetString(j);
  for (int i = 0, ni = s.GetLength(); i < ni; i++)
  {
   char c = s[i];
   if (m_Histogram[(unsigned char)c]<MAX_CHAR_FREQUENCY) m_Histogram[(unsigned char)c]++;
 }}
 m_Flags = 0;
}

void CCharHistogram::Remove(const char AChar)
{
 if (m_Histogram[(unsigned char)AChar]>0) m_Histogram[(unsigned char)AChar]--;
 m_Flags = 0;
}

void CCharHistogram::Remove(const CString& AString)
{
 for (int i = 0, n = AString.GetLength(); i < n; i++)
 {
  char c = AString[i];
  if (m_Histogram[(unsigned char)c]>0) m_Histogram[(unsigned char)c]--;
 }
 m_Flags = 0;
}

void CCharHistogram::Remove(const CStringList& AStringList)
{
 for (int j = 0, nj = AStringList.GetCount(); j < nj; j++)
 {
  CString s = AStringList.GetString(j);
  for (int i = 0, ni = s.GetLength(); i < ni; i++)
  {
   char c = s[i];
   if (m_Histogram[(unsigned char)c]>0) m_Histogram[(unsigned char)c]--;
 }}
 m_Flags = 0;
}

bool CCharHistogram::IsPureNumeric(void)
{
 if (0==m_Flags) Analyze();
 return ((m_Flags&FLAG_PURE_NUMERIC)!=0);
}

bool CCharHistogram::IsPureInteger(void)
{
 if (0==m_Flags) Analyze();
 return ((m_Flags&FLAG_PURE_INTEGER)!=0);
}

bool CCharHistogram::IsAsciiText(void)
{
 if (0==m_Flags) Analyze();
 return ((m_Flags&FLAG_ASCII_TEXT)!=0);
}

bool CCharHistogram::IsCustomBinary(void)
{
 if (0==m_Flags) Analyze();
 return ((m_Flags&FLAG_CUSTOM_BINARY)!=0);
}

unsigned int CCharHistogram::GetAt(const char AChar) const
{
 return m_Histogram[(unsigned char)AChar];
}

void CCharHistogram::SetAt(const char AChar, const unsigned int Frequency)
{
 m_Histogram[(unsigned char)AChar] = Frequency;
}

CString CCharHistogram::GetAlphabet(void) const
{
 CString result;
 for (int i = 0, n = CHARSET_SIZE; i < n; i++)
 {
  if (m_Histogram[i]>0) result.Append(char(i));
 }
 return result;
}

void CCharHistogram::Print(std::ostream& out)
{
 for (int i = 0, n = CHARSET_SIZE; i < n; i++)
 {
  out<<"char 0x"<<hex(i,2*sizeof(char)).GetCString();
  if (i>0x20) out<<char(i)<<" "; else out<<"  ";
  out<<" freq "<<m_Histogram[i]<<std::endl<<std::flush;
 }
}

//------------------------------------------------------------------------------

CString StringOfChars(const char AChar, const int Count)
{
 CString result;
 if (Count>0) result.SetLength(Count).Fill(AChar);
 return result;
}

CString FillStr(const CString& Prefix, const char AChar,
                const CString& Suffix, const int Length)
{
 CString filler = StringOfChars(AChar,Length-Prefix.GetLength()-Suffix.GetLength());
 CString result = Prefix+filler+Suffix;
 return result;
}

CString EnumStr(const CString& Prefix, const int Counter, const CString& Suffix, const unsigned int Length)
{
 CString result, lead_zero, counter = IntegerToString(Counter);
 unsigned int length = Prefix.GetLength()+counter.GetLength()+Suffix.GetLength();
 if (Length>length) { lead_zero.SetLength(Length-length); lead_zero.Fill('0'); }
 result = Prefix + lead_zero + counter + Suffix;
 return result;
}

CString SignStr(const int Signature)
{
 CString result; int s = Signature;
 for (size_t i = 0; i < sizeof(s); i++)
 {
  result+=char(s&0xff);
  s>>=8;
 }
 return result;
}

CString SubStr(const CString& AString, const int FirstPos, const int LastPos)
{
 CString result;
 if (FirstPos > LastPos) return result;
 int first_pos = std::max(std::min(FirstPos,AString.GetLength()-1),0);
 int last_pos = std::max(std::min(LastPos,AString.GetLength()-1),0);
 if (last_pos<first_pos) return result;
 result.SetLength(last_pos-first_pos+1);
 for (int i = first_pos, j = 0; i <= last_pos; i++, j++) { result[j] = AString.GetChar(i); }
 return result;
}

CString ShiftStr(const CString& AString, const int Shift)
{
 CString result;
 if (AString.GetLength()==0) return result;
 if (Shift>0) return SubStr(AString,Shift,AString.GetLength()-1);
 if (Shift<0) if (AString.GetLength()>(-Shift)) return SubStr(AString,-Shift,AString.GetLength()-Shift);
 return result;
}

CString LeftStr(const CString& AString, const int Position)
{
 return SubStr(AString,0,std::min(AString.GetLength(),Position));
}

CString RightStr(const CString& AString, const int Position)
{
 CString result;
 if (AString.GetLength() > 0)
 {
  result = SubStr(AString,Position,std::max(0,AString.GetLength()-1));
 }
 return result;
}

void SplitStr(const CString& AString, const int Position, CString& LString, CString& RString)
{
 LString = LeftStr(AString,Position);
 RString = RightStr(AString,Position);
}

CString JoinStr(const CString& AString, const CString& BString, const char Separator)
{
 CString result = AString;
 if (!AString.IsEmpty() && !BString.IsEmpty())
 {
  if (BString.GetFirstChar()!=Separator)
  {
   result = CheckLastChar(result,Separator);
  }
 }
 result.Append(BString);
 //std::cout<<"join('"<<AString.GetCString()<<"','"<<BString.GetCString()<<"','"<<Separator<<"')='"<<result.GetCString()<<"'"<<std::endl;
 return result;
}

CString FilterStr(const CString& AString, const CCharset& ValidChars, const CCharset& InvalidChars, const bool ValidateUndefined)
{
 CString result(AString);
 int length = 0;
 for (int i = 0; i < AString.GetLength(); i++)
 {
  if (InvalidChars.Isset(result[i])) continue;
  if (ValidChars.Isset(result[i]) || ValidateUndefined) result[length++] = result[i];
 }
 result.SetLength(length);
 return result;
}

int CountChars(const CString& AString, const char AChar)
{
 int result = 0;
 for (int i = 0; i < AString.GetLength(); i++) if (AChar==AString.GetChar(i)) result++;
 return result;
}

int NextCharPos(const CString& AString, const char AChar, const int Position)
{
 for (int i = Position; i < AString.GetLength(); i++) if (AChar==AString.GetChar(i)) return i;
 return INVALID_INDEX;
}

int PrevCharPos(const CString& AString, const char AChar, const int Position)
{
 for (int i = Position; i > 0; i--)
 {
  if (AChar==AString.GetChar(i)) return i;
 }
 return INVALID_INDEX;
}

int FirstCharPos(const CString& AString, const char AChar)
{
 return NextCharPos(AString,AChar,0);
}

int LastCharPos(const CString& AString, const char AChar)
{
 if (AString.GetLength() > 0)
 {
  return PrevCharPos(AString,AChar,AString.GetLength()-1);
 }
 else return INVALID_INDEX;
}

CString TrimStr(const CString& AString)
{
 int first_pos = 0, last_pos = AString.GetLength()-1;
 for (int i = first_pos; i <= last_pos; i++)
 { if (' '==AString.GetChar(i)) first_pos++; else break; }
 for (int i = last_pos; i > first_pos; i--)
 { if (' '==AString.GetChar(i)) last_pos--; else break; }
 return SubStr(AString,first_pos,last_pos);
}

CString LeadStr(const CString& AString, const unsigned int RLength)
{
 CString result;
 result.SetLength(std::max(0,(int)RLength-(int)AString.GetLength()));
 result.Fill(' ');
 result.Append(AString);
 return result;
}

void ParseStr(const CString& AString, const CString& Delimiters, CStringList& SubStrings)
{
 if (Delimiters.GetLength()==0) { SubStrings.Insert(AString); return; }
 if (AString.GetLength()==0) { return; }
 int str_pos = 0, delim_pos = 0;
 CString sub_str;
 while (str_pos<AString.GetLength())
 {
  if (AString.GetChar(str_pos)==Delimiters.GetChar(delim_pos))
  {
   SubStrings.Insert(sub_str);
   sub_str.SetLength(0);
   if (++delim_pos>=Delimiters.GetLength())
   {
    sub_str = SubStr(AString,++str_pos,AString.GetLength()-1);
    break;
  }}
  else sub_str.Append(AString.GetChar(str_pos));
  str_pos++;
 }
 SubStrings.Insert(sub_str);
}

void ParseStr(const CString& AString, const char Delimiter, CStringList& SubStrings)
{
 CString _Delimiters;
 _Delimiters.SetLength(CountChars(AString,Delimiter));
 _Delimiters.Fill(Delimiter);
 ParseStr(AString,_Delimiters,SubStrings);
}

CString CollectStr(const CStringList& SubStrings, const CString& Delimiters)
{
 CString result;
 int delim_pos = 0; int sub_strIdx = 0;
 bool delim_set, sub_str_set;
 do
 {
  sub_str_set = (sub_strIdx<SubStrings.GetCount());
  delim_set = (delim_pos<Delimiters.GetLength());
  if (sub_str_set) result.Append(SubStrings.GetString(sub_strIdx++));
  if (delim_set) result.Append(Delimiters.GetChar(delim_pos++));
 }
 while (delim_set||sub_str_set);
 return result;
}

CString StripDupChars(const CString& AString, const char AChar)
{
 CString result(AString);
 int length = 0;
 for (int i = 0; i < AString.GetLength(); i++)
 {
  if ((AChar!=AString.GetChar(i))||(AChar!=AString.GetChar(i+1))) result[length++] = result[i];
 }
 result.SetLength(length);
 return result;
}

CString StripDupChars(const CString& AString)
{
 CString result(AString);
 int length = 0;
 for (int i = 0; i < AString.GetLength(); i++)
 {
  if (AString.GetChar(i)!=AString.GetChar(i+1)) result[length++] = result[i];
 }
 result.SetLength(length);
 return result;
}

CString CheckFirstChar(const CString& AString, const char AChar)
{
 CString result;
 if (AChar!=AString.GetFirstChar()) result = CString(AChar) + AString; else result = AString;
 return result;
}

CString CheckLastChar(const CString& AString, const char AChar)
{
 CString result;
 if (AChar!=AString.GetLastChar()) result = AString + AChar; else result = AString;
 return result;
}

int FindStr(const CString& AString, const CString& BString)
{
 int result = INVALID_INDEX;
 for (int i = 0, n = ((int)AString.GetLength() - (int)BString.GetLength()); i <= n; i++)
 {
  bool flag = true;
  for (int j = 0, k = i, m = BString.GetLength(); j < m; j++, k++)
  {
   if (AString[k]!=BString[j]) { flag = false; break; }
  }
  if (flag) { result = i; break; }
 }
 return result;
}

CString FindReplaceChar(const CString& AString,
 const char SearchChar, const char ReplaceChar)
{
 CString result = AString;
 for (int i = 0, n = result.GetLength(); i < n; i++)
 {
  if (SearchChar==result[i]) result[i] = ReplaceChar;
 }
 return result;
}

CString FindReplaceStr(const CString& AString,
 const CString& SearchString, const CString& ReplaceString)
{
 CString result = AString;
 do
 {
  int pos = FindStr(result,SearchString);
  if (INVALID_INDEX == pos) break;
  result = LeftStr(result,pos-1)+ReplaceString+RightStr(result,pos+SearchString.GetLength());
 }
 while (true);
 return result;
}

int ParseMemSizeStr(const CString& AString, int& UnitSize, char& UnitChar)
{
 CCharset invalid_chars;
 CCharset valid_number_chars(ALPHABET_NUMBER);
 CCharset valid_suffix_chars("bkmgBKMG");
 CString number_str = FilterStr(AString,valid_number_chars,invalid_chars,false);
 CString suffix_str = FilterStr(AString,valid_suffix_chars,invalid_chars,false);
 valid_suffix_chars.SetAlphabet(suffix_str[0]);
 int result = StringToInteger(number_str);
 UnitSize = 1; UnitChar = 'b';
 if (valid_suffix_chars.Isset('k')||valid_suffix_chars.Isset('K')) { UnitSize = 0x00000400; UnitChar = 'k'; };
 if (valid_suffix_chars.Isset('m')||valid_suffix_chars.Isset('M')) { UnitSize = 0x00010000; UnitChar = 'm'; };
 if (valid_suffix_chars.Isset('g')||valid_suffix_chars.Isset('G')) { UnitSize = 0x40000000; UnitChar = 'g'; };
 return result;
}

int ParseMemSizeStr(const CString& AString)
{
 int unit_size; char unit_char;
 int result = ParseMemSizeStr(AString,unit_size,unit_char);
 result *= unit_size;
 return result;
}

long long int ParseMemSizeStr64(const CString& AString, long long int& UnitSize, char& UnitChar)
{
 const long long int kb = 0x00000000400LL;
 const long long int mb = 0x00000100000LL;
 const long long int gb = 0x00040000000LL;
 const long long int tb = 0x10000000000LL;
 CCharset invalid_chars;
 CCharset valid_number_chars(ALPHABET_NUMBER);
 CCharset valid_suffix_chars("bkmgtBKMGT");
 CString number_str = FilterStr(AString,valid_number_chars,invalid_chars,false);
 CString suffix_str = FilterStr(AString,valid_suffix_chars,invalid_chars,false);
 valid_suffix_chars.SetAlphabet(suffix_str[0]);
 int result = StringToInteger(number_str);
 UnitSize = 1; UnitChar = 'b';
 if (valid_suffix_chars.Isset('k')||valid_suffix_chars.Isset('K')) { UnitSize = kb; UnitChar = 'K'; };
 if (valid_suffix_chars.Isset('m')||valid_suffix_chars.Isset('M')) { UnitSize = mb; UnitChar = 'M'; };
 if (valid_suffix_chars.Isset('g')||valid_suffix_chars.Isset('G')) { UnitSize = gb; UnitChar = 'G'; };
 if (valid_suffix_chars.Isset('t')||valid_suffix_chars.Isset('T')) { UnitSize = tb; UnitChar = 'T'; };
 return result;
}

long long int ParseMemSizeStr64(const CString& AString)
{
 long long int unit_size; char unit_char;
 long long int result = ParseMemSizeStr64(AString,unit_size,unit_char);
 result *= unit_size;
 return result;
}

int GuessStr(const CString& AString, const CStringList& Variants, CString& Guessed, const bool TryFuzzy)
{
 CString source = AString;
 Guessed = "";
 // try to match variants exactly
 for (int i = 0, n = Variants.GetCount(); i < n; i++)
 {
  if (Variants[i]==source)
  {
   Guessed = Variants[i];
   return i;
  }
 }
 if (!TryFuzzy) return INVALID_INDEX;
 // if none succeed, try to match fuzzy
 int result = -1; unsigned int max_match = 0;
 CCharHistogram freq_a,freq_v;
 freq_a.Insert(source);
 for (int i = 0, n = Variants.GetCount(); i < n; i++)
 {
  freq_v.Reset(); freq_v.Insert(Variants[i]);
  unsigned int match = 0;
  for (int j = 0; j < CHARSET_SIZE; j++) { match += freq_a.GetAt(j)*freq_v.GetAt(j); }
  if ((match>0)&&(match==max_match)) return INVALID_INDEX;
  if (match>max_match) { result = i; max_match = match; }
 }
 if (result>=0) Guessed = Variants[result];
 // if none succeed, return -1
 return result;
}

int GuessStr(const CString& AString, const CString& Variants, CString& Guessed, const bool TryFuzzy)
{
 CStringList _Variants;
 ParseStr(Variants,' ',_Variants);
 return GuessStr(AString,_Variants,Guessed,TryFuzzy);
}

CString VariantStr(const int Index, const CString& Variants, const char Delimiter)
{
 CStringList variants;
 ParseStr(Variants,Delimiter,variants);
 CString result = variants.GetString(Index);
 return result;
}

CString MemSizeStr(const long long int Value, long long int& UnitSize, CString& UnitSuffix)
{
 const long long int kb = 0x00000000400LL;
 const long long int mb = 0x00000100000LL;
 const long long int gb = 0x00040000000LL;
 const long long int tb = 0x10000000000LL;
 const CString format = "%lg";
 if (Value < kb)
 {
  UnitSize = 1; UnitSuffix = "b";
  return Int64ToString(Value);
 }
 else if (Value < mb)
 {
  UnitSize = kb; UnitSuffix = "Kb";
  return FloatToString((double)Value/(double)kb,format);
 }
 else if (Value < gb)
 {
  UnitSize = mb; UnitSuffix = "Mb";
  return FloatToString((double)Value/(double)mb,format);
 }
 else if (Value < tb)
 {
  UnitSize = gb; UnitSuffix = "Gb";
  return FloatToString((double)Value/(double)gb,format);
 }
 else
 {
  UnitSize = tb; UnitSuffix = "Tb";
  return FloatToString((double)Value/(double)tb,format);
 }
 return "";
}

CString MemSizeStr(const long long int Value)
{
 long long int unit_size; CString unit_suffix;
 return MemSizeStr(Value,unit_size,unit_suffix).Append(" ").Append(unit_suffix);
}

CString SI_UnitStr(const double Value, double& UnitSize, CString& UnitPrefix, CString& UnitSymbol)
{
 int m = (int)floor(log10(Value)); CString format = "%lg";
 if (m<0)
 {
  if (m<=-24) { UnitSize = 1e-24; UnitPrefix = "yocto"; UnitSymbol = "y"; return FloatToString(Value/UnitSize,format); }
  else if (m<=-21) { UnitSize = 1e-21; UnitPrefix = "zepto"; UnitSymbol = "z"; return FloatToString(Value/UnitSize,format); }
  else if (m<=-18) { UnitSize = 1e-18; UnitPrefix = "atto"; UnitSymbol = "a"; return FloatToString(Value/UnitSize,format); }
  else if (m<=-15) { UnitSize = 1e-15; UnitPrefix = "femto"; UnitSymbol = format; return FloatToString(Value/UnitSize,format); }
  else if (m<=-9) { UnitSize = 1e-9; UnitPrefix = "pico"; UnitSymbol = "p"; return FloatToString(Value/UnitSize,format); }
  else if (m<=-6) { UnitSize = 1e-6; UnitPrefix = "nano"; UnitSymbol = "n"; return FloatToString(Value/UnitSize,format);  }
  else if (m<=-3) { UnitSize = 1e-3; UnitPrefix = "micro"; UnitSymbol = "mu"; return FloatToString(Value/UnitSize,format);  }
  else if (m<=-2) { UnitSize = 1e-2; UnitPrefix = "milli"; UnitSymbol = "m"; return FloatToString(Value/UnitSize,format); }
  else if (m<=-1) { UnitSize = 1e-1; UnitPrefix = "deci"; UnitSymbol = "d"; return FloatToString(Value/UnitSize,format); }
 }
 if (m>0)
 {
  if (m>=24) { UnitSize = 1e24; UnitPrefix = "yotta"; UnitSymbol = "Y"; return FloatToString(Value/UnitSize,format); }
  else if (m>=21) { UnitSize = 1e21; UnitPrefix = "zetta"; UnitSymbol = "Z"; return FloatToString(Value/UnitSize,format); }
  else if (m>=18) { UnitSize = 1e18; UnitPrefix = "exa"; UnitSymbol = "E"; return FloatToString(Value/UnitSize,format); }
  else if (m>=15) { UnitSize = 1e15; UnitPrefix = "peta"; UnitSymbol = "P"; return FloatToString(Value/UnitSize,format); }
  else if (m>=12) { UnitSize = 1e12; UnitPrefix = "tera"; UnitSymbol = "T"; return FloatToString(Value/UnitSize,format); }
  else if (m>=9) { UnitSize = 1e9; UnitPrefix = "giga"; UnitSymbol = "G"; return FloatToString(Value/UnitSize,format); }
  else if (m>=6) { UnitSize = 1e6; UnitPrefix = "mega"; UnitSymbol = "M"; return FloatToString(Value/UnitSize,format); }
  else if (m>=3) { UnitSize = 1e3; UnitPrefix = "kilo"; UnitSymbol = "k"; return FloatToString(Value/UnitSize,format); }
  else if (m>=2) { UnitSize = 1e2; UnitPrefix = "hecto"; UnitSymbol = "h"; return FloatToString(Value/UnitSize,format); }
  else if (m>=1) { UnitSize = 1e1; UnitPrefix = "deca"; UnitSymbol = "da"; return FloatToString(Value/UnitSize,format); }
 }
 UnitSize = 1; UnitPrefix = ""; UnitSymbol = ""; return FloatToString(Value,format);
}

CString SI_UnitStr(const double Value)
{
 double unit_size; CString unit_prefix; CString unit_symbol;
 return SI_UnitStr(Value,unit_size,unit_prefix,unit_symbol).Append(" ").Append(unit_symbol);
}

bool IsQuoted(const CString& AString)
{
 return ((AString.GetLength()>=2)&&(AString.GetFirstChar()=='\"')&&(AString.GetLastChar()=='\"'));
}

CString QuoteStr(const CString& AString)
{
 if (!IsQuoted(AString)) return "\""+AString+"\"";
 else return AString;
}

CString UnquoteStr(const CString& AString)
{
 if (IsQuoted(AString)) return SubStr(AString,1,AString.GetLength()-2);
 else return AString;
}

CString ShieldSpaces(const CString& AString)
{
 CString result; bool shield = false;
 for (int i = 0, n = AString.GetLength(); i < n; i++)
 {
  if (shield)
  {
   shield = false;
   result.Append(AString[i]);
  }
  else
  {
   if ('\\'==AString[i]) shield = true;
   else if (' '==AString[i])
   {
    result.Append('\\');
   }
   result.Append(AString[i]);
  }
 }
 return result;
}

CString UnshieldSpaces(const CString& AString)
{
 CString result; bool shield = false;
 for (int i = 0, n = AString.GetLength(); i < n; i++)
 {
  if (shield)
  {
   shield = false;
   if (' '!=AString[i]) result.Append('\\');
   result.Append(AString[i]);
  }
  else
  {
   if ('\\'==AString[i]) shield = true;
   else result.Append(AString[i]);
  }
 }
 if (shield) result.Append('\\');
 return result;
}

CString ShieldChars(const CString& AString, const CString& Chars)
{
 if (Chars.IsEmpty()) return AString;
 CCharset esc_chars(Chars);
 CString result; bool shield = false;
 for (int i = 0, n = AString.GetLength(); i < n; i++)
 {
  if (shield)
  {
   shield = false;
   if (!esc_chars.Isset(AString[i]))
   {
    result.Append('\\');
   }
   result.Append(AString[i]);
  }
  else
  {
   if ('\\'==AString[i]) shield = true;
   else if (esc_chars.Isset(AString[i]))
   {
    result.Append('\\');
   }
   result.Append(AString[i]);
  }
 }
 return result;
}

CString UnshieldChars(const CString& AString, const CString& Chars)
{
 if (Chars.IsEmpty()) return AString;
 CCharset esc_chars(Chars);
 CString result; bool shield = false;
 for (int i = 0, n = AString.GetLength(); i < n; i++)
 {
  if (shield)
  {
   shield = false;
   if (!esc_chars.Isset(AString[i])) result.Append('\\');
   result.Append(AString[i]);
  }
  else
  {
   if ('\\'==AString[i]) shield = true;
   else result.Append(AString[i]);
  }
 }
 if (shield) result.Append('\\');
 return result;
}

//------------------------------------------------------------------------------
