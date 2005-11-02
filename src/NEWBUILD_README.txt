!!!       Please note that this project WILL NOT WORK WITH Code::Blocks RC2        !!!
!!! If you want to use this feature, you must first compile Code::Blocks from HEAD !!!


The file ''CodeBlocks-NewBuild.cbp'' is a modified copy of ''CodeBlocks-wx2.6.0.cbp''
which uses the new build environment with global user variables.

Global user variables implement the "compose once, build everywhere" paradigm. Instead of
requiring each and every user to modify compiler paths with every new project, a properly
made project will build everywhere, deriving the necessary path information from user-provided
global variables which only need to be set once.

Documentation on how to properly use this functionality in your own projects will come out soon.

To compile Code::Blocks, open the project, and you will be prompted to provide a value
for the variable WX. Enter the location of your wxWidgets installation in the field "value",
and confirm. You should now be able to compile Code::Blocks without ever having to bother again.
