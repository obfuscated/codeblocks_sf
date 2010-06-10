
Inorder to properly use wxPropertyGrid with wxWidgets 2.6.x, you
need to patch your library with reference-counted wxVariant. This
can be done easiest by copying variant.h and variant.cpp supplied
here (adapted from wxWidgets 2.8.3) to appropriate directories in
your wxWidgets 2.6.x source directory.

If you wish, you can adapt ref-counted wxVariant from another
version of wxWidgets. Doing following changes will probably
be enough:


  * variant.h

- Add #define wxUSE_VARIANT 1 to the top of the file.

- Add #define wxVARIANT_REFCOUNT 1 to the top of the file.


  * variant.cpp

- Add following at the top of the code:

  inline bool wxIsSameDouble(double x, double y) { return x == y; }


