<?php
ini_set('display_errors','0');
if(is_file('forums/SSI.php'))
  include('forums/SSI.php');

////////////////////////////////////////////////////////
// Hack to redirect old forum urls (PNphpBB2) to the  //
// correct forum/topic in the SMF forum.              //
//                                                    //
// Inspired from  phpBB_Redirection_v1.0 hack         //
// by Spaceman-Spiff - spiff@monkey-pirate.com        //
////////////////////////////////////////////////////////
if(in_array($_SERVER['PHP_SELF'],array('/','/index.php')) &&
   in_array($_GET['file'],array('viewtopic','viewforum')))
{
    if ($_GET['name'] == 'PNphpBB2')
    {
        $forumurl = 'http://forums.codeblocks.org/';
        if ($_GET['file'] == 'viewtopic')
        {
            $x = (int) $_GET['t'];
            header ('Location: ' . $forumurl . 'index.php?topic=' . $x);
            exit;
        }
        else if ($_GET['file'] == 'viewforum')
        {
            $x = (int) $_GET['f'];
            header ('Location: ' . $forumurl . 'index.php?board=' . $x);
            exit;
        }
    }
}
// end of PNphpBB2 urls hack

require_once('prepend.php');
?>
<title>Code::Blocks Studio - Open Source, Cross-platform Free C++ IDE</title>
<h1>Code::Blocks</h1>

<div class="right">
<a href="/img/screenshots/scr1.png"><img width="312" height="229"
alt="Screenshot" title="Zoom in"
src="/img/screenshots/scr1_tiny.jpg" /><br />
<span class="caption">(zoom in)</span></a><br />
<a class="caption" href="screenshots.shtml">(more screenshots)</a>
</div>
<h2 style="font-size:1.2em;border-bottom:none;">The open source, cross platform Free C++ IDE.</h2>
<p><b>Newest version: <a href="/downloads.shtml">1.0rc2</a>, Oct 25, 2005</b></p>

<p>Code::Blocks is a <em>free C++ IDE</em> built specifically to meet the most demanding needs of its users.
It was designed, right from the start, to be extensible and configurable.</p>

<p>Built around a plugin framework, Code::Blocks can be <em>extended with plugin DLLs</em>. It
includes a plugin wizard so you can <em>compile your own plugins</em>! (Free SDK downloaded separately)</p>

<p style="clear:right">For more information on Code::Blocks, please go to the <a href="/features.shtml">features list</a>.<br />
We hope you enjoy using it!</p>
<p>The Code::Blocks Team.</p>

<p class="caption"><b>[ <a title="Features list" href="/features.shtml">Next</a> ]</b></p>

<hr>


<?php
if(function_exists('ssi_recentTopics')) {
?><p><b>Latest <a href="http://forums.codeblocks.org">forum</a> topics:</b></p><?
   ssi_recentTopics();
}
?>