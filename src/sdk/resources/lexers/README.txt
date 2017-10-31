The files in this folder define syntax highlighting for various languages.
Any XML file added here, defining the syntax highlighting of a language,
will be automattically added in C::B.

Look at the existing files. Their syntax is quite straightforward.
The only thing that needs clarification is the "index" attribute.

In the <Lexer> element, the "index" attribute denotes the index of the
scintilla lexer to be used. These are the wxSTC_LEX_* macros in stc.h
(don't use macros - use the index number).

In the <Style> element, the "index" attribute is the style index. It's the
wxSTC_[lang]_* (e.g. for c/cpp it's wxSTC_C_*). You can use more than one
index (comma separated) in a style. This groups many styles under a single
configuration option (like single line comments, multi-line comments, etc
under one "comments" option).

Yiannis Mandravellos.
