Debian package maintainers should create the sources with the
-i -I and -Idoc_tmp.* parameters to exclude unneeded svn-files
and temporary doc-files, otherwise the sources might become
really large.
If you want to build without docs (or have no internet-access
during the build), you can comment out the call
to "./updateDocs.sh" in the rules-script, and remove the entries
for the doc-packages and the build-dependency for "pngnq" and "wget" in
"debian/control".