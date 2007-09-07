Requirements:
Installation of ImageMagick, inkscape, tex4ht (http://tug.org/applications/tex4ht/bugfixes.html) (for html-version of the document)
Installation of TeXLive 2007 with full installation scheme.

export TEXINPUTS=<directory>/mystyles:$TEXINPUTS

or copy files to <Tex-Install-directory>/texmf-local/tex/latex/mystyles

For html-version copy the files for tex4ht in <Tex-Installation-directory>/texmf-local
and update tex database using command

texhash <Tex-Install-directory>/texmf-local

If you have question regarding installation or generating the document 
please contact codeblocks@hightec-rt.com.

Generating documentation under linux or mingw shell:
pdf-version:
german version:

make pdf MAINFILE=main_codeblocks_de

english version:
make pdf MAINFILE=main_codeblocks_en

The output file will be generated in the directory output.

html-version:
Use the command
make html MAINFILE=main_codeblocks_en

html-version with sectioning:
make htmlsec MAINFILE=main_codeblocks_en

How to write a tex document:
Escape Character in Latex:

special character must be escaped within the text

\&, \$, \#, \_, \^{}, \~{} \}, \{, \%,

Sections:

\part{name} Add a part to the document
\chapter{name} Add a chapter to the document
\section{name} Add section to the document
\subsection{name} Add a subsection to the document
\genterm{name} Add a section entry without enumerating (e.g. for examples)

Labels:

\label{sec:name} labels of sections
\label{fig:name} labels of graphics. Use the as entry the name of the graphic.
\label{tab:name} label of tables
\label{eqn:name} label of equations
\label{lbl:name} label of hyperlinks within a document

Reference of labels:
\pxref{labelname} e.g. \pxref{sec:name}

Reference of urls:
\cite{url:name}. The file url_codeblocks.bbl contains the reference key.

Include graphics:

The label of graphics will be auto generated \label{fig:filename}. It can be reference within
the document with \pxref{fig:filename}. The search path of figures are figures/png, figures/eps, figures/pdf

\figures{filename without extension}{caption}

The following macro scales the graphic to 70% of the textwidth
\screenshot{filename without extension}{Caption}

Tables:

Put tables in own files under the directory tables. The are include with the macro:

\tables{filename}{caption}

This macro support long tables
\longtables{format}{filename}

Macros:
\codeblocks is a macro for the name Code::Blocks
\menu{item1, item2, ..., itemn} Formating for menu entries
\opt{name} Add formating for options
\optional{name} Specify optional parameters
\var{name} Variables that have to be substituded
\file{name} Formating file names
\samp{name} Add quote to name
\cmdline{name} Highlight command line entries.
\codeline{name} Highlight code within text.

Environments

For example cmdline output

\begin{cmd}
...
\end{cmd}

Include code snippets

\begin{code}
int main (void)
{
}
\end{code}

Include command files

\cmdinput{rel. path/filename}

Include source files

\codeinput{rel. path/filename}

Description:

\begin{description}
\item[item1] description
\item[item2] description
\end{description}

Itemize List:

\begin{itemize}
\item Option1
\item Option2
\end{itemize}

Enumeration:

\begin{enumerate}
\item first item
\item second item
\end{enumerate}

\begin{codeentry}
\item[.abort] Directive ...
\item[.dim] Directive ...
\end{codeentry}

\begin{optentry}
\item[--gstabs] option1 ...
\item[--help] option2 ...
\end{optentry}

Hyperlink to a document or url:

\href{name.pdf}{name in text}

This command will launch application for a file:

\launch{name der Datei}{name im Text}

Attach file in pdf document:

\attach{name der Datei}

Adding internal links:
\hyperlink{lbl:Text of anchor}{Name in text}

\hypertarget{lbl:Text of anchor}{Name in text}

Add hints:

\hint{Text}

