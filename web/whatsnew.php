<?php require_once('prepend.php'); ?>

<title>What's New in this release</title>
<h1>What's New in this release</h1>

<h2 id="improvements">Improvements</h2>

<p class="caption">Improvements from RC1-1 to RC2:</p>

<ul>
<li><b>Precompiled headers (PCH) support!</b> (currently works only with GCC)</li>
<li>The source is now fully UNICODE-compatible!</li>
<li>The source is also fully 64-bit compatible!</li>
<li>Autoconf/automake build system is now used for non-windows platforms.</li>
<li>New plugins:
  <ul>
    <li>Code statistics: counts nr. of source lines, nr. of comment lines, etc.</li>
    <li>CBProfiler: parses and displays the output of GProf, the GNU Profiler.</li>
    <li>SourceExporter: exports the active file to HTML/RTF/ODT.</li>
  </ul>
</li>
<li>New lexer (syntax highlighting) files:
  <ul>
    <li>Fortran77 (by Martin Halle)</li>
    <li>NVidia CG (by anonymous)</li>
    <li>X-Base languages (Clipper,  Foxpro)</li>
  </ul>
</li>
<li>New and improved project templates:
  <ul>
    <li>SDCC program</li>
    <li>Irrlicht 3D Graphics Engine</li>
    <li>Ogre 3D Graphics Engine</li>
    <li>GLFW project (OpenGL FrameWork)</li>
    <li>wxWidgets: selection between static/dynamic unicode/ansi wxWidgets version.</li>
  </ul>
</li>
<li>New editor functionality (by right-clicking the editor):
  <ul>
    <li>"Find declaration of [keyword]"</li>
    <li>"Open #include file: [file]"</li>
  </ul>
</li>
<li>New plugin type: cbProjectWizardPlugin to allow creation of custom new-project wizards.</li>
<li>New compiler supported: SDCC (Small Device C Compiler - http://sdcc.sourceforge.net/) for micro-processors. Contributed by Stanimir Jordanov.</li>
<li>Better handling of modified files outside the IDE: r/w->r/o, r/o->r/w, deletion</li>
<li>Single file (without project) compilation is now supported, for quick tests.</li>
<li>Code::Blocks now allows multiple files selection when adding link libraries.</li>
<li>BCC compiler is now working correctly for DLLs and static libraries.</li>
<li>Find-in-Files supports searching in arbitrary paths/filemasks (outside of project).</li>
<li>All Code::Blocks internal file formats (project, workspace, etc) are now valid XML.</li>
<li>Many new editor settings.</li>
<li>Many other bug-fixes.</li>
</ul>


<p class="caption">Improvements from RC1 to RC1-1:</p>

<ul>
<li>Unix Makefiles fixed</li>
<li>Close button added to close the active editor<br />
(added at request of our users but it's ugly, so it's disabled by default)</li>
<li>Fixed editor caret and focus line disappearing under linux</li>
<li>Fixed all dialogs to appear correctly under linux</li>
<li>Enabled opened files tree under linux</li>
<li>Revamped the design of the compiler settings to make it more user-friendly.</li>
<li>When pressing "Run" it checks if the project isn't built and displays a message</li>
<li>Enabled "Find" function for files without project</li>
<li>More MSVC importer updates</li>
<li>Code-completion enhancements</li>
<li>Editor background color can now be changed</li>
</ul>


<p class="caption">Improvements from finalbeta to RC1:</p>

<ul>
<li> Compiler support for OpenWatcom</li>
<li> Environment variables can be set per target/project or globally. Can be used anywhere in the build process (include dirs, link dirs, output filenames, etc).</li>
<li> Improved gcc warnings/errors parsing (thanks to tiwag).</li>
<li> Various configuration enhancements.</li>
<li> Auto-hide function for message pane.</li>
<li> Editors are now notebook pages (bye-bye MDI).</li>
<li> Convenient "Start here" page with links to common tasks.</li>
<li> Find in files :)</li>
<li> Ability to add files to project recursively.</li>
<li> External dependencies can be checked against additional files (specified by user), instead of only with target's output file (very usefull for commands-only targets).</li>
<li> Optional single-instance check.</li>
<li> Fixes in Dev-C++ project files import.</li>
<li> Fixes in MSVC project files import.</li>
<li> Editor smart as-you-type indentation (auto-indent when opening/closing brace).</li>
<li> Syntax highlighting support moved to files. Currently supported are:
  <ul>
    <li>C/C++</li>
    <li>Windows resources</li>
    <li>HTML/XML/XSL (by rickg22)</li>
    <li>Lua scripts</li>
    <li>GameMonkey scripts</li>
    <li>Hitach H8 ASM (by tiwag)</li>
  </ul>
  <p>User can add easily support for more languages (guide will be posted soon).</p>
</li>
<li> Many GDB debugger updates (most by kjliew):
   <ul>
    <li>GDB breakpoints on constructrs/destructors</li>
    <li>Step-into can start debugging session</li>
    <li>Step-out function to continue until the end of the current block {}.</li>
   </ul>
</li>
<li> Workspace tree now keeps its state after rebuilding.</li>
<li> Updated AStyle plugin to use astyle-1.17.0-dev (by ceniza).</li>
<li> Comment/uncomment/toggle are now 3 separate functions.</li>
<li> New project template for QT4.</li>
<li> New project template for Ogre3D.</li>
<li> Single file compile (not belonging to a project).</li>
<li> DevPaks support.</li>
</ul>

<h2 id="knownbugs">Known bugs and issues</h2>

<p class="caption">The following (unfixed) bugs have been reported for Release Candidate 2:</p>

<ul>
<li><b>Critical</b>: The SVN plugin doesn't work as expected if there's no svn.exe installed
on the machine. Please delete the svn.dll from the "share/codeblocks/plugins" directory if
you encounter any problems.</li>

<li><b>Critical</b>: Codeblocks crashes if you create a new file without extension ("Untitled1").
Unfortunately that's the default setting (another bug). Please delete the file
so you don't have to experience this problem.</li>

<li><b>Critical but rare</b>: If you delete a file from outside codeblocks, and then minimize, CodeBlocks
gets stuck in an eternal loop of dialog boxes, and then crashes depending on the
circumstances.<br />
<span class="caption">(All these bugs have been fixed in CVS)</span>.</li>

<li>MSVC importer still has some incompatibilities, like custom commands and
inter-project dependencies. We're currently working on it.</li>
</ul>