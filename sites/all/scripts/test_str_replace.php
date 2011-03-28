<?php

require('bootstrap_drupal.php');

main();

function main(){
    print preg_replace('/class="menu"/', 'id="dla-nav-menu" class="dla-nav-menu sf-menu"', menu_tree('menu-whoi-dla'), 1); 
}
  

?>
