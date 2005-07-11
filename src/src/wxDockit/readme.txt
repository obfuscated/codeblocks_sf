This is the CVS module for wxDockIt, part of the wxExtended project
supporting extra modules for wxWidgets.

Code::Blocks README about wxDockit found here
---------------------------------------------
wxDockIt is a docking library for wxWidgets. The version included
currently in Code::Blocks is 2.0.1 with two changes which are documented
below.

Code::Blocks changes to wxDockIt-2.0.1
--------------------------------------
* Fixed bug in wxLayoutManager::UpdateAllHosts(). The wxEVT_LAYOUT_CHANGED
  event was fired before the UI was actually updated and some docked
  controls couldn't recalculate their size correctly, because of this.
  The event generation and dispatching was moved after the layout code.

* Replaced WXDLLEXPORT attribute with WXDOCKIT_DECLSPEC (defined in added
  file include/dockit_defs.h). Without this change, wxDockIt could not
  be built with the wxWidgets configuration Code::Blocks uses (i.e.
  wx DLL). With this change, the user chooses how to build wxDockIt.
  To build it as a DLL the preprocessor flag WXMAKING_DOCKIT_DLL must be
  defined. To use this DLL, WXUSING_DOCKIT_DLL must be defined.
  To build as a static library (like Code::Blocks does) no
  extra action needed.

The Code::Blocks Team.
