This is a preliminary module to support the XRDS-Simple spec.

The draft spec can be found here:
http://xrds-simple.net/core/1.0/

The module defines a single hook: hook_xrds()

Example implentation as follows:

function example_xrds() {
  $xrds['example'] = array(
    'services' => array(
      array('priority' => 10,
            'data' => array(
                'Type' => array('http://specs.openid.net/auth/2.0/signon'),
                'URI' => array('http://example.com/server')
                )
            )
      )
    );

  return $xrds;
}
