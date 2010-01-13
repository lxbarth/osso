$Id: README.txt,v 1.15 2009/12/03 21:27:40 alexb Exp $


"It feeds"


FEEDS
=====

The new incarnation of FeedAPI and Feed Element Mapper. Work in progress.

Contact alex_b for details:
http://drupal.org/user/53995

Features
========

- Pluggable import configurations consisting of fetchers (get data) parsers
  (read and transform data) and processors (create content on Drupal).
-- HTTP upload.
-- File upload.
-- CSV, RSS, Atom parsing.
-- Creates nodes or terms.
-- Creates lightweight database records if Data module is installed.
   http://drupal.org/project/data
-- Additional fetchers/parsers or processors can be added by an object oriented
   plugin system.
-- Granular mapping of parsed data to content elements.
- Import configurations can be piggy backed on nodes, thus using nodes as
  importers ("feed as node" approach) or they can be used on a standalone form.
- Unlimited number of import configurations.
- Export import configurations to code.
- Optional libraries module support.

Requirements
============

- CTools 1.1
- Drupal 6.14 or higher.
- PHP 5.2.x

Installation
============

- Install Feeds, Feeds Admin UI and Feeds defaults.
- Make sure cron is correctly configured http://drupal.org/cron
- Navigate to admin/build/feeds.
- Enable one or more default configuration or create your own: from scratch or
  by cloning.
- Go to import/ to import data.
- To use SimplePie parser, download SimplePie and place simplepie.inc into
  feeds/libraries.
  http://simplepie.org/

Libraries support
=================

If you are using Libraries module, you can place external libraries in the
Libraries module's search path (for instance sites/all/libraries. The only
external library used at the moment is SimplePie.

Libraries found in the libraries search path are preferred over libraries in
feeds/libraries/.

API Overview
============

See "The developer's guide to Feeds":
http://drupal.org/node/622700

Testing
=======

For all tests to pass, run tests with the latest releases of these modules
available in the Drupal search path:

- Views
- Data

This list does not include modules that are required by Feeds anyway.

Performance
===========

Untested. There is support for drupal_queue module in the works.
http://drupal.org/node/599180

Hidden settings
===============

Hidden settings are variables that you can define by adding them to the $conf
array in your settings.php file.


Name:        feeds_importer_class
Default:     'FeedsImporter'
Description: The class to use for importing feeds.

Name:        feeds_source_class
Default:     'FeedsSource'
Description: The class to use for handling feed sources.

Name:        feeds_scheduler_class
Default:     'FeedsScheduler'
Description: The class to use for scheduling feed refreshing.

Name:        feeds_worker_time
Default:     15
Description: Execution time for a queue worker, only effective if used with
             drupal_queue.

Name:        feeds_schedule_num
Default:     5
Description: The number of feeds to import on cron time.
             Only has an effect if Drupal Queue is *not* enabled.
             http://drupal.org/project/drupal_queue

Name:        feeds_schedule_queue_num
Default:     200
Description: The number of feeds to queue on cron time. Only has an effect if
             Drupal Queue is enabled.
             http://drupal.org/project/drupal_queue

Name:        feeds_data_$importer_id
Default:     feeds_data_$importer_id
Description: The table used by FeedsDataProcessor to store feed items. Usually a
             FeedsDataProcessor builds a table name from a prefix (feeds_data_)
             and the importer's id ($importer_id). This default table name can
             be overridden by defining a variable with the same name.

Glossary
========

See "Feeds glossary":
http://drupal.org/node/622710