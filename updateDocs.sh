#!/bin/sh
mkdir -p docs/doc-de/html
mkdir -p docs/doc-de/pdf
mkdir -p docs/doc_tmp.de

cd docs/doc_tmp.de
wget -N http://www.codeblocks.org/docs/manual_de.pdf

wget -Nkp http://www.codeblocks.org/docs/main_codeblocks_de.html

cd www.codeblocks.org/docs/

mkdir -p png.tmp

for i in *.png; do pngnq -f -e .png -d png.tmp $i;echo "crushing $i";  done

cd ../../..

cp -fu doc_tmp.de/manual_de.pdf doc-de/pdf/
cp -fu doc_tmp.de/www.codeblocks.org/docs/* doc-de/html
cp -fu doc_tmp.de/www.codeblocks.org/docs/png.tmp/* doc-de/html

rm -rf doc_tmp.de/www.codeblocks.org/doc-de/png.tmp/

mkdir -p doc-en/html
mkdir -p doc-en/pdf
mkdir -p doc_tmp.en

cd doc_tmp.en

wget -N http://www.codeblocks.org/docs/manual_en.pdf

wget -Nkp http://www.codeblocks.org/docs/main_codeblocks_en.html

cd www.codeblocks.org/docs/
mkdir -p  png.tmp

for i in *.png; do pngnq -f -e .png -d png.tmp $i;echo "crushing $i";  done

cd ../../..

cp -fu doc_tmp.en/manual_en.pdf doc-en/pdf/
cp -fu doc_tmp.en/www.codeblocks.org/docs/* doc-en/html
cp -fu doc_tmp.en/www.codeblocks.org/docs/png.tmp/* doc-en/html

rm -rf doc_tmp.en/www.codeblocks.org/docs/png.tmp/

