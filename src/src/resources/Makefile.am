nobase_pkgdata_DATA = resources.zip \
				start_here.zip \
				icons/app_64.xpm \
				icons/app.xpm \
				images/*.png \
				images/16x16/*.png

CLEANFILES = $(nobase_pkgdata_DATA)

resources.zip:
	zip  resources.zip *.xrc > /dev/null
	zip -0 resources.zip images/*.png images/16x16/*.png > /dev/null

start_here.zip:
	zip -j9 start_here.zip start_here/* > /dev/null
