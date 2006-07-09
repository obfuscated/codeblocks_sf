if BUILD_ASTYLE
MAYBE_ASTYLE=astyle
endif

if BUILD_AUTOSAVE
MAYBE_AUTOSAVE=autosave
endif

if BUILD_CLASSWIZARD
MAYBE_CLASSWIZARD=classwizard
endif

if BUILD_CODECOMPLETION
MAYBE_CODECOMPLETION=codecompletion
endif

if BUILD_COMPILER
MAYBE_COMPILER=compilergcc
endif

if BUILD_DEBUGGER
MAYBE_DEBUGGER=debuggergdb
endif

if BUILD_MIMEHANDLER
MAYBE_MIMEHANDLER=defaultmimehandler
endif

if BUILD_PLUGINWIZARD
MAYBE_PLUGINWIZARD=pluginwizard
endif

if BUILD_PROJECTWIZARD
MAYBE_PROJECTWIZARD=projectwizard
endif

if BUILD_TODO
MAYBE_TODO=todo
endif

if BUILD_MANIFEST
MAYBE_MANIFEST=xpmanifest
endif

if BUILD_CONTRIB
MAYBE_CONSTRIB=contrib
endif

SUBDIRS=$(MAYBE_ASTYLE) \
		$(MAYBE_AUTOSAVE) \
		$(MAYBE_CLASSWIZARD) \
		$(MAYBE_CODECOMPLETION) \
		$(MAYBE_COMPILER) \
		$(MAYBE_DEBUGGER) \
		$(MAYBE_MIMEHANDLER) \
		$(MAYBE_PLUGINWIZARD) \
		$(MAYBE_PROJECTWIZARD) \
		$(MAYBE_TODO) \
		$(MAYBE_MANIFEST) \
		$(MAYBE_CONSTRIB)

DIST_SUBDIRS = astyle \
		autosave \
		classwizard \
		codecompletion \
		compilergcc \
		debuggergdb \
		defaultmimehandler \
		pluginwizard \
		projectwizard \
		todo \
		xpmanifest \
		contrib
