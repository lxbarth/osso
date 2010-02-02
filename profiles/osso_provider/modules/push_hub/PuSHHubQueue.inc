<?php

/**
 * Modify the SystemQueue's default behaviour so that it supports delayed
 * dequeueing.
 */
class PuSHHubQueue extends SystemQueue {
  /**
   * Create an item.
   *
   * @param $data
   *   As in SystemQueue::createItem().
   * @param $delay
   *   Time when item should be dequeued earliest.
   */
  public function createItem($data, $delay = 0) {
    $record = new stdClass();
    $record->name = $this->name;
    $record->data = $data;
    $record->expire = $delay;
    $record->created = time();
    return drupal_write_record('queue', $record) !== FALSE;
  }
}
