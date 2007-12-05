AM_CPPFLAGS = -DWXMAKINGLIB_PROPGRID

INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/include/wxFlatNotebook/include \
		-I$(top_srcdir)/src/include/wxscintilla/include \
		-I$(top_srcdir)/src/include/wxFlatNotebook \
		-I$(top_srcdir)/src/plugins/contrib/wxSmith/propgrid/contrib/include

noinst_LTLIBRARIES = libwxsmith_wxwidgets_defitems.la

libwxsmith_wxwidgets_defitems_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libwxsmith_wxwidgets_defitems_la_LIBADD = ../../../../../sdk/libcodeblocks.la

libwxsmith_wxwidgets_defitems_la_SOURCES = \
./wxsdatepickerctrl.cpp \
./wxsstatictext.cpp \
./wxslistctrl.cpp \
./wxsflexgridsizer.cpp \
./wxsboxsizer.cpp \
./wxsgenericdirctrl.cpp \
./wxsstaticbitmap.cpp \
./wxscheckbox.cpp \
./wxschoice.cpp \
./wxsspinctrl.cpp \
./wxscalendarctrl.cpp \
./wxsstaticline.cpp \
./wxsgridsizer.cpp \
./wxsstaticboxsizer.cpp \
./wxsbitmapbutton.cpp \
./wxstextctrl.cpp \
./wxstogglebutton.cpp \
./wxsspacer.cpp \
./wxsscrollbar.cpp \
./wxsdialog.cpp \
./wxsspinbutton.cpp \
./wxsbutton.cpp \
./wxslistbox.cpp \
./wxsgauge.cpp \
./wxsradiobutton.cpp \
./wxspanel.cpp \
./wxsstaticbox.cpp \
./wxsradiobox.cpp \
./wxscombobox.cpp \
./wxschecklistbox.cpp \
./wxsslider.cpp \
./wxstreectrl.cpp \
./wxsframe.cpp \
./wxshtmlwindow.cpp \
./wxsscrolledwindow.cpp \
./wxsstddialogbuttonsizer.cpp \
./wxssplitterwindow.cpp \
./wxscustomwidget.cpp \
./wxsnotebook.cpp \
./wxschoicebook.cpp \
./wxslistbook.cpp \
./wxstimer.cpp \
./wxsmenubar.cpp \
./wxsmenu.cpp \
./wxsmenueditor.cpp \
./wxsmenuitem.cpp \
./wxsstatusbar.cpp \
./wxstoolbar.cpp \
./wxstoolbaritem.cpp \
./wxstoolbareditor.cpp \
./wxsgrid.cpp \
./wxscolourdialog.cpp \
./wxsdirdialog.cpp \
./wxsfiledialog.cpp \
./wxsmultichoicedialog.cpp \
./wxssinglechoicedialog.cpp

noinst_HEADERS = \
./wxsgauge.h \
./wxsradiobutton.h \
./wxsstaticline.h \
./wxsstaticbox.h \
./wxscombobox.h \
./wxstextctrl.h \
./wxstogglebutton.h \
./wxsspacer.h \
./wxsslider.h \
./wxschecklistbox.h \
./wxsscrollbar.h \
./wxstreectrl.h \
./wxsframe.h \
./wxsdatepickerctrl.h \
./wxsstatictext.h \
./wxsdialog.h \
./wxslistctrl.h \
./wxsspinbutton.h \
./wxsbutton.h \
./wxsstaticbitmap.h \
./wxscheckbox.h \
./wxschoice.h \
./wxsspinctrl.h \
./wxscalendarctrl.h \
./wxspanel.h \
./wxsgenericdirctrl.h \
./wxsgridsizer.h \
./wxsradiobox.h \
./wxsbitmapbutton.h \
./wxsstaticboxsizer.h \
./wxsboxsizer.h \
./wxsflexgridsizer.h \
./wxslistbox.h \
./wxshtmlwindow.h \
./wxsscrolledwindow.h \
./wxsstddialogbuttonsizer.h \
./wxssplitterwindow.h \
./wxscustomwidget.h \
./wxsnotebook.h \
./wxschoicebook.h \
./wxslistbook.h \
./wxstimer.h \
./wxsmenubar.h \
./wxsmenu.h \
./wxsmenueditor.h \
./wxsmenuitem.h \
./wxsstatusbar.h \
./wxstoolbar.h \
./wxstoolbaritem.h \
./wxstoolbareditor.h \
./wxsgrid.h \
./wxscolourdialog.h \
./wxsdirdialog.h \
./wxsfiledialog.h \
./wxsmultichoicedialog.h \
./wxssinglechoicedialog.h

