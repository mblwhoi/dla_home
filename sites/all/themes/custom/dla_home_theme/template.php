<?php

/**
 * Implementation of hook_theme_breadcrumb.
 * This is intended to override the parent dla_theme_breadcrumb function.
 */
function dla_home_theme_breadcrumb($breadcrumb){

    // Remove 'home' breadcrumb (first array element) if it is present
    if (!empty($breadcrumb)) {
        array_shift($breadcrumb);
    }

    // Get sitename.
    $sitename = variable_get('site_name', 'drupal');

    // Prepend MBLWHOI Library and DLA to breadcrumbs.
    array_unshift($breadcrumb, 
                  l('MBLWHOI Library', 'http://www.mblwhoilibrary.org'),
                  l('WHOI DLA', '<front>')
                 );

    // Append page title.
    $breadcrumb[] = drupal_get_title();

    return '<div class="breadcrumb">' . implode(' » ', $breadcrumb) . '</div>';
}

