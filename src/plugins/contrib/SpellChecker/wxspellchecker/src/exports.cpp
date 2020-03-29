char** wxSpellChecker_GetSuggestions(const char*);
char** wxSpellChecker_GetSuggestions(const char* /*strMisspelledWord*/)
{
  return 0;
}

char* wxSpellChecker_SpellCheck(const char*);
char* wxSpellChecker_SpellCheck(const char* /*strWord*/)
{
  return 0;
}

int wxSpellChecker_IsValidWord(const char*);
int wxSpellChecker_IsValidWord(const char* /*strWord*/)
{
  return true;
}

void wxSpellChecker_AddWordToDictionary(const char*);
void wxSpellChecker_AddWordToDictionary(const char* /*strWord*/)
{
}

void wxSpellChecker_SetOption(const char*,               const char*);
void wxSpellChecker_SetOption(const char* /*strOption*/, const char* /*strValue*/)
{
}

char* wxSpellChecker_GetOption(const char*);
char* wxSpellChecker_GetOption(const char* /*strOption*/)
{
  return 0;
}
