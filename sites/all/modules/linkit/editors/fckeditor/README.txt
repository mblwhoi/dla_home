; $Id: README.txt,v 1.1 2010/07/04 22:25:02 anon Exp $

##############################################
## ONLY if you use fckeditor WITHOUT wysiwyg ##
##############################################

Installation:

Do the following steps to add Linkit button to the FCKeditor toolbar:

   1. Open /drupal/modules/fckeditor/fckeditor.config.js

   2. Add this BEFORE the first "FCKConfig.ToolbarSets" array.
     
      var linkit_basePath = '/' // Change this if you have drupal installed in a subdir

      var linkit_url_fckeditor = 'admin/linkit/dashboard/fckeditor'; // DO NOT CHANGE
      FCKConfig.Plugins.Add( 'linkit', null, linkit_basePath + 'sites/all/modules/linkit/editors/fckeditor/') ; // DO NOT CHANGE

   3. Add button to the toolbar. The button name is: linkit (NOTICE: small "L" is needed)
      For example if you have a toolbar with an array of buttons defined as follows:

      ['Link','Unlink','Anchor'],

      simply add the button somewhere in the array:

      ['linkit','Link','Unlink','Anchor'],

      (remember about single quotes).