#ifndef __WX_SPELL_CHECK_EXPORTS__
#define __WX_SPELL_CHECK_EXPORTS__

#ifdef __cplusplus
extern "C" {
#endif

char** wxSpellChecker_GetSuggestions(const char* strMisspelledWord);
char* wxSpellChecker_SpellCheck(const char* strWord);
int wxSpellChecker_IsValidWord(const char* strWord);
void wxSpellChecker_AddWordToDictionary(const char* strWord);
void wxSpellChecker_SetOption(const char* strOption, const char* strValue);
char* wxSpellChecker_GetOption(const char* strOption);

#ifdef __cplusplus
}
#endif

#endif // __WX_SPELL_CHECK_EXPORTS__