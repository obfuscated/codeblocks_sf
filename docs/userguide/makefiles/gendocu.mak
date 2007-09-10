## SHELL for linux
SHELL=/bin/sh

## IP address of gutenberg
SERVERIP = T:
TEXDISTRIBUTION = TeXLive
## Some variables for pdf generation
NUP = 2x2
FILE =
FILELIST =
## xsl file name without extension
XSLFILE =

## ENCODING
INENCODE = ISO-8859-1
OUTENCODE = UTF-8

## Conversion of graphic formats
INFORMAT =
OUTFORMAT =

## Templates in TEXMFLOCAL/tex/latex/mystyles
TEMPLATES = /opt/Tex/texmf-local
DOCUTEMPLATE = docu_template
BEAMERTEMPLATE = beamer_template

## user options
MAINFILE = $(shell ls main_*.tex | sed -es/\.tex//g)
INTERNET =
CUSTOMER =
PDF2PSOTPS =
PAGES = -
ORIENTATION =
# OUTPUT FILE FOR XML2OUT
OUT =

## User option and defines
metadata:
CVSTAG = $(shell grep "Name:"  $(MAINFILE).tex  | sed -es/%//g| sed -es/"\\\\RCS "//g | sed -es/"Name:"//g | sed -es/" "//g | sed -e 's/\$$//g')
RELEASE = $(shell echo $(CVSTAG) |  sed -es/".*\(V.*\)"/"\1"/g | sed -es/_/\./g)
MAINRELEASE = $(shell echo $(RELEASE) | sed -e"s/\(^V[0-9]*\.[0-9]*\.[0-9]*\).*/\1/" | sed -es/"V"//g)
RELEASEDATE =
TARGET = $(shell echo $(CVSTAG) | sed -es/".*_\([^_]*\)_V.*"/"\1"/g)
TARGET_LOWERCASE = $(shell echo $(TARGET) |  tr [A-Z] [a-z])
PREFIX = $(TARGET_LOWERCASE)
DOC_FULL_VERSION = $(shell echo $(CVSTAG) | sed -es/"\(.*\)_[^_]*_V.*"/"\1"/g | sed -es/" "//g)
DOC_MAIN_VERSION = $(shell echo $(DOC_FULL_VERSION) | sed -es/"\(V[0-9]*_[0-9]*\).*"/"\1"/g | sed -es/"_"/"\."/g | sed -es/"V"//g)
REVISION = $(shell echo $(DOC_FULL_VERSION) | grep "Rev" | sed -es/".*\(Rev_.\)"/"\1"/g | sed -es/"_"/" "/g)
LANGUAGE=$(shell grep "lang{" $(MAINFILE).tex | sed -es/"\\\\def\\\\lang{"//g | sed -es/"}"//g | tr [a-z] [A-Z])
MSG = default

NOCUSTOMER = $(SAMP)\def\customer{none}$(SAMP)
PXROS = PXROS

ifeq ("$(PREFIX)","arm")
PREFIX = arm-elf
MCU = ARM
endif

ifeq ("$(PREFIX)","arm-elf")
PREFIX = arm-elf
MCU = ARM
endif

ifeq ("$(PREFIX)","c16x")
MCU = C16x
endif

ifeq ("$(PREFIX)","elfi386")
MCU = I386
endif

ifeq ("$(PREFIX)","msp430")
MCU = MSP430
endif

ifeq ("$(PREFIX)","ppc")
MCU = PowerPC
endif

ifeq ("$(PREFIX)","tricorelinux")
MCU = TriCoreLinux
endif

ifeq ("$(PREFIX)","tricore")
MCU = TriCore
endif

## executables
TEX = tex
LATEX = latex
BIBTEX = bibtex
DVIPS = dvips
PDFLATEX = pdflatex
COMPILER = $(PDFLATEX)
TEX4HT = tex4ht
T4HT = t4ht
PSTOPS = pstops
PSBOOK = psbook
PSSELECT = psselect
PDFTOPS = pdftops

## Msys Tools
CP = cp
XCP = cp -r
MV = mv
RM = rm
RMF = rm -f
RD = rm -r
RDF = rm -rf
DIR = .
TESTDIR = test -d
TESTFILE = test -e
FIND = find . -name
FINDNOT = find . -not -name
LOGFILE  = $(MAINFILE).log
GREP = grep
CITWARN = "LaTeX Warning: Citation"
RERUN = "Rerun to get cross-references right"
SHELLPAUSE = read

ifeq ($(OSTYPE),linux)
SAMP = '
GS = gs
MAKEINDEX = splitindex.pl
INDEXOPT = -- -g -s mystyle.ist
TEX4HTENVFILE = unix/tex4ht.env
MKDIR = mkdir
MKDIRPARENT = $(MKDIR) -p
else
SAMP = "
GS = gswin32c
MAKEINDEX = splitindex.exe
INDEXOPT = -g -s mystyle.ist
TEX4HTENVFILE = win32/tex4ht.env
MKDIR = mkdir-linux
MKDIRPARENT = $(MKDIR) -p
endif

## Tex4ht parameter (HTML, XML, SXW)
XMLINDEX = $(SAMP)\def\filename{{$(MAINFILE)}{idx}{4dx}{ind}} \input idxmake.4ht$(SAMP)
TEX4HTCFGHTML = $(GUTENBERG)/$(TEXDISTRIBUTION)/configfiles/html.cfg
TEX4HTCFGXML = $(GUTENBERG)/$(TEXDISTRIBUTION)/configfiles/xml.cfg
TEX4HTCFGODT = $(GUTENBERG)/$(TEXDISTRIBUTION)/configfiles/odt.cfg
TEX4HTCFGBEAMERXML = $(GUTENBERG)/$(TEXDISTRIBUTION)/configfiles/beamerxml.cfg
TEX4HTENV = $(GUTENBERG)/$(TEXDISTRIBUTION)/texmf-local/tex4ht/base/$(TEX4HTENVFILE)
TEX4HTTFM = $(GUTENBERG)/$(TEXDISTRIBUTION)/texmf/fonts/tfm
TEX4HTFONTS = $(GUTENBERG)/$(TEXDISTRIBUTION)/texmf-local/tex4ht/ht-fonts
TEX4HTFONTSSYMBOL = $(TEX4HTFONTS)/symbol/!
TEX4HTFONTSUNICODE = $(TEX4HTFONTS)/unicode/!
TEX4HTFONTSMOZILLA = $(TEX4HTFONTS)/mozilla/!
TEX4HTFONTSISO = $(TEX4HTFONTS)/iso8859/1/!
TEX4HTFONTSALIAS = $(TEX4HTFONTS)/alias/!
XMLVALIDATE = -cvalidate
HTMLVALIDATE = -cvalidatehtml
##PNGCONVERT = -cdvipng
PNGCONVERT =
ifeq ($(LANGUAGE),GERMAN)
RTFC=/T/makefiles/rtfc.bat rtfc_de.cfg $(MAINFILE)
endif

ifeq ($(LANGUAGE),ENGLISH)
RTFC=/T/makefiles/rtfc.bat rtfc_en.cfg $(MAINFILE)
endif

## Scripts for conversion
HTMLSCRIPT=wlatex
XMLSCRIPT=dbmlatex
OOSCRIPT=oolatex
## Output directories
OUTPUT = output
BOOKLETOUTPUT = output
OPENOFFICEOUTPUT =  output
XMLOUTPUT = output
HTMLOUTPUT = output
PDFOUTPUT = output
BOOKLETOUTPUTF = $(BOOKLETOUTPUT)/$(MAINFILE)*.ps
XMLOUTPUTF = $(XMLOUTPUT)/$(MAINFILE)*.xml
HTMLOUTPUTF = $(HTMLOUTPUT)/$(MAINFILE)*.html
PDFOUTPUTF = $(PDFOUTPUT)/$(MAINFILE).pdf
OPENOFFICEOUTPUTF =  $(OPENOFFICEOUTPUT)/$(MAINFILE).odt
OUTPUTF =  $(OUTPUT)/$(MAINFILE)*.*
GRAPHICS_EPS = figures/eps
GRAPHICS_PNG = figures/png
GRAPHICS_SVG = figures/svg
SIGNATURE = 8

## Options ##
## List of changes
MAKECHANGE = makeindex -g -r -s scrguide.gst -o

## Dvi to ps options
DVIOPT = -P pdf -t A4

## Options for generation of ps to pdf
GSOPT = -dBATCH -dNOPAUSE -sDEVICE=pdfwrite -r300 \
	-dPDFSETTINGS=/prepress -dAutoFilterColorImages=false \
	-dAutoFilterGrayImages=false -dUseFlateCompression=false -sColorImageFilter=FlateEncode \
	-dColorImageResolution=96

## OPTIONS FOR BOOKLET
BOOKLETGSOPT = -dBATCH -dNOPAUSE -sDEVICE=pdfwrite -r720 -sPAPERSIZE=a4

PAPERSIZE = default
ifneq ($(PAPERSIZE),default)
	PAPEROPT = -sPAPERSIZE=$(PAPERSIZE)
endif

## Options for generating booklet
PSTOPSOPT = -pa4 $(SAMP)4:0@.7L(21cm,00)+1L@.7(21cm,14.8cm),2L@.7(21cm,00)+3L@.7(21cm,14.8cm)$(SAMP)
## Booklet options with pdflatex
SCALE = 0.95

## Options for tex4ht for html generation
HTMLFLAGS = $(SAMP)$(TEX4HTCFGHTML),html,2,sections+,info,next,refcaption,fn-in$(SAMP)
RTFCFLAGS = $(SAMP)$(TEX4HTCFGHTML),html,pic-tabular,pic-longtable$(SAMP)

## For $(TEXDISTRIBUTION) this option for inlcuding eps in pdf with use of package epstopdf
## For Miktex --enable-write18 is the corresponding option
EPSTOPDFOPT = --shell-escape

OPTS = 	$(SAMP)\def\customer{$(CUSTOMER)} \
	\def\mcu{$(PREFIX)} \
	\def\Mcu{$(MCU)} \
	\def\pxros{$(PXROS)} \
	\def\mykeywords{Target: $(MCU), Release: $(RELEASE), DocVersion: $(DOC_MAIN_VERSION), Revision: $(REVISION)} \
	\providecommand{\release}{$(MAINRELEASE)} \
	\def\releasevers{V$(MAINRELEASE)\xspace} \
	\def\DocVersion{$(DOC_MAIN_VERSION) $(REVISION)} $(SAMP)

## Targets
all: $(MAINFILE).dvi $(MAINFILE).ps $(MAINFILE)_BOOKLET.ps $(MAINFILE).pdf $(MAINFILE).html $(MAINFILE).xml clean

## Targets
## delivery: realclean pdf deliver
dvi: clean $(MAINFILE).dvi
ps: $(MAINFILE).ps
pdf: cleanpdf metadata $(MAINFILE).pdf clean
html: clean metadata $(MAINFILE).html cleanwww
xml: clean $(MAINFILE).xml cleanwww
sxw: $(MAINFILE).sxw cleanwww
odt: $(MAINFILE).sxw cleanwww
svg2png: $(SRC:%.svg=%.png)
svg2eps: $(SRC:%.svg=%.eps)
svg2pdf: $(SRC:%.svg=%.pdf)
png2eps: $(SRC:%.png=%.eps)

SRC	= $(shell ls *.png)

## Help screen
help:
	@echo "----------------------------------------------------------------"
	@echo MAKEFILE VARIABLES
	@echo "----------------------------------------------------------------"
	@echo MAINFILE= \
		"# Filename of main_xxx without extension #"
	@echo "----------------------------------------------------------------"
	@echo CUSTOMER= \
		"# Select customer (use capital letters) #"
	@echo "----------------------------------------------------------------"
	@echo TARGET= \
		"# mcu (tricore, ppc, msp430, arm, c16x, elfi386) #"
	@echo "----------------------------------------------------------------"
	@echo RELEASE= \
		"# Select RELEASE e.g. RELEASE=3.3.x #"
	@echo "----------------------------------------------------------------"
	@echo NUP= \
		"# Set column and rows of pdf document"
	@echo "----------------------------------------------------------------"
	@echo FILELIST= \
		"# Specify list of pdf documents that should be merged"
	@echo "----------------------------------------------------------------"
	@echo XSLFILE= \
		"# Stylesheet for converting XML-file to other formats"
	@echo "----------------------------------------------------------------"
	@echo OUT= \
		"# Specify name of the convertered XML file"
	@echo "----------------------------------------------------------------"
	@echo TARGETS
	@echo "----------------------------------------------------------------"
	@echo pdf, presentation: \
		"# Generate pdf output and put files in dir output #"
	@echo "----------------------------------------------------------------"
	@echo relnotes: \
		"# Generate release notes #"
	@echo "----------------------------------------------------------------"
	@echo dvi, ps, booklet, html, xml, chm \(windows only, requires rtfc\), sxw: \
		"# These output formats can be generated; for booklet use PAPERSIZE=a4 a5 etc.#"
	@echo "----------------------------------------------------------------"
	@echo clean, cleanpdf, realclean: \
		"# clean commands #"
	@echo "----------------------------------------------------------------"
	@echo psbooklet: \
		"# create booklet of a MAINFILE as postscript file #"
	@echo "----------------------------------------------------------------"
	@echo pdfbooklet: \
		"# create booklet of a MAINFILE from a pdf file #"
	@echo "----------------------------------------------------------------"
	@echo svg2png, svg2eps, svg2pdf, png2eps: \
		"# convert all svg files in current directory to specified output format #"
	@echo "----------------------------------------------------------------"
	@echo ps2pdf, eps2pdf: \
		"# convert ps, eps figures to MAINFILE pdf figures use PAPERSIZE=a4 a5 etc option #"
	@echo "----------------------------------------------------------------"
	@echo pdfmerge: \
		"# merge pdf files FILELIST="Name1.pdf, -, ..." #"
	@echo "----------------------------------------------------------------"
	@echo nup: \
		"# create pdf with NUP=rowsxcolumns a MAINFILE pdf file #"
	@echo "----------------------------------------------------------------"
	@echo texdirset: \
		"# setting texdir directories with SERVERIP=//IPADDRESS  #"
	@echo "----------------------------------------------------------------"
	@echo addpath: \
		"# set shell and paths for generating other formats xml, html etc.  #"
	@echo "----------------------------------------------------------------"
	@echo setshell: \
		"# set entry in path of shell #"
	@echo "----------------------------------------------------------------"
	@echo removeshell: \
		"# remove entry in path of shell #"
	@echo "----------------------------------------------------------------"
	@echo xml2out: \
		"# convert MAINFILE=xmlfile file with XSLFILE=name.xsl with saxon in OUT #"
	@echo "----------------------------------------------------------------"
	@echo mogrify \
		"# batch conversion of graphic formats OUTFORMAT=extension e.g. eps to INFORMAT=*.extension e.g. png#"
	@echo "----------------------------------------------------------------"
	@echo charencode \
		"# convert charencode FILE=name e.g. INENCODE=ISO-8859-1 to OUTENCODE=UTF-8; see iconv --help for available encoding#"

## init ##
init:
	$(MKDIRPARENT) figures/eps figures/png figures/svg && \
	$(MKDIRPARENT) tables && \
	$(MKDIRPARENT) $(BOOKLETOUTPUT) $(HTMLOUTPUT) $(XMLOUTPUT) $(OPENOFFICEOUTPUT) $(PDFOUTPUT) && \
	$(MKDIRPARENT) mcu/common/examples/input && \
	$(CP) $(TEMPLATES)/tex/latex/mystyles/$(DOCUTEMPLATE).tex main_$(DOCUTEMPLATE).tex && \
	$(CP) $(TEMPLATES)/tex/latex/mystyles/$(BEAMERTEMPLATE).tex main_$(BEAMERTEMPLATE).tex

presentation: realclean
ifneq ($(MSG), default)
	@echo $(MSG) && $(SHELLPAUSE)
endif
	$(PDFLATEX) $(EPSTOPDFOPT) $(OPTS) $(SAMP)\input $(MAINFILE).tex$(SAMP) && \
	$(PDFLATEX) $(EPSTOPDFOPT) $(OPTS) $(SAMP)\input $(MAINFILE).tex$(SAMP)
	if !($(TESTDIR) $(PDFOUTPUT)); then $(MKDIRPARENT) $(PDFOUTPUT); fi
	$(PDFLATEX) -jobname=$(MAINFILE)_1x2_handouts $(SAMP)\documentclass[a4]{scrbook} \usepackage[pdftex]{graphicx} \usepackage{pdfpages} \usepackage[pdftex]{hyperref} \begin{document} \includepdf[pages=-, delta=0 5mm, nup=1x2, scale=.95]{$(MAINFILE).pdf} \end{document}$(SAMP)
	$(PDFLATEX) -jobname=$(MAINFILE)_2x2_handouts $(SAMP)\documentclass[a4]{scrbook} \usepackage[pdftex]{graphicx} \usepackage{pdfpages} \usepackage[pdftex]{hyperref} \begin{document} \includepdf[pages=-, delta=5mm 5mm, landscape, nup=2x2, scale=.95]{$(MAINFILE).pdf} \end{document}$(SAMP)
	$(PDFLATEX) -jobname=$(MAINFILE)_article $(EPSTOPDFOPT) $(OPTS) $(SAMP)\def\myarticlemode{article} \input $(MAINFILE).tex$(SAMP)
	$(PDFLATEX) -jobname=$(MAINFILE)_article $(EPSTOPDFOPT) $(OPTS) $(SAMP)\def\myarticlemode{article} \input $(MAINFILE).tex$(SAMP)
	$(CP) $(MAINFILE).pdf $(PDFOUTPUT)/V$(RELEASE)_$(MAINFILE).pdf && \
	$(MV) --target-directory=$(PDFOUTPUT) $(MAINFILE)_1x2_handouts.pdf $(MAINFILE)_2x2_handouts.pdf $(MAINFILE)_article.pdf

$(MAINFILE).dvi: $(MAINFILE).tex
	$(LATEX) $(OPTS) $(SAMP)\input $(MAINFILE).tex$(SAMP) && \
	if ($(TESTFILE) $(MAINFILE).idx); then $(MAKEINDEX) $(MAINFILE).idx $(INDEXOPT); fi
	if ($(TESTFILE) $(MAINFILE).glo); then $(MAKECHANGE) $(MAINFILE).chn $(MAINFILE).glo; fi
	if ($(TESTFILE) $(MAINFILE).bib); then $(BIBTEX) $(MAINFILE); fi
	$(LATEX) $(OPTS) $(SAMP)\input $(MAINFILE).tex$(SAMP) && \
	$(LATEX) $(OPTS) $(SAMP)\input $(MAINFILE).tex$(SAMP)

$(MAINFILE).ps: $(MAINFILE).dvi
	echo "Running dvips"
	$(DVIPS) $(DVIOPT) $(MAINFILE)

psbooklet:
	$(PSBOOK) $(MAINFILE).ps | $(PSTOPS) $(PSTOPSOPT) > $(MAINFILE)_Book.ps && \
	$(PSSELECT) -o $(MAINFILE)_Book.ps $(MAINFILE)_Book1.ps && \
	$(PSSELECT) -e -r $(MAINFILE)_Book.ps $(MAINFILE)_Book2.ps
	$(GS) $(BOOKLETGSOPT) -sOutputFile=$(MAINFILE)_Book.pdf -c save pop -f $(MAINFILE)_Book.ps

pdfbooklet:
ifeq ($(COMPILER),$(PDFLATEX))
	$(PDFLATEX) -jobname=Booklet_$(MAINFILE) $(SAMP)\documentclass{scrbook} \usepackage[pdftex]{graphicx} \usepackage{pdfpages} \usepackage[pdftex]{hyperref} \begin{document} \includepdf[pages=-, signature=$(SIGNATURE), landscape, scale=$(SCALE)]{$(MAINFILE).pdf} \end{document}$(SAMP)
endif
ifeq ($(COMPILER),$(LATEX))
	$(PDFTOPS) $(PDF2PSOTPS) $(MAINFILE).pdf $(MAINFILE).ps
	$(PSBOOK) $(MAINFILE).ps | $(PSTOPS) $(PSTOPSOPT) > $(MAINFILE)_Book.ps && \
	$(PSSELECT) -o $(MAINFILE)_Book.ps $(MAINFILE)_Book1.ps && \
	$(PSSELECT) -e -r $(MAINFILE)_Book.ps $(MAINFILE)_Book2.ps
	$(GS) $(BOOKLETGSOPT) -sOutputFile=$(MAINFILE)_Book.pdf -c save pop -f $(MAINFILE)_Book.ps
endif

ps2pdf:
	$(GS) $(BOOKLETGSOPT) $(PAPEROPT) -sOutputFile=$(MAINFILE).pdf -c save pop -f $(MAINFILE).ps

eps2pdf:
	$(GS) $(BOOKLETGSOPT) -dEPSCrop -sOutputFile=$(MAINFILE).pdf -c save pop -f $(MAINFILE).eps

%.eps: %.png
	inkscape --without-gui --export-eps=$@ --export-bbox-page --export-text-to-path $<

%.png: %.svg
	inkscape --without-gui --export-png=$@ --export-text-to-path $<

%.eps: %.svg
	inkscape --without-gui --export-eps=$@ --export-bbox-page --export-text-to-path  $<

%.pdf: %.svg
	inkscape --without-gui --export-pdf=$@ --export-text-to-path $<

pdfmerge:
	$(PDFLATEX) -jobname=NUP$(NUP)_merged $(SAMP)\documentclass{scrbook} \usepackage[pdftex]{graphicx} \usepackage{pdfpages} \usepackage[pdftex]{hyperref} \begin{document} \includepdfmerge[nup=$(NUP)]{$(FILELIST)} \end{document}$(SAMP)

nup:
	$(PDFLATEX) -jobname=NUP_$(MAINFILE) $(SAMP)\documentclass{scrbook} \usepackage[pdftex]{graphicx} \usepackage{pdfpages} \usepackage[pdftex]{hyperref} \begin{document} \includepdf[pages={$(PAGES)}, nup=$(NUP), $(ORIENTATION)]{$(MAINFILE).pdf} \end{document}$(SAMP)

ifeq ($(COMPILER),$(LATEX))
booklet: $(MAINFILE).ps
	$(PSBOOK) $(MAINFILE).ps | $(PSTOPS) $(PSTOPSOPT) > $(MAINFILE)_Book.ps && \
	$(PSSELECT) -o $(MAINFILE)_Book.ps $(MAINFILE)_Book1.ps && \
	$(PSSELECT) -e -r $(MAINFILE)_Book.ps $(MAINFILE)_Book2.ps
	if !($(TESTDIR) $(BOOKLETOUTPUT)); then $(MKDIRPARENT) $(BOOKLETOUTPUT); fi
	$(CP) $(MAINFILE)_Book.ps $(BOOKLETOUTPUT)/$(PREFIX)_$(CUSTOMER)_Book_V$(RELEASE).ps && \
	$(CP) $(MAINFILE)_Book1.ps $(BOOKLETOUTPUT)/$(PREFIX)_$(CUSTOMER)_Book1_V$(RELEASE).ps && \
	$(CP) $(MAINFILE)_Book2.ps $(BOOKLETOUTPUT)/$(PREFIX)_$(CUSTOMER)_Book2_V$(RELEASE).ps
	$(GS) $(BOOKLETGSOPT) -sOutputFile=$(MAINFILE)_Book.pdf -c save pop -f $(MAINFILE)_Book.ps
	$(MV) $(MAINFILE)_Book.pdf $(BOOKLETOUTPUT)/$(PREFIX)_$(CUSTOMER)_Book_V$(RELEASE).pdf && \
	$(RMF) $(MAINFILE)_Book*
endif
ifeq ($(COMPILER),$(PDFLATEX))
booklet: $(MAINFILE).pdf
	$(PDFTOPS) $(PDF2PSOTPS) $(MAINFILE).pdf $(MAINFILE).ps
	$(PSBOOK) $(MAINFILE).ps | $(PSTOPS) $(PSTOPSOPT) > $(MAINFILE)_Book.ps && \
	$(PSSELECT) -o $(MAINFILE)_Book.ps $(MAINFILE)_Book1.ps && \
	$(PSSELECT) -e -r $(MAINFILE)_Book.ps $(MAINFILE)_Book2.ps
	if !($(TESTDIR) $(BOOKLETOUTPUT)); then $(MKDIRPARENT) $(BOOKLETOUTPUT); fi
	$(CP) $(MAINFILE)_Book.ps $(BOOKLETOUTPUT)/$(PREFIX)_$(CUSTOMER)_Book_V$(RELEASE).ps && \
	$(CP) $(MAINFILE)_Book1.ps $(BOOKLETOUTPUT)/$(PREFIX)_$(CUSTOMER)_Book1_V$(RELEASE).ps && \
	$(CP) $(MAINFILE)_Book2.ps $(BOOKLETOUTPUT)/$(PREFIX)_$(CUSTOMER)_Book2_V$(RELEASE).ps
	$(GS) $(BOOKLETGSOPT) -sOutputFile=$(MAINFILE)_Book.pdf -c save pop -f $(MAINFILE)_Book.ps
	$(MV) $(MAINFILE)_Book.pdf $(BOOKLETOUTPUT)/$(PREFIX)_$(CUSTOMER)_Book_V$(RELEASE).pdf && \
	$(RMF) $(MAINFILE)_Book*
endif

ifeq ($(COMPILER),$(LATEX))
$(MAINFILE).pdf: $(MAINFILE).ps
	$(GS) $(GSOPT) -sOutputFile=$(MAINFILE).pdf -c save pop -f $(MAINFILE).ps
endif
ifeq ($(COMPILER),$(PDFLATEX))
$(MAINFILE).pdf: $(MAINFILE).tex
ifneq ($(MSG), default)
	@echo $(MSG) && $(SHELLPAUSE)
endif
	$(PDFLATEX) $(EPSTOPDFOPT) $(OPTS) $(SAMP)\input $(MAINFILE).tex$(SAMP)
	if ($(TESTFILE) $(MAINFILE).idx); then $(MAKEINDEX) $(MAINFILE).idx $(INDEXOPT); fi
	if ($(TESTFILE) $(MAINFILE).glo); then $(MAKECHANGE) $(MAINFILE).chn $(MAINFILE).glo; fi
	if ($(TESTFILE) $(MAINFILE).bib); then $(BIBTEX) $(MAINFILE); fi
	$(PDFLATEX) $(EPSTOPDFOPT) $(OPTS) $(SAMP)\input $(MAINFILE).tex$(SAMP) && \
	$(PDFLATEX) $(EPSTOPDFOPT) $(OPTS) $(SAMP)\input $(MAINFILE).tex$(SAMP)
endif
##	$(PDFLATEX) -jobname=NUP1x2_$(MAINFILE) $(SAMP)\documentclass{scrbook} \usepackage[pdftex]{graphicx} \usepackage{pdfpages} \usepackage[pdftex]{hyperref} \begin{document} \includepdf[pages=-, nup=1x2, landscape]{$(MAINFILE).pdf} \end{document}$(SAMP)
	if !($(TESTDIR) $(PDFOUTPUT)); then $(MKDIRPARENT) $(PDFOUTPUT); fi
ifeq ($(PREFIX),)
	$(CP) $(MAINFILE).pdf $(PDFOUTPUT)/$(MAINFILE).pdf
else
	$(CP) $(MAINFILE).pdf $(PDFOUTPUT)/$(PREFIX)_$(CUSTOMER)$(MAINFILE).pdf
endif

## html and xml generation
$(MAINFILE).html: $(MAINFILE).tex 
	$(RMF) *.aux *.log *.4*
	if !($(TESTDIR) $(GRAPHICS_EPS)); then $(MKDIRPARENT) $(GRAPHICS_EPS); fi
	if ($(TESTDIR) $(GRAPHICS_PNG)); then make -f $(GENDOCU) png2eps SRC="$(shell ls $(GRAPHICS_PNG)/*.png)" && $(CP) $(GRAPHICS_PNG)/*.eps $(GRAPHICS_EPS); fi
	if ($(TESTDIR) $(GRAPHICS_SVG)); then make -f $(GENDOCU) svg2eps SRC="$(shell ls $(GRAPHICS_SVG)/*.svg)"  && $(CP) $(GRAPHICS_PNG)/*.eps $(GRAPHICS_EPS); fi
	$(LATEX) $(OPTS) $(SAMP)\makeatletter\def\htmlmode{enabled}\makeatother$(SAMP) $(SAMP)\input $(MAINFILE).tex$(SAMP) && \
	if ($(TESTFILE) $(MAINFILE).idx); then $(MAKEINDEX) $(MAINFILE).idx $(INDEXOPT); fi
	if ($(TESTFILE) $(MAINFILE).chn); then $(MAKECHANGE) $(MAINFILE).chn $(MAINFILE).glo; fi
	if ($(TESTFILE) $(MAINFILE).bib); then $(BIBTEX) $(MAINFILE); fi
	if !($(TESTDIR) $(HTMLOUTPUT)); then $(MKDIRPARENT) $(HTMLOUTPUT); fi
# $2 should be $(TEX4HTCFGHTML) text in quotes must be one string
	echo '\def\customer{$(CUSTOMER)}\def\mcu{$(PREFIX)}\def\Mcu{$(MCU)}\def\pxros{$(PXROS)}\def\mykeywords{Target: $(MCU), Release: $(RELEASE), DocVersion: $(DOC_MAIN_VERSION), Revision: $(REVISION)}\providecommand{\release}{$(MAINRELEASE)}\def\releasevers{V$(MAINRELEASE)\xspace}\def\DocVersion{$(DOC_MAIN_VERSION) $(REVISION)}\def\htmlmode{enabled} \input $(MAINFILE)' > $(MAINFILE).foo
#	Generate index
#	tex $(SAMP)\def\filename{{main_gcc_ug}{idx}{4dx}{ind}} \input  idxmake.4ht$(SAMP)
#  	$(MV) $(MAINFILE).4dx $(MAINFILE).idx
#	if ($(TESTFILE) $(MAINFILE).idx); then $(MAKEINDEX) $(MAINFILE).idx $(INDEXOPT); fi
	$(HTMLSCRIPT) $(MAINFILE).foo $(SAMP)$(RTFCFLAGS)$(SAMP) $(SAMP)$(SAMP) $(SAMP)$(SAMP)
	for i in $$(ls *.html); do cat $$i | sed -es/figures\\/eps\\///g | sed -es/id=\"x1/name=\"x1/g | sed -es/": <\\/td><td"/":"/ -es/"class=\"content\">"// > $$i; done
	$(MV) --target-directory=$(HTMLOUTPUT) $(MAINFILE)*.html $(MAINFILE).css $(MAINFILE)*.png figures/eps/*.png

chm: $(MAINFILE).tex
	$(RTFC)

htmlsec: $(MAINFILE).tex png2eps svg2eps
	make -f $(GENDOCU) html RTFCFLAGS=$(HTMLFLAGS)

$(MAINFILE).xml: $(MAINFILE).tex png2eps svg2eps
	if !($(TESTDIR) $(XMLOUTPUT)); then $(MKDIRPARENT) $(XMLOUTPUT); fi
	$(RMF) *.aux *.log *.ind *.idx *.ilg *.4*
	if !($(TESTDIR) $(GRAPHICS_EPS)); then $(MKDIRPARENT) $(GRAPHICS_EPS); fi
	if ($(TESTDIR) $(GRAPHICS_PNG)); then make -f $(GENDOCU) png2eps SRC="$(shell ls $(GRAPHICS_PNG)/*.png)" && $(CP) $(GRAPHICS_PNG)/*.eps $(GRAPHICS_EPS); fi
	if ($(TESTDIR) $(GRAPHICS_SVG)); then make -f $(GENDOCU) svg2eps SRC="$(shell ls $(GRAPHICS_SVG)/*.svg)"  && $(CP) $(GRAPHICS_PNG)/*.eps $(GRAPHICS_EPS); fi
## $2 should be $(TEX4HTCFGXML) text in quotes must be one string
	echo '\def\customer{$(CUSTOMER)}\def\mcu{$(PREFIX)}\def\Mcu{$(MCU)}\def\pxros{$(PXROS)}\def\mykeywords{Target: $(MCU), Release: $(RELEASE), DocVersion: $(DOC_MAIN_VERSION), Revision: $(REVISION)}\providecommand{\release}{$(MAINRELEASE)}\def\releasevers{V$(MAINRELEASE)\xspace}\def\DocVersion{$(DOC_MAIN_VERSION) $(REVISION)}\def\htmlmode{enabled} \input $(MAINFILE)' > $(MAINFILE).foo
	$(XMLSCRIPT) $(MAINFILE).foo $(SAMP)$(TEX4HTCFGHTML)$(SAMP) $(SAMP)$(SAMP)
	$(MV) --target-directory=$(XMLOUTPUT) *.xml *.css
##	Index generation for docbook format
##	if ($(TESTFILE) $(MAINFILE).bib); then $(BIBTEX) $(MAINFILE); fi
##	if ($(TESTFILE) $(MAINFILE).idx); then $(TEX) $(XMLINDEX); fi
##	if ($(TESTFILE) $(MAINFILE).idx); then $(RMF) $(MAINFILE)-*.idx; fi
##	if ($(TESTFILE) $(MAINFILE).4dx); then $(CP) $(MAINFILE).4dx $(MAINFILE)-idx.idx; fi
##	if ($(TESTFILE) $(MAINFILE)-idx.idx); then $(MAKEINDEX) $(MAINFILE)-idx.idx; fi
##	dbmlatex $(MAINFILE).foo $(SAMP)$(TEX4HTCFGHTML)$(SAMP) $(SAMP)$(SAMP) -e$(TEX4HTENV)
	$(MV) --target-directory=$(XMLOUTPUT) $(MAINFILE).xml $(MAINFILE).css $(MAINFILE)*.png figures/eps/*.png

$(MAINFILE).sxw: clean $(MAINFILE).tex
	if !($(TESTDIR) $(OPENOFFICEOUTPUT)); then $(MKDIRPARENT) $(OPENOFFICEOUTPUT); fi
	if !($(TESTDIR) $(GRAPHICS_EPS)); then $(MKDIRPARENT) $(GRAPHICS_EPS); fi
	if ($(TESTDIR) $(GRAPHICS_PNG)); then make -f $(GENDOCU) png2eps SRC="$(shell ls $(GRAPHICS_PNG)/*.png)" && $(CP) $(GRAPHICS_PNG)/*.eps $(GRAPHICS_EPS); fi
	if ($(TESTDIR) $(GRAPHICS_SVG)); then make -f $(GENDOCU) svg2eps SRC="$(shell ls $(GRAPHICS_SVG)/*.svg)"  && $(CP) $(GRAPHICS_PNG)/*.eps $(GRAPHICS_EPS); fi
	echo '\def\customer{$(CUSTOMER)}\def\mcu{$(PREFIX)}\def\Mcu{$(MCU)}\def\pxros{$(PXROS)}\providecommand{\release}{$(MAINRELEASE)}\def\releasevers{V$(MAINRELEASE)\xspace}\def\htmlmode{enabled} \input $(MAINFILE)' > $(MAINFILE).foo
	$(OOSCRIPT) $(MAINFILE).foo $(SAMP)$(SAMP) $(SAMP)$(SAMP) -e$(TEX4HTENV)
	$(MV) --target-directory=$(OPENOFFICEOUTPUT) *.odt

relnotes:
	make -f $(RELNOTES) pdf MAINFILE=$(MAINFILE)

xml2out:
	java -jar $(GUTENBERG)/saxon/saxon.jar $(MAINFILE).xml $(XSLFILE) > $(OUT)

mogrify:
	mogrify -format $(OUTFORMAT) $(INFORMAT)

charencode:
	iconv -f $(INENCODE) -t $(OUTENCODE) -o  $(OUTENCODE)_$(FILE) $(FILE)
	$(MV) $(OUTENCODE)_$(FILE) $(FILE)

clean:
	$(RMF) *.aux *.log *.bbl *.blg *.ind *.idx *.ilg *.glo *.chn \
	*.toc *.lot *.lof *.log *.out $(MAINFILE).dvi $(MAINFILE).ps $(MAINFILE)*.html $(MAINFILE).xml zz$(MAINFILE).ps $(MAINFILE).foo *.cut *.ppm \
	*.fls *.4* *.idv *.lg *.xref *.tmp *.bak  $(MAINFILE)*.css $(MAINFILE)*.png $(MAINFILE)*.gif .pdf *.head  _temp.* *.snm *.nav *~ *.backup
	if ($(TESTFILE) main_$(DOCUTEMPLATE).tex); then $(RMF) main_$(DOCUTEMPLATE).tex; fi
	if ($(TESTFILE) main_$(BEAMERTEMPLATE).tex); then $(RMF) main_$(BEAMERTEMPLATE).tex; fi
	if ($(TESTDIR) figures/eps); then $(RMF) figures/eps/*.pdf; fi

cleanpdf: clean
	$(RMF) $(MAINFILE).pdf *.dep
	if ($(TESTDIR) $(PDFOUTPUT)); then $(RMF) $(PDFOUTPUTF); fi

cleanwww: clean

realclean: clean
	$(RMF) $(MAINFILE).pdf *.dep
	if ($(TESTDIR) $(OUTPUT)); then $(RMF) $(OUTPUTF); fi

texdirset: addpath
ifeq ($(OSTYPE),linux)
## . causes that variables in shell script are set globally !!!!!
	$(shell . $(GUTENBERG)/$(TEXDISTRIBUTION)/makefiles/texdirset.sh)
else
# Variables for GHOST SCRIPT option -m -> Systemenvironment and not user environment
	setx GUTENBERG "$(SERVERIP)/Tex"
# Location of Makefiles
	setx GENDOCU "$(SERVERIP)/Tex/$(TEXDISTRIBUTION)/makefiles/gendocu.mak"
	setx RELNOTES "$(SERVERIP)/Tex/$(TEXDISTRIBUTION)/makefiles/release_notes.mak"

# Variables for TEX SYSTEM
	setx TEXDIR "$(SERVERIP)/Tex/$(TEXDISTRIBUTION)"
	setx TEXMFMAIN "$(SERVERIP)/Tex/$(TEXDISTRIBUTION)/texmf"
	setx TEXMFLOCAL "$(SERVERIP)/Tex/$(TEXDISTRIBUTION)/texmf-local"
	setx VARTEXMF "$(SERVERIP)/Tex/$(TEXDISTRIBUTION)/texmf-var"
	setx TEXMFCNF "$(SERVERIP)/Tex/$(TEXDISTRIBUTION)/texmf/web2c"
endif

addpath:
ifeq ($(OSTYPE),linux)
else
	setx MSYSSHELL "$(SERVERIP)/Tex/Msys/bin;"
	setx ADDPATH "$(SERVERIP)/Tex/$(TEXDISTRIBUTION)/bin/win32;$(SERVERIP)/Tex/ImageMagick;$(SERVERIP)/Tex/GsTools/gs$(GSVERSION)/lib;$(SERVERIP)/Tex/GsTools/gs$(GSVERSION)/bin;$(SERVERIP)/Tex/GsTools/gsview;$(SERVERIP)/Tex/Miktex/texmf/miktex/bin;$(SERVERIP)/Tex/Winedt;"
endif

setshell:
ifeq ($(OSTYPE),linux)
else
	setx MSYSSHELL "$(SERVERIP)/Tex/Msys/bin;"
endif

removeshell:
ifeq ($(OSTYPE),linux)
else
	setx MSYSSHELL ""
endif
