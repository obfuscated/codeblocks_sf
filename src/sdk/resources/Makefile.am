SUBDIRS = lexers

resources_forZIP = \
	auto_detect_compilers.xrc \
	configure_tools.xrc \
	confirm_replace_multiple.xrc \
	confirm_replace.xrc \
	edit_array_order.xrc \
	edit_array_string.xrc \
	editor_manager.xrc \
	edit_pair.xrc \
	edit_path.xrc \
	edit_tool.xrc \
	external_deps.xrc \
	findreplacedlg.xrc \
	generic_multiline_notes.xrc \
	generic_multi_select.xrc \
	get_global_uservar.xrc \
	global_uservars.xrc \
	new_from_template.xrc \
	plugins_configuration.xrc \
	projectfile_options.xrc \
	project_manager_file_types.xrc \
	scripting_security_dlg.xrc \
	select_target.xrc \
	images/8x8/breakpoint_disabled.png \
	images/8x8/breakpoint_other.png \
	images/8x8/breakpoint.png \
	images/10x10/breakpoint_disabled.png \
	images/10x10/breakpoint_other.png \
	images/10x10/breakpoint.png \
	images/12x12/breakpoint_disabled.png \
	images/12x12/breakpoint_other.png \
	images/12x12/breakpoint.png \
	images/16x16/breakpoint_disabled.png \
	images/16x16/breakpoint_other.png \
	images/16x16/breakpoint.png \
	images/16x16/select_target.png \
	images/20x20/breakpoint_disabled.png \
	images/20x20/breakpoint_other.png \
	images/20x20/breakpoint.png \
	images/24x24/breakpoint_disabled.png \
	images/24x24/breakpoint_other.png \
	images/24x24/breakpoint.png \
	images/28x28/breakpoint_disabled.png \
	images/28x28/breakpoint_other.png \
	images/28x28/breakpoint.png \
	images/32x32/breakpoint_disabled.png \
	images/32x32/breakpoint_other.png \
	images/32x32/breakpoint.png \
	images/40x40/breakpoint_disabled.png \
	images/40x40/breakpoint_other.png \
	images/40x40/breakpoint.png \
	images/48x48/breakpoint_disabled.png \
	images/48x48/breakpoint_other.png \
	images/48x48/breakpoint.png \
	images/56x56/breakpoint_disabled.png \
	images/56x56/breakpoint_other.png \
	images/56x56/breakpoint.png \
	images/64x64/breakpoint_disabled.png \
	images/64x64/breakpoint_other.png \
	images/64x64/breakpoint.png \
	images/closebutton.png \
	images/select_target.png

dist_noinst_DATA = \
	$(resources_forZIP)

pkgdata_DATA = manager_resources.zip
CLEANFILES = $(pkgdata_DATA)

manager_resources.zip: $(resources_forZIP)
	PWD=`pwd` cd $(srcdir) && zip -qu $(PWD)/manager_resources.zip $(resources_forZIP) > /dev/null
