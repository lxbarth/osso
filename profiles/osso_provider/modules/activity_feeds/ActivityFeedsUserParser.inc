<?php

/**
 * Look up user id by author name.
 */
class ActivityFeedsUserParser extends FeedsSimplePieParser {

  /**
   * If the requested element is uid, look up the user by name.
   *
   * @todo: this could be folded into FeedsSimplePieParser.
   */
  public function getSourceElement($item, $element_key) {
    if ($element_key == 'uid') {
      if (isset($item['author_name']) && ($uid = db_result(db_query('SELECT uid FROM {users} WHERE name = "%s"', $item['author_name'])))) {
        return $uid;
      }
      return 0;
    }
    return parent::getSourceElement($item, $element_key);
  }

  /**
   * Override parent::getMappingSources().
   */
  public function getMappingSources() {
    $sources = parent::getMappingSources();
    return $sources + array(
      'uid' => array(
        'name' => t('User ID'),
        'description' => t('User ID looked up by feed item\'s author property. 0 (= anonymous) if no user could be found.'),
      ),
    );
  }
}