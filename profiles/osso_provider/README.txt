
OpenID SSO Demo (Provider)
--------------------------

Drupal as OpenID provider in a web of trusted sites. Knowing the OpenID Provider
and all available Relying Parties allows for some simplifications of the sign-on
experience "Simple Sign-On". This profile uses OpenID Provider SSO to demo
Simple Sign-On.

Use together with OpenID SSO Demo (Relying Party) profile.

Installation
------------

1.

Run install profile, set up OpenID Relying Party information as described in
OpenID Provider SSO module README.

2. (optional)

Go to node/add/activity-feed and add a feed for activity/comments and for
activity/posts for every Relying Party. Set the activity type ("Post" or
"Comment" accordingly).

Example:

Say there are two Relying Parties, one is http://blogexample.com and the other
one is http://shopexample.com. Add four feed nodes:

Feed URL: http://blogexample.com/activity/posts, Activity type: "Post"
Feed URL: http://blogexample.com/activity/comments, Activity type: "Comment"
Feed URL: http://shopexample.com/activity/posts, Activity type: "Post"
Feed URL: http://shopexample.com/activity/comments, Activity type: "Comment"

3. (optional)

Create some content (story nodes, comments) on Relying Parties with different
users and hit cron on the Provider to see user activity on the Provider.