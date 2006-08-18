echo "creating core .pot files"
find ../sdk ../src			|grep -P "(\.cpp|\.h)$" |xargs xgettext --keyword=_ -o codeblocks.pot
find ../plugins |grep -v contrib	|grep -P "(\.cpp|\.h)$" |xargs xgettext --keyword=_ -o coreplugins.pot

echo "creating contrib .pot files"
find ../plugins/contrib/codestat	|grep -P "(\.cpp|\.h)$" |xargs xgettext --keyword=_ -o  codestat.pot
find ../plugins/contrib/byogames	|grep -P "(\.cpp|\.h)$" |xargs xgettext --keyword=_ -o  byogames.pot
find ../plugins/contrib/cb_koders	|grep -P "(\.cpp|\.h)$" |xargs xgettext --keyword=_ -o  cb_koders.pot
find ../plugins/contrib/codesnippets	|grep -P "(\.cpp|\.h)$" |xargs xgettext --keyword=_ -o  codesnippets.pot
find ../plugins/contrib/copystrings	|grep -P "(\.cpp|\.h)$" |xargs xgettext --keyword=_ -o  copystrings.pot
find ../plugins/contrib/devpak_plugin	|grep -P "(\.cpp|\.h)$" |xargs xgettext --keyword=_ -o  devpak_plugin.pot
find ../plugins/contrib/dragscroll	|grep -P "(\.cpp|\.h)$" |xargs xgettext --keyword=_ -o  dragscroll.pot
find ../plugins/contrib/source_exporter	|grep -P "(\.cpp|\.h)$" |xargs xgettext --keyword=_ -o  source_exporter.pot
find ../plugins/contrib/help_plugin	|grep -P "(\.cpp|\.h)$" |xargs xgettext --keyword=_ -o  help_plugin.pot
find ../plugins/contrib/keybinder	|grep -P "(\.cpp|\.h)$" |xargs xgettext --keyword=_ -o  keybinder.pot
find ../plugins/contrib/profiler	|grep -P "(\.cpp|\.h)$" |xargs xgettext --keyword=_ -o  profiler.pot
find ../plugins/contrib/wxSmith		|grep -P "(\.cpp|\.h)$" |xargs xgettext --keyword=_ -o  wxsmith.pot


echo "running msginit"

for LANG in de fr it es cs da nl ga hu pl ru sk jp tr
do
mkdir $LANG
for POT in *.pot
do

# Windows hack: use cmd /C mkdir because the UnxUtils mkdir does nothing when run from bash...
#	test -d $LANG || mkdir $LANG
	test -d $LANG || cmd /C mkdir $LANG

	msginit -i $POT -o $LANG/$POT -l $LANG
done

done


