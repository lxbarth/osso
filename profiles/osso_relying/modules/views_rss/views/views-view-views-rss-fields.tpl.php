<?php // $Id: views-view-views-rss-fields.tpl.php,v 1.1 2009/09/22 16:49:55 rsoden Exp $ ?>
<?php print "<?xml"; ?> version="1.0" encoding="utf-8" <?php print "?>"; ?>

<rss version="2.0" <?php print $namespaces ?>>
  <channel>
    <title><?php print $viewtitle; ?></title>
    <description><?php print $description; ?></description>
    <link><?php print $link ?></link>
    <?php print $rows ?>
  </channel>
</rss>
