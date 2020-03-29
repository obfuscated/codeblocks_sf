// discussion about this issue can be found here:
// http://forums.codeblocks.org/index.php/topic,19791.msg137040.html#msg137040

#define wxCONCAT_HELPER(text, line) text ## line

#define wxT(x) wxCONCAT_HELPER(L, x)

#define _T(x) wxT(x)

wxString illegal(_T(" -:.\"\'$&()[]<>+#"));

int aaa;


//a //aaa