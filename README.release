Preparations for a release:

1.) Make a release branch in SVN, e.g.
    http://svn.code.sf.net/p/codeblocks/code/branches/release-20.xx

2.) Adopt the RELEASE #define in:
    [C::B]\src\src\appglobals.h
    ...and remove the comment.

3.) Feel free to adopt or add a new logo in e.g.:
    [C::B]\src\src\resources\images\splash_20xx.png

4.) With respect to changes (if any) in the logo, adopt:
    [C::B]\src\src\splashscreen.cpp
    if needed.
    Especially have an eye on the positions of text overlays in:
    DrawReleaseInfo(wxDC& dc)
    Check if it works, also for SAFE-MODE (start C::B w/ safe-mode option).

5.) Make relevant changes in specific files, set new version number in:
    [C::B]\codeblocks.plist.in
    [C::B]\codeblocks.spec.in    <- careful here: don't change the changelog
    [C::B]\codeblocks.spec.fedora.in    <- careful here: don't change the changelog
    [C::B]\README
    [C::B]\update_revision.sh
    [C::B]\debian\get-orig-source
    [C::B]\debian\get-orig-source-from-svn
    [C::B]\src\build_tools\Info.plist
    [C::B]\src\src\resources\resources.rc
    [C::B]src\plugins\codecompletion\codecompletion.doxy
    [C::B]src\plugins\contrib\wxSmith\wxs.doxy
    [C::B]src\sdk.doxy

5.1.) Make sure the SVN_REVISION entry in revision.m4 includes only the version number
    and no svn revisions (contrary to what the name implies). This is required for
    proper work of make dist.

5.2.) Add a release entry in the debian change log, so the generated packages could
    have the proper version numbers.

5.3.) Remove .svn.%{_svnrev} from the version variables in the .spec.in files.

6.) Probably update year in footer of:
    [C::B]\src\src\resources\start_here\start_here.html

7.) Create nightlies based on that branch and fix bugs in that branch
    (No new features!)
    Make sure to merge from trunk or into trunk, if needed.

8.) After release, probably merge back relevant final changes from the
    branch into trunk excluding those related to release #defines and
    alike. (But including logos etc. if not yet done.)

9.) After release, rename the branch to the date C::B was actually released, e.g.:
    branches/release-20.xx -> branches/release-20.03
