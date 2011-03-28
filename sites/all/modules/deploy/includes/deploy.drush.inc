<?php
/**
 * @file
 * Drush integration for the Deploy module.
 */

/**
 * Implementation of hook_drush_command().
 */
function deploy_drush_command() {
  $items = array();
  $items['deploy'] = array(
    'description' => 'Executes a deployment plan',
    'options' => array (
      'show-progress' => 'Will cause progress information to be printed to stdout',
    ),
  );
  return $items;
}


/**
 * Deploy a plan from the command line with drush.
 */
function drush_deploy() {
  global $user;
  $args = func_get_args();
  $vars = array();

  // Check to see if we should list 'internal' plans as an option.
  $internal = $args[0] == 'all' ? TRUE : FALSE;
  $plans = deploy_get_plans($internal);

  // If there are plans...
  if (!empty($plans)) {
    
    // List plans and ask which one to deploy. Save the response in $vars.
    foreach ($plans as $plan) {
      $plan_options[$plan['pid']] = $plan['name'];
    }
    if (!($plan_id = drush_choice($plan_options, dt('Which plan would you like to deploy?')))) {
      return;
    }
    $vars['!plan'] = $plan_options[$plan_id];
  
    // List servers and ask which one to deploy to. Save the response in $vars.
    $server_options = deploy_get_servers();
    if (!$server_id = drush_choice($server_options, dt('Which server would you like to deploy to?'))) {
      return;
    }
    $vars['!server'] = $server_options[$server_id];
  
    // Ditto for username and password.
    $username = drush_prompt(dt('What username would you like to use'), $user->name);
    $vars['!username'] = $username;
    $pass = drush_prompt(dt('What password would you like to use for this user'));
  
    // Confirm
    if (drush_confirm(dt('Deploy !plan to !server using user !username', $vars))) {
      
      // For anyone looking to work with the Deploy API, this is a pretty 
      // complete tutorial.
      
      // Initialize setup variables
      deploy_init_deployment($plan_id, $server_id);
      
      // Attempt to get a session on the remote server. 
      if (deploy_set_session($username, $pass)) {
        $items = deploy_get_plan_items($plan_id);
        $item_count = count($items);
        $count = 0;
  
        // Run through the items in our plan and run the dependency checking
        // hooks.
        foreach ($items as $item) {
          $count++;
          deploy_plan_check_item($item['module'], $item['data']);
        }
        drush_log(dt('!count items checked', array('!count' => $count)), 'ok', FALSE);
        
        // Run the post-check cleanup routines.
        module_invoke_all('deploy_check_cleanup', $plan_id);
  
        // The plan items can change (and almost certainly have 
        // changed) as a result of plan cleanup, so get the list again.
        $items = deploy_get_plan_items($plan_id);
        $item_count = count($items);
        $count = 0;

        // Go through this list and deploy each item one by one.
        foreach ($items as $item) {
          $count++;
          drush_log(dt('Deploying item: !item', array('!item' => $item['description'])), 'ok', FALSE);
          deploy_item($item);
        }
        $deploy_log_id = variable_get('deploy_log_id', NULL);

        // Run the post-deployment cleanup routines. 
        deploy_plan_cleanup();

        // Hey look we're done.
        // @todo: make this report a link to the log details for the lazy.
        drush_log(dt('Deployment completed, check deployment log !log_id for results'), array('!log_id' => $deploy_log_id), 'ok', FALSE);
      }
      else {
        drush_log(dt('Failure setting up deploy session'), 'error', TRUE);
      }
    }
  }
  else {
    drush_print(dt('There are no plans to deploy'));
  }
}