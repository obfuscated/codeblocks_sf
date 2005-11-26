# This Makefile is based on the Makefile of the Ubuntu documentation project.
# 
# st = status
# dg = Documentation Guide

current_distro=$(shell test -e /etc/debian_version && echo "debian")

# NWalsh Docbook XSL's
ifeq ($(current_distro), debian)
# Ubuntu
NWDBXSL=/usr/share/xml/docbook/stylesheet/nwalsh/html/docbook.xsl

# Debian and Ubuntu also need an explicit location for the XML catalog files
export XML_CATALOG_FILES = /usr/share/xml/docbook/schema/dtd/4.1.2/catalog.xml
else
# SuSE
NWDBXSL=/usr/share/xml/docbook/stylesheet/nwalsh/current/html/docbook.xsl
endif

# Collected and Write Status
wOS=./libs/writeOwnerStatus.xsl

HTMLXSL=$(NWDBXSL)

# Base directories for output from processor
BASE=./build/
STBASE=./build/status/
DGBASE=./build/docguide/

.PHONY: all
all: status dg

clean:
	rm -rf $(BASE)

#Status: AVAILABLE
# Code::Blocks Status Reports
status:
	@echo "--- Building status reports... please be patient. ---"
	@echo "--- Documentation Guide status report ---"
	xsltproc --xinclude -o $(STBASE)dg-report.xml $(wOS) docguide/C/documentation-guide.xml
	xsltproc --xinclude -o $(STBASE)dg-report.html $(NWDBXSL) $(STBASE)dg-report.xml
	
	@echo "--- Complete. Find outputs at" $(STBASE) ". ---"

# Status: TESTING
# Documentation Guide

dg:
	@echo "--- Building Documentation Guide. ---"

	@echo "--- Removing excist build files (if any). ---"
	rm -rf $(DGBASE)C/*.html
	@echo "--- Building new files. ---"
	xsltproc -o $(DGBASE)C/documentation-guide.html $(HTMLXSL) docguide/C/documentation-guide.xml
	
	@echo "--- Complete. Find output at " $(DGBASE) ". ---"
