$Id: README.txt,v 1.4 2009/11/02 21:48:21 alexb Exp $

DRUPAL QUEUE
------------

Queues jobs for asynchronous execution.

Drupal 6 Backport of Drupal 7 Queue API by chx, dww, neclimdul et. al.

Related Drupal 7 issues

Job Queue API http://drupal.org/node/391340

Use queue for cron http://drupal.org/node/578676

INSTALLATION
------------

- Install module
- Schedule workers: If using drush, use add "drush queue cron" to your crontab.
  Otherwise copy drupal_queue_cron.php to your site's root directory and add it
  to your crontab just like cron.php
- You can schedule as many workers concurrently as your server resources allow
  for.

USING DRUPAL QUEUE
------------------

If your module uses the Drupal Queue API, note that jobs being queued need to be
concurrency-safe. For an example look at Drupal 7 aggregator module or Drupal 6
Feeds module.

http://cvs.drupal.org/viewvc/drupal/drupal/modules/aggregator/
http://drupal.org/project/feeds