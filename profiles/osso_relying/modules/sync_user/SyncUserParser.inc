<?php

/**
 * Parses a feed provided by PuSH User module.
 */
class SyncUserParser extends FeedsParser {
  /**
   * Implementation of FeedsParser::parse().
   */
  public function parse(FeedsImportBatch $batch, FeedsSource $source) {
    $xml = new SimpleXMLElement($batch->getRaw());
    $xml->registerXPathNamespace('atom', 'http://www.w3.org/2005/Atom');
    $batch->setTitle((string) $xml->title);
    $batch->setItems($xml->xpath('//atom:entry'));
  }

  /**
   * Get an element identified by $element_key of the given item.
   */
  public function getSourceElement($item, $element_key) {
    if ($item instanceof SimpleXMLElement) {
      switch ($element_key) {
        case 'name':
          return (string) $item->children('http://xmlns.com/foaf/0.1/')->Person->name;
        case 'mail':
          $resource = $item
            ->children('http://xmlns.com/foaf/0.1/')
            ->Person
            ->mbox
            ->attributes('http://www.w3.org/1999/02/22-rdf-syntax-ns#')
            ->resource;
          if ($resource) {
            return str_replace('mailto:', '', $resource);
          }
          return '';
        case 'openid':
          $resource = $item
            ->children('http://xmlns.com/foaf/0.1/')
            ->Person
            ->openid
            ->attributes('http://www.w3.org/1999/02/22-rdf-syntax-ns#')
            ->resource;
          if ($resource) {
            return str_replace('mailto:', '', $resource);
          }
          return '';
        case 'profile_url':
          return (string)$item->link->attributes()->href;
      }
    }
    return '';
  }

  /**
   * Declare mapping sources.
   */
  public function getMappingSources() {
    return array(
      'name' => array(
        'name' => t('User name'),
        'description' => t('The name of the user.'),
      ),
      'mail' => array(
        'name' => t('E-mail address'),
        'description' => t('The e-mail address of the user.'),
      ),
      'openid' => array(
        'name' => t('OpenID Identifier'),
        'description' => t('The user\'s OpenID identifier.'),
      ),
      'profile_url' => array(
        'name' => t('Profile URL'),
        'description' => t('The URL to the user\'s profile.'),
      ),
     );
  }
}