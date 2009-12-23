<?php

/**
 * Clean up comment view link field. Give site builders the option of outputting
 * the full URL instead of an anchor with a 'view' title and an absolute link.
 */
class simple_blog_handler_field_comment_link extends views_handler_field_comment_link {

  /**
   * Don't invent a 'view' text, just return a full URL if there is no text
   * defined.
   *
   * @todo:
   * Make this a patch to views.
   */
  function render($values) {
    if ($text = $this->options['text']) {
      return l($text, "node/" . $values->{$this->aliases['nid']}, array('html' => TRUE, 'fragment' => "comment-" . $values->{$this->aliases['cid']}));
    }
    return url("node/" . $values->{$this->aliases['nid']}, array('absolute' => TRUE, 'html' => TRUE, 'fragment' => "comment-" . $values->{$this->aliases['cid']}));
  }
}