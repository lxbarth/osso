$Id: README.txt,v 1.7.2.8 2009/10/08 16:51:21 alexb Exp $

Note: Data requires CTools 6.x-dev

Data module
===========

The Data module provides

* An API for dynamically allocating tables for single-row records
* An API for insert/update/delete operations
* Automatic views integration
* Together with CTools module: exportable configurations
* Together with schema module: schema inspection and fixing

Its companion Data UI provides

* UI to add new database tables
* UI to add or alter columns to existing tables managed by Data module
* Default views for tables managed by Data module

Use Data Search module if you would like to search one or more columns of your 
data tables:

* Install Data Search
* Go to admin/content/data
* Edit table to be indexed
* Click on "Configure search" tab
* Check table columns to be indexed

Use Data Node module if you would like to relate nodes to data records:

* Install Data Node
* Go to admin/content/data
* Edit table to relate to nodes
* Click on "Relate to nodes" tab
* Pick a content type
* Pick which id in table will be related to a node id
* Optionally: 
  * Use views field handler for adding/removing a data table record to
    a node.
  * Configure Data Node block to show up in sidebar for selecting an 
    active node to add a data table record to.

RECOMMENDATIONS
===============

Check out FeedAPI Data module for using Data as a storage engine for feed items.
