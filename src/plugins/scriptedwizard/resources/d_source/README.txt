
This is the d_source file wizard for the Code::Blocks IDE. This code 
implements a "D source" option in the "New File" dialog box that pops up
when you press Ctrl+N in the IDE.
 
Presently, the wizard.script sets the file to have a comment and license
block that is hard-coded. You can change it by editing the script 
(assign the copyright to yourself, please!). 
 
Sadly, the "default code" feature of the Settings > Editor menu is specific 
only to C and C++, and requires a recompile of the C::B sources to change.
If you'd like to help, see about making that code more flexible.

== INSTALLATION ==

Reference:  http://wiki.codeblocks.org/index.php?title=Wizard_scripts

=== Linux ===

In your $HOME directory: 

I'm not sure what the exact paths are for Linux home directory scripts. See below 
for Locating the $HOME scripts directory.

GOTO Registation

In your SYSTEM install directory:

Install to [CodeBlocks install dir]/share/CodeBlocks/templates/wizard/d_source.

GOTO Registration

=== Windows ===

In your $HOME directory:

Install to %APPDATA%\CodeBlocks\share\CodeBlocks\templates\wizard\d_source.

GOTO Registration

In your SYSTEM install directory:

Install to [CodeBlocks install dir]\share\CodeBlocks\templates\wizard\d_source .

GOTO Registration

=== Locating the $HOME Scripts Directory ===

Open CodeBlocks. Click on File > New > File. The "New from template" dialog box
appears. Right-click on "C/C++ Source" and select "Edit this script". 

Switch to the "wizard.script" tab that opened in the editor. Now click on File > 
"Save file as."

The editable wizard.script file has been copied to the $HOME scripts directory, under 
a subdirectory called c_file. When the Save As dialog appears, make a note of the 
path to the containing directory. That is your scripts directory. The d_source 
package should be installed beside the c_file directory.

Close the wizard.script file, discard the changes - we just wanted it to make the directory.

=== Registration ===

The wizard has to be registered. As above:

Open CodeBlocks. Click on File > New > File. The "New from template" dialog box
appears. Right-click on "C/C++ Source" and select "Edit global registration script."

Near the bottom of the RegisterWizards function is a comment "// file wizards". Copy
the line for "c_file", and change the copied line to read:

    RegisterWizard(wizFiles,       _T("d_source"),       _T("PL/D source"),          _T("Programming Language/D"));

You may change the 2nd and 3rd strings, if you like, but the first string (d_source) 
is a directory name - you will have to update the filesystem if you change that 
value.

Save the result, then stop and restart Code::Blocks.

=== Testing ===

Select File > New > File. The "New from template" dialog appears. The last item in the 
dialog should be the newly-installed PL/D source option. Selecting that option should 
invoke a new D source wizard.

