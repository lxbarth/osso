<?php print "<?xml"; ?> version="1.0" encoding="utf-8" <?php print "?>"; ?>
<feed xmlns="http://www.w3.org/2005/Atom"
 xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
 xmlns:foaf="http://xmlns.com/foaf/0.1/">

  <link rel="hub" href="<?php print $hub ?>" />
  <link rel="self" href="<?php print $feed_url ?>" />
  <id><?php print $feed_url ?></id>
  <link href="<?php print $user_url ?>"/>
  <title type="text"><?php print $feed_title ?></title>
  <updated><?php print $updated ?></updated>
  <author>
    <name><?php print $site_name ?></name>
    <uri><?php print $site_url ?></uri>
  </author>

  <entry>
    <title><?php print $user_name ?></title>
    <link href="<?php print $user_url ?>" />
    <id><?php print $user_url ?></id>
    <updated><?php print $updated ?></updated>
    <foaf:Person>
      <foaf:name><?php print $user_name ?></foaf:name>
      <?php if (isset($user_openid)): ?>
      <foaf:openid rdf:resource="<?php print $user_openid ?>" />
      <?php endif; ?>
      <foaf:mbox rdf:resource="<?php print $user_mail ?>" />
    </foaf:Person>
  </entry>

</feed>