<?php

$max_memory = "256M";
 
// set some server variables so Drupal doesn't freak out
$_SERVER['SCRIPT_NAME'] = '/script.php';
$_SERVER['SCRIPT_FILENAME'] = '/script.php';
$_SERVER['HTTP_HOST'] = 'dev-lighthouse.whoi.edu';
$_SERVER['REMOTE_ADDR'] = '127.0.0.1';
$_SERVER['REQUEST_METHOD'] = 'POST';
 
// act as the first user
global $user;
$user->uid = 1;
 
// change to the Drupal directory
chdir('/var/www/dev-lighthouse.whoi.edu/htdocs/dla');
 
// Drupal bootstrap throws some errors when run via command line
//  so we tone down error reporting temporarily
error_reporting(E_ERROR | E_PARSE);
 
// run the initial Drupal bootstrap process
require_once('includes/bootstrap.inc');
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
 
// restore error reporting to its normal setting
error_reporting(E_ALL);

// include node file, necessary for node generation
module_load_include('inc', 'node', 'node.pages');


/*
 helper function for deleting node w/out node access
*/
function do_node_delete($nid) {

  // Clear the cache before the load, so if multiple nodes are deleted, the
  // memory will not fill up with nodes (possibly) already removed.
  $node = node_load($nid, NULL, TRUE);

  //if (node_access('delete', $node)) {
  if (1) {
    db_query('DELETE FROM {node} WHERE nid = %d', $node->nid);
    db_query('DELETE FROM {node_revisions} WHERE nid = %d', $node->nid);

    // Call the node-specific callback (if any):
    node_invoke($node, 'delete');
    node_invoke_nodeapi($node, 'delete');

    // Clear the page and block caches.
    cache_clear_all();

    // Remove this node from the search index if needed.
    if (function_exists('search_wipe')) {
      search_wipe($node->nid, 'node');
    }
    watchdog('content', '@type: deleted %title.', array('@type' => $node->type, '%title' => $node->title));
    drupal_set_message(t('@type %title has been deleted.', array('@type' => node_get_types('name', $node), '%title' => $node->title)));
  }
}


?>
