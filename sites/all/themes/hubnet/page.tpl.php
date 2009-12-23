<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language ?>" lang="<?php print $language->language ?>" dir="<?php print $language->dir ?>">
  <head>
    <?php print $head ?>
    <?php print $styles ?>
    <title><?php print $head_title ?></title>
  </head>
  <body <?php print drupal_attributes($attr) ?>>

  <?php if (!empty($admin)) print $admin ?>

  <?php if ($hub_bar): ?>
  <div id='hub-bar'><div class='limiter clear-block'>
    <?php print $hub_bar ?>
  </div></div>
  <?php endif; ?>

  <?php if ($header): ?>
    <div id='header'><div class='limiter clear-block'>
      <?php print $header; ?>
    </div></div>
  <?php endif; ?>

  <div id='branding'><div class='limiter clear-block'>
    <?php if ($action_link) print $action_link ?>
    <?php if ($logo) print $logo ?>
    <?php if ($site_name): ?><h1 class='site-name'><?php print $site_name ?></h1><?php endif; ?>
    <?php if ($site_slogan): ?><h2 class='site-slogan'><?php print $site_slogan ?></h2><?php endif; ?>
  </div></div>

  <div id='navigation'><div class='limiter clear-block'>
    <?php if (isset($primary_links)) : ?>
      <?php print theme('links', $primary_links, array('class' => 'links primary-links')) ?>
      <?php if ($search_box): ?><div class="block block-theme"><?php print $search_box ?></div><?php endif; ?>
    <?php endif; ?>
    <?php if (isset($secondary_links)) : ?>
      <?php print theme('links', $secondary_links, array('class' => 'links secondary-links')) ?>
    <?php endif; ?>
  </div></div>

  <div id='page'><div class='limiter clear-block'>

    <?php if ($left): ?>
      <div id='left' class='clear-block'><?php print $left ?></div>
    <?php endif; ?>

    <div id='main' class='clear-block'>
      <?php if ($title): ?><h2 class='page-title'><?php print $title ?></h2><?php endif; ?>
      <?php if ($help || ($show_messages && $messages)): ?>
        <div id='console'>
          <?php print $help; ?>
          <?php if ($show_messages && $messages): print $messages; endif; ?>
        </div>
      <?php endif; ?>
      <?php if ($tabs) print $tabs ?>
      <?php if ($tabs2) print $tabs2 ?>
      <div id='content' class='clear-block'><?php print $content ?></div>
    </div>

    <?php if ($right): ?>
      <div id='right' class='clear-block'><?php print $right ?></div>
    <?php endif; ?>

  </div></div>

  <div id="footer"><div class='limiter clear-block'>
    <?php print $feed_icons ?>
    <?php print $footer ?>
    <?php print $footer_message ?>
  </div></div>

  <?php print $scripts ?>
  <?php print $closure ?>

  </body>
</html>
