<?
  if(!in_array($_SERVER['PHP_SELF'],array('/index.shtml','/','/index.php')))
   $title.=' - Code::Blocks';
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" 
  "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html xml:lang="en-us" xmlns="http://www.w3.org/1999/xhtml">
<head>
    <title><? echo $title;?></title>

    <link rel="stylesheet" type="text/css" media="screen" href="/css/main.css" />
    <!-- link rel="stylesheet" type="text/css" media="print" href="/css/print.css" / -->
    <link rel="Shortcut Icon" href="/favicon.ico" type="image/x-icon" />

    <!-- script type="text/javascript" src="/js/scripts.js"></script -->

    <meta http-equiv="Content-type" content="text/html; charset=iso-8859-1" />
    <meta http-equiv="Content-Language" content="en-us" />
    <meta name="ROBOTS" content="ALL" />
    <meta name="Copyright" content="Design Copyright (c) 2005 Ricardo Garcia, Per Eckerdal. Content Copyright (c) the respective authors" />
    <meta http-equiv="imagetoolbar" content="no" />

    <!-- <link rel="alternate" type="application/rss+xml" title="RSS" href="/rss.xml" /> -->
    <link rel="help" href="/help.shtml" title="Site help" />

    <meta name="description" content="Codeblocks is a cross-platform IDE built around wxWidgets, designed to be extensible and configurable. Runs on Windows and Linux." />
    <meta name="keywords" content="ide,free,mingw,windows,linux,wxwidgets,codeblocks,
 code blocks studio,codeblocks IDE,MinGW,cygwin,gnu,gcc,g++,open source,cross platform,c,c++,
 dev-cpp alternatives,dev-c++ alternatives,Visual Studio alternative,
 plugin,plugins,plugin framework,extensible,configurable,project management,
 integrated,development,environment,debugger,code completion,auto-complete,
 wizards,multiple compilers,code::blocks" />
</head>
<body>
  <div id="mainhdr"><? include('mainhdr.tpl');?></div>
<table class="maintable" cellspacing="8" style="width:100%;margin:0;table-layout:fixed">
<colgroup>
<col width="155" />
<col width="*" />
</colgroup>
<tr>
<td class="main topblock navblock"><br />
<h2>Navigation</h2>
<div class="subblock">
<? include('mainmenu.tpl');?></div>
<h2>Donate</h2>
<div class="subblock">
<a href="http://sourceforge.net/donate/index.php?group_id=126998"><img
 class="center" src="/img/project-support.jpg" width="88" height="32" alt="Support this project"/></a>
</div>
</td>
<td rowspan="2" class="main topblock"><div id="mainblock">
<?
 if(!empty($title2)) echo "<h1>$title2</h1>";
 echo $this->get('body');
?>
</div></td>
</tr>
<tr><td class="main topblock navblock">
<h2>Sponsors</h2>
<div id="adsblock">
<? include('adsblock.tpl'); ?></div>
<br />
</td></tr>
</table>
<div id="footer" class="topblock"><? include('footer.tpl');?></div>
</body>
</html>


