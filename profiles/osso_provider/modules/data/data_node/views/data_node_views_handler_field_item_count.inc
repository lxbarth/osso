<?php
// $Id: data_node_views_handler_field_item_count.inc,v 1.1.2.1 2009/09/22 17:40:45 alexb Exp $
/**
 * @file
 * Class defition for data_node_views_handler_field_item_count.
 */

class data_node_views_handler_field_item_count extends views_handler_field {
  function query() { return; }

  function pre_render($result) {
    $this->items = array();
    foreach ($result as $row) {
      if (!empty($row->{$this->view->base_field})) {
        $nid = $row->{$this->view->base_field};
        $this->items[$nid] = db_result(db_query("SELECT COUNT(id) AS count FROM {data_table_node} WHERE nid = %d", $nid));
      }
    }
  }

  function render($values) {
    if (!empty($values->{$this->view->base_field})) {
      $nid = $values->{$this->view->base_field};
      if (isset($this->items[$nid])) {
        return check_plain($this->items[$nid]);
      }
    }
    return 0;
  }
}
