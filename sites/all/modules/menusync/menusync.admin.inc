<?php
// $Id: menusync.admin.inc,v 1.2.2.4 2010/07/09 12:44:13 rsevero Exp $

/**
 * @file
 * Admin forms to menusync module.
 *
 * @ingroup menusync
 *
 */

/*
 * Page that lists all non-system menus and the number of their menusync rules.
 */
function menusync_overview_menus() {
  $menus = menusync_get_menus();
  $form = array('#tree' => TRUE);
  $form['menus'] = array(
    '#type' => 'fieldset',
    '#collapsible' => FALSE,
    '#title' => 'Menus',
    '#theme' => 'menusync_menu_list_form',
    '#tree' => TRUE
  );
  foreach ($menus as $menu) {
    //$form['menus'][$menu['menu_name']]['#menu'] = (array)$menu;
    $form['menus'][$menu['menu_name']]['title'] = array('#value' => check_plain($menu['title']));
    $form['menus'][$menu['menu_name']]['counter'] = array('#value' => $menu['counter']);
    $form['menus'][$menu['menu_name']]['list'] = array('#value' => l(t('list rules'), "admin/content/menusync/{$menu['menu_name']}/list"));
    $form['menus'][$menu['menu_name']]['add'] = array('#value' => l(t('add rule'), "admin/content/menusync/{$menu['menu_name']}/add/rule"));
  }
  return $form;
}

/*
 * Theme form that allows to edit or order a menu list of rules.
 *
 * @see menusync_overview_menus()
 */
function theme_menusync_menu_list_form($form) {
  $rows = array();
  foreach (element_children($form) as $key) {
    if (isset($form[$key]['title'])) {
      $menu = &$form[$key];
      $row = array();
      $row[] = drupal_render($menu['title']);
      $row[] = drupal_render($menu['counter']);
      $row[] = drupal_render($menu['list']);
      $row[] = drupal_render($menu['add']);
      $rows[] = array('data' => $row, 'class' => 'draggable');
    }
  }
  if (empty($rows)) {
    $rows[] = array(array('data' => t('No menus available.'), 'colspan' => '5'));
  }

  $header = array(t('Menu'), t('Rules'));
  $header[] = array('data' => t('Operations'), 'colspan' => '2');
  return theme('table', $header, $rows, array('id' => 'menusync')) . drupal_render($form);
}

/*
 * Menu callback; edit or order a menu list of rules
 *
 * @see theme_menusync_overview_rules()
 * @see menusync_overview_rules_submit()
 */
function menusync_overview_rules(&$form_state, $menu) {
  drupal_set_title(t('Rules in %menu', array('%menu' => $menu['title'])));
  $form = array('#tree' => TRUE);
  $rules = menusync_get_rules($menu);

  $menu_options = menu_parent_options(
    array($menu['menu_name'] => $menu['title']),
    array('mlid' => 0)
  );

  foreach ($rules as $key => $rule) {
    $form[$key]['#rule'] = (array) $rule;
    $form[$key]['edit'] = array('#value' => l(t('edit'), "admin/content/menusync/edit/rule/$rule->msid", array('query' => drupal_get_destination())));
    $form[$key]['path'] = array('#value' => $rule->path);
    $form[$key]['link_title'] = array('#value' => $rule->link_title);
    $form[$key]['description'] = array('#value' => $rule->description);
    $form[$key]['weight'] = array('#type' => 'weight', '#delta' => 10,  '#default_value' => $rule->weight);
  }
  $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save')
    );
  return $form;
}

/*
 * Theme form to edit or order a menu list of rules
 *
 * @see menusync_overview_rules()(
 * @see menusync_overview_rules_submit()
 */
function theme_menusync_overview_rules($form) {
  $rows = array();
  foreach (element_children($form) as $key) {
    if (isset($form[$key]['link_title'])) {
      $rule = &$form[$key];
      $row = array();
      $row[] = drupal_render($rule['link_title']);
      $row[] = drupal_render($rule['path']);
      $row[] = drupal_render($rule['description']);
      if (isset($rule['weight'])) {
        $rule['weight']['#attributes']['class'] = 'rule-weight';
        $row[] = drupal_render($rule['weight']);
      }

      $row[] = drupal_render($rule['edit']);
      $rows[] = array('data' => $row, 'class' => 'draggable');
    }
  }
  if (empty($rows)) {
    $rows[] = array(array('data' => t('No rules available.'), 'colspan' => '5'));
  }
  $header = array(t('Menu item '), t('Path'), t('Description'), t('Weight'));
  $header[] = array('data' => t('Operations'), 'colspan' => '1');
  drupal_add_tabledrag('menusync', 'order', 'sibling', 'rule-weight');
  return theme('table', $header, $rows, array('id' => 'menusync')) . drupal_render($form);
}

/**
 * Submit handler for rules overview. Updates changed rules weights.
 *
 * @see menusync_overview_rules()
 * @see theme_menusync_overview_rules()
 */
function menusync_overview_rules_submit($form, &$form_state) {
  foreach ($form_state['values'] as $msid => $rule) {
    if (is_numeric($msid) && $form[$msid]['#rule']['weight'] != $form_state['values'][$msid]['weight']) {
      $form[$msid]['#rule']['weight'] = $form_state['values'][$msid]['weight'];
      menusync_save_rule($form[$msid]['#rule']);
    }
  }
}

/**
 * Menu callback; return the edit form for a new rule after setting the title.
 *
 * @see menusync_form_rule()
 */
function menusync_add_rule_page($menu) {
  drupal_set_title(t('Add rule to %menu', array('%menu' => $menu['title'])));
  $list = menu_parent_options(array($menu['menu_name'] => $menu['title']), array('mlid' => 0));
  $menu['items']=$list;
  return drupal_get_form('menusync_form_rule' , $menu);
}

/**
 * Menu callback; return the edit form for a rule after setting the title.
 *
 * @see menusync_form_rule()
 */
function menusync_admin_rule_edit($msid) {
  $rule = menusync_get_rule($msid);
  if ($rule) {
    $menu = menusync_load_menu_from_rule($rule);
    drupal_set_title(t('Set rule to %menu', array('%menu' => $menu['title'])));
    return drupal_get_form('menusync_form_rule', $menu, $rule);
  }
  return drupal_not_found();
}

/**
 * Form to insert/update rule.
 *
 * @see menusync_admin_rule()
 */
function menusync_form_rule(&$form_state, $menu, $rule = array()) {
  $rule += array(
    'msid' => NULL,
    'mlid' => NULL,
    'menu_name' => $menu['menu_name'],
    'path' => '',
    'description' => '',
    'weight' => 0,
  );
  $form['#rule'] = $rule;
  // Check for confirmation forms.
  if (isset($form_state['confirm_delete'])) {
    return array_merge($form, menusync_rule_confirm_delete($form_state, $rule['msid']));
  }
  $form['menu_name'] = array(
    '#type' => 'hidden',
    '#value' => $menu['menu_name'],
  );
  $form['msid'] = array('#type' => 'value', '#value' => $rule['msid']);
  $form['menu_link'] = array(
    '#type' => 'select',
    '#options' => $menu['items'],
    '#description' => t('Choose the menu item to be activated by the path.'),
    '#title' => 'Menu item',
    '#default_value' => "{$menu['menu_name']}:{$rule['mlid']}",
    '#required' => TRUE,
  );
  $form['path'] = array(
    '#type' => 'textfield',
    '#description' => t('Type a path that activates the menu item (wildcard * admitted).'),
    '#title' => t('Path'),
    '#default_value' => $rule['path'],
    '#required' => TRUE,
  );
  $form['description'] = array(
    '#type' => 'textarea',
    '#description' => t('Description of your Menu Sync rule.'),
    '#title' => t('Description'),
    '#default_value' => $rule['description'],
  );
  $form['weight'] = array(
    '#type' => 'weight',
    '#description' => t('The lighter rules will be tested first.'),
    '#title' => t('Weight'),
    '#default_value' => $rule['weight'],
    '#required' => TRUE,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  if ($rule['msid']) {
    $form['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
    );
  }
  return $form;
}

/**
 * Validate handler for rules form.
 *
 * @see menusync_form_rule()
 */
function menusync_form_rule_validate($form, &$form_state) {
  if (trim($form_state['values']['path'] == '')) {
    form_set_error('path', t('Path cannot be empty.'));
  }
  if (trim($form_state['values']['path'][0] == '/')) {
    form_set_error('path', t('Path cannot start with slash.'));
  }
  if ($form_state['values']['menu_link'] == $form_state['values']['menu_name'] . ':0') {
    form_set_error('menu_link', t('You have to choose a menu item.'));
  }
}

/**
 * Submit handler for rules overview. Updates changed rules weights.
 *
 * @see menusync_form_rule()
 */
function menusync_form_rule_submit($form, &$form_state) {
  if ($form_state['clicked_button']['#value'] == t('Delete')) {
    // Execute the rule deletion.
    if ($form_state['values']['msid']) {
      if ($form_state['values']['delete'] === TRUE) {
        return menusync_rule_confirm_delete_submit($form, $form_state);
      }
    }
    // Rebuild the form to confirm term deletion.
    $form_state['rebuild'] = TRUE;
    $form_state['confirm_delete'] = TRUE;
    return;
  }
  switch (menusync_save_rule($form_state['values'])) {
    case SAVED_NEW:
      drupal_set_message(
        t('Created new rule with path %path.',
          array('%path' => $form_state['values']['path']
      )));
      watchdog(
        'menusync',
        'Created new rule with path %path.',
        array('%path' => $form_state['values']['path']),
        WATCHDOG_NOTICE,
        l(t('edit'),
        'admin/content/menusync/edit/rule/' . $form_state['values']['msid']
      ));
      break;

    case SAVED_UPDATED:
      drupal_set_message(
        t('Updated rule with path %path.',
          array('%path' => $form_state['values']['path']
      )));
      watchdog(
        'menusync',
        'Updated rule with path %path.',
        array('%path' => $form_state['values']['path']),
        WATCHDOG_NOTICE,
        l(
          t('edit'),
          'admin/content/menusync/edit/rule/'. $form_state['values']['msid']
      ));
      break;

  }
  $form_state['redirect'] = "/admin/content/menusync/{$form_state['values']['menu_name']}/list";
  return;
}

/**
 * Confirmation handler to delete rule.
 *
 * @see menusync_form_rule()
 */
function menusync_rule_confirm_delete(&$form_state, $msid) {
  $rule = menusync_get_rule($msid);
  $form['link_title'] = array('#type' => 'value', '#value' => $rule['link_title']);
  $form['path'] = array('#type' => 'value', '#value' => $rule['path']);
  $form['description'] = array('#type' => 'value', '#value' => $rule['description']);
  $form['msid'] = array('#type' => 'value', '#value' => $msid);
  $form['delete'] = array('#type' => 'value', '#value' => TRUE);
  return confirm_form(
    $form,
    t('Are you sure you want to delete the rule to path %path to menu item %link_title?',
      array('%path' => $rule['path'], '%link_title' => $rule['link_title'])),
    'admin/content/menusync',
    t('Deleting a rule cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Submit handler to delete rule.
 *
 * @see menusync_rule_confirm_delete()
 */
function menusync_rule_confirm_delete_submit($form, &$form_state) {
  menusync_del_rule($form_state['values']['msid']);
  drupal_set_message(
    t('Deleted rule %name.',
      array('%name' => $form_state['values']['path'])
  ));
  watchdog(
    'menusync',
    'Deleted rule %name.',
    array('%name' => $form_state['values']['path']),
    WATCHDOG_NOTICE
  );
  $form_state['redirect'] = 'admin/content/menusync';
  return;
}

/**
 * Load a menu associated to a given rule.
 */
function menusync_load_menu_from_rule($rule) {
  $menu = menu_load($rule['menu_name']);
  $list = menu_parent_options(
    array($menu['menu_name'] => $menu['title']),
    array('mlid' => 0)
  );
  $menu['items']=$list;
  return $menu;
}

/**
 * Load rule.
 */
function menusync_get_rule($msid) {
  $result = db_query("SELECT msr.msid, msr.mlid, msr.menu_name, msr.path,
      msr.weight, msr.description, ml.link_title
    FROM {menusync_rules} AS msr
      LEFT JOIN {menu_links} As ml ON ml.mlid = msr.mlid
    WHERE msr.msid='%s'", 
    $msid
  );
  return (db_fetch_array($result));
}

/**
 * Get rules of a given menu.
 */
function menusync_get_rules($menu) {
  $result = db_query("SELECT msr.msid, msr.mlid, msr.path, msr.weight,
      msr.description, ml.link_title
    FROM {menusync_rules} AS msr
      LEFT JOIN {menu_links} As ml ON ml.mlid = msr.mlid
    WHERE msr.menu_name='%s'
    ORDER BY msr.weight",
    $menu['menu_name']
  );
  $rows = array();
  while ($line = db_fetch_object($result)) {
    $line->sel_value = "{$menu['menu_name']}:{$line->mlid}";
    $rows[] = $line;
  }
  return $rows;
}

/**
 * Get all non-system menus.
 */
function menusync_get_menus() {
  $system_menus = menu_list_system_menus();
  $sql = 'SELECT * FROM {menu_custom}';
  $sql .= ' WHERE menu_name NOT IN ('. implode(',', array_fill(0, count($system_menus), "'%s'")) .')';
  $sql .= ' ORDER BY title';
  $result = db_query($sql, $system_menus);
  while ($row = db_fetch_array($result)) {
    $result_counter = db_query("SELECT count(*) as total 
      FROM {menusync_rules}
      WHERE menu_name='%s'",
      $row['menu_name']
    );
    $item_counter = db_fetch_object($result_counter);
    $row['counter'] = $item_counter->total;
    $rows[] = $row;
  }
  return $rows;
}
