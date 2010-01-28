<?php
// $Id: FeedsScheduler.inc,v 1.12 2010/01/28 21:00:45 alexb Exp $

/**
 * @file
 * FeedsScheduler class and related.
 */

/**
 * Describe a scheduler.
 */
interface FeedsSchedulerInterface {

  /**
   * Run Drupal cron.
   */
  public function cron();

  /**
   * Add a feed to the schedule.
   *
   * @param $importer_id
   *   Id of a FeedsImporter object.
   * @param $callback
   *   The callback to invoke on importer. Either 'import' or 'expire'.
   * @param $feed_nid
   *   Feed nid that identifies the source for this configuration.
   */
  public function add($importer_id, $callback, $feed_nid = 0);

  /**
   * Remove a feed from the schedule.
   *
   * @param $feed_nid
   *   Feed nid that identifies the source for this configuration.
   */
  public function remove($importer_id, $callback, $feed_nid = 0);

  /**
   * Work off a given feed identified by $feed_info.
   *
   * @param $feed_info
   *   Array where 'importer_id' key is the id of a FeedsImporter object,
   *   and 'feed_nid' is the feed node id that identifies the
   *   source of a FeedsSource object.
   */
  public function work($feed_info);
}

/**
 * Implementation of FeedsSchedulerInterface.
 *
 * This scheduler uses the last_scheduled_time paradigm: By storing the time
 * when a particular feed was scheduled to be refreshed last rather than
 * storing when a feed should be _refreshed_ next, we gain two advantages:
 *
 * 1) If a feed's import_period setting changes, it has immediate effects -
 *    without batch updating an existing schedule.
 * 2) The time between refreshes will always be scheduled based on when it
 *    has been scheduled last. Less drift occurs.
 */
class FeedsScheduler implements FeedsSchedulerInterface {

  /**
   * Create a single instance of FeedsScheduler.
   */
  public static function instance() {
    static $instance;
    if (!isset($instance)) {
      $class = variable_get('feeds_scheduler_class', 'FeedsScheduler');
      $instance = new $class();
    }
    return $instance;
  }

  /**
   * Protect constructor.
   */
  protected function __construct() {}

  /**
   * Implementation of FeedsSchedulerInterface::cron().
   *
   * Refreshes scheduled feeds.
   *
   * If drupal_queue is present, only pushes refresh tasks to queue and
   * returns. If drupal_queue is not available, works off tasks.
   */
  public function cron() {

    // Check and set scheduler semaphore, take time.
    if (variable_get('feeds_scheduler_cron', FALSE)) {
      watchdog('FeedsScheduler', 'Last cron process did not finish.', array(), WATCHDOG_ERROR);
    }
    variable_set('feeds_scheduler_cron', TRUE);
    $start = time();

    // Get feeds configuration, check whether drupal_queue is present and set
    // parameters accordingly.
    if ($importers = feeds_importer_load_all()) {

      if ($use_queue = module_exists('drupal_queue')) {
        drupal_queue_include();
        $queue = drupal_queue_get(FEEDS_SCHEDULER_QUEUE);
        $num = variable_get('feeds_schedule_queue_num', 200);
      }
      else {
        $num = variable_get('feeds_schedule_num', 5);
      }

      // Iterate over feed configurations, pick $num feeds for each
      // configuration, push to queue or refresh feeds.
      foreach ($importers as $importer) {
        foreach ($importer->getScheduleCallbacks() as $callback) {

          // Check whether jobs are scheduled.
          $period = $importer->getSchedulePeriod($callback);
          if ($period != FEEDS_SCHEDULE_NEVER) {

            // Refresh feeds that have a refresh time older than now minus
            // refresh period.
            $time = FEEDS_REQUEST_TIME - $period;

            $result = db_query_range('SELECT feed_nid, id AS importer_id, callback, last_scheduled_time FROM {feeds_schedule} WHERE id = "%s" AND callback = "%s" AND scheduled = 0 AND (last_scheduled_time < %d OR last_scheduled_time = 0) ORDER BY last_scheduled_time ASC', $importer->id, $callback, $time, 0, $num);
            while ($feed_info = db_fetch_array($result)) {

              // If drupal_queue is present, add to queue, otherwise work off
              // immediately.
              if ($use_queue) {
                if ($queue->createItem($feed_info)) {
                  $this->flag($feed_info['importer_id'], $feed_info['callback'], $feed_info['feed_nid']);
                }
                else {
                  watchdog('FeedsScheduler', 'Error adding item to queue.', WATCHDOG_ALERT);
                }
              }
              else {
                $this->flag($feed_info['importer_id'], $feed_info['callback'], $feed_info['feed_nid']);
                $this->work($feed_info);
              }
            }
          }
        }
      }
    }

    // Unflag and post a message that we're done.
    variable_set('feeds_scheduler_cron', FALSE);
    watchdog('FeedsScheduler', 'Finished processing schedule after !time.', array('!time' => format_interval(time() - $start)));
  }

  /**
   * Implementation of FeedsSchedulerInterface::add().
   *
   * Add a feed to the scheduler.
   *
   * @todo Create optional parameter $last_scheduled_time to pass in. Set this
   *   value if a feed is refreshed on creation.
   * @todo Create an abstract interface for items that can be added?
   */
  public function add($importer_id, $callback, $feed_nid = 0) {
    $save = array(
      'id' => $importer_id,
      'callback' => $callback,
      'feed_nid' => $feed_nid,
      'last_scheduled_time' => 0,
      'scheduled' => 0, // Means NOT scheduled at the moment.
    );
    drupal_write_record('feeds_schedule', $save, array('id', 'callback', 'feed_nid'));
    if (!db_affected_rows()) {
      drupal_write_record('feeds_schedule', $save);
    }
  }

  /**
   * Implementation of FeedsSchedulerInterface::remove().
   */
  public function remove($importer_id, $callback, $feed_nid = 0) {
    db_query('DELETE FROM {feeds_schedule} WHERE id = "%s" AND callback = "%s" AND feed_nid = %d', $importer_id, $callback, $feed_nid);
  }

  /**
   * Implementation of FeedsSchedulerInterface::work().
   *
   * Refresh a feed.
   *
   * Used as worker callback invoked from feeds_scheduler_refresh() or
   * if drupal_queue is not enabled, directly from $this->cron().
   */
  public function work($feed_info) {
    $importer = feeds_importer($feed_info['importer_id']);
    // Remove scheduled flag, if we fail after this we'd like to try again asap.
    $this->unflag($feed_info['importer_id'], $feed_info['callback'], $feed_info['feed_nid']);
    try {
      $importer->work($feed_info);
    }
    catch (Exception $e) {
      watchdog('FeedsScheduler', $e->getMessage(), array(), WATCHDOG_ERROR);
    }
  }

  /**
   * Helper function to flag a feed scheduled.
   *
   * This function sets the feed's scheduled bit to 1 and updates
   * last_scheduled_time to FEEDS_REQUEST_TIME.
   *
   * @param $id
   *   Id of the importer configuration.
   * @param $callback
   *   Callback of the job.
   * @param $feed_nid
   *   Identifier of the feed node.
   */
  protected function flag($id, $callback, $feed_nid) {
    $save = array(
      'id' => $id,
      'callback' => $callback,
      'feed_nid' => $feed_nid,
      'last_scheduled_time' => FEEDS_REQUEST_TIME,
      'scheduled' => 1,
    );
    drupal_write_record('feeds_schedule', $save, array('id', 'callback', 'feed_nid'));
  }

  /**
   * Helper function to flag a feed unscheduled.
   *
   * This function sets the feed's scheduled bit to 0 and thus makes
   * it eligible for being added to the queue again.
   *
   * @param $id
   *   Id of the importer configuration.
   * @param $callback
   *   Callback of the job.
   * @param $feed_nid
   *   Identifier of the feed node.
   */
  protected function unflag($id, $callback, $feed_nid) {
    $save = array(
      'id' => $id,
      'callback' => $callback,
      'feed_nid' => $feed_nid,
      'scheduled' => 0,
    );
    drupal_write_record('feeds_schedule', $save, array('id', 'callback', 'feed_nid'));
  }
}
