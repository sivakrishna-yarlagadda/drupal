<?php

function file_upload_callback($form, &$form_state) {
  
  $img_dir = drupal_get_path('module', 'file_upload') . '/images';
  
  $form['#prefix'] =  "<table>";
  $form['#suffix'] =  "</table>";
  
  $form['client_id'] = array(
    '#type' => 'password',
    '#title' => t('Testing'),
    //'#required' => TRUE,
    '#size' => 45,
    '#prefix'=> '<tr><td>',
    '#suffix'=> '</td>',
  );
  $form['imp_manager'] = array(
    '#type' => 'textfield',
    '#title' => t('Testing2'),
    //'#required' => TRUE,
    '#size' => 45,
    '#prefix'=> '<td>',
    '#suffix'=> '</td></tr>',
  );
  
  /*$form['file_title1'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    //'#required' => TRUE,
    '#size' => 45,
    '#prefix'=> '<tr><td>',
    '#suffix'=> '</td>',
  );
  $form['file_title2'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    //'#required' => TRUE,
    '#size' => 45,
    '#prefix'=> '<td>',
    '#suffix'=> '</td></tr>',
  );*/
  $form['upload_item1'] = array(
		'#type' => 'file',
    '#prefix'=>'<tr><td><img src="'.$img_dir.'/tls.png" width="80" height="80"><br><span class="txt-upform">This a sample text for demo purpose.</span>',
    '#suffix'=>'</td>',
	);
  $form['upload_item2'] = array(
		'#type' => 'file',
    '#prefix'=>'<td><img src="'.$img_dir.'/tls.png" width="80" height="80"><br><span class="txt-upform">This a sample text for demo purpose.</span>',
    '#suffix'=>'</td></tr>',
	);
	
	if(isset($_SESSION['uploaded_link1'])) {
		$form['uploaded_link1'] = array(
			'#type' => 'markup',
			'#markup' => '<a href="' . $_SESSION['uploaded_link1'] . '">' . $_SESSION['uploaded_link1'] . '</a>',
      '#prefix'=>'<tr><td>',
      '#suffix'=>'</td>',
		);
		unset($_SESSION['uploaded_link1']);
	}
  if(isset($_SESSION['uploaded_link2'])) {
		$form['uploaded_link2'] = array(
			'#type' => 'markup',
			'#markup' => '<a href="' . $_SESSION['uploaded_link2'] . '">' . $_SESSION['uploaded_link2'] . '</a>',
      '#prefix'=>'<td>',
      '#suffix'=>'</td></tr>',
		);
		unset($_SESSION['uploaded_link2']);
	}

	$form['upload_submit'] = array(
		'#type' => 'submit',
		'#value' => 'Upload',
		'#submit' => array('upload_file_submit_callback'),
    '#prefix'=>'<tr><td>',
    '#suffix'=>'</td></tr>',
    
	);
	
	return $form;
	
}

function upload_file_submit_callback($form, &$form_state) {
	
	$validators = array(
		'file_validate_size' => array(2*1024*1024),
		'file_validate_extensions' => array("jpg png doc docx pdf gif jpeg txt")
	);
	
	$location = 'public://';
	
	$newfile = file_save_upload('upload_item1', $validators);
	
	if($uploadCheck) {
		$link = file_create_url($uploadCheck->uri);
		$_SESSION['uploaded_link1'] = $link;
    if ($uploadCheck->fid > 0) {
      db_insert('file_upload_extra')
      ->fields(array(
        'fid' => $uploadCheck->fid,
        'client_id' => $form_state['values']['client_id'],
        'imp_mgr' => $form_state['values']['imp_manager'],
      ))
      ->execute();
    }
	}
  $filename1 = pathinfo($link, PATHINFO_BASENAME);
 
  if($newfile) {
    $some_location = 'public://';
    $des = $some_location . '-' . time() . $newfile->filename; // Define the new location and add the time stamp to file name.

    $result = file_copy($newfile, $des, FILE_EXISTS_REPLACE);
    if ($result) {
      // Succeed.
    }
    else {
      // Fail.
    }

  }
 
 
 
 
  $uploadCheck = file_save_upload('upload_item2', $validators, $location);
	if($uploadCheck) {
		$link = file_create_url($uploadCheck->uri);
		$_SESSION['uploaded_link2'] = $link;
    if ($uploadCheck->fid > 0) {
      db_insert('file_upload_extra')
      ->fields(array(
        'fid' => $uploadCheck->fid,
        'client_id' => $form_state['values']['client_id'],
        'imp_mgr' => $form_state['values']['imp_manager'],
      ))
      ->execute();
    }
	}
  $filename2 = pathinfo($link, PATHINFO_BASENAME);
 
  
  $files_dir = file_stream_wrapper_get_instance_by_uri('public://')->realpath();
   
}

function upload_file_git_deploy($filename, $files_dir) {
  /**
	 * GIT DEPLOYMENT SCRIPT
	 *
	 * Used for automatically deploying websites via github or bitbucket, more deets here:
	 * https://gist.github.com/riodw/71f6e2244534deae652962b32b7454e2
	 * How To Use:
	 * https://medium.com/riow/deploy-to-production-server-with-git-using-php-ab69b13f78ad
	 */
	// The commands
  if ($files_dir != getcwd()) {
    chdir($files_dir);
  }
  $output = '';
  shell_exec('git clone https://github.com/gitdv/test.git');
  $user = trim(shell_exec('whoami'));
  $email = 'no-replay@example.com';
  chdir($files_dir.'\\test');
	$commands = array(
    'cd',// Display current working directory
		'whoami',//Display username
		'git pull origin master',
		'git status',
    'move '. $files_dir .'\\' . $filename . ' '. $files_dir .'\\test',
		'git add .',
		'git -c user.name="'. $user .'" -c user.email="'. $email .'" commit -m "Added file '. $filename .'"',
    'git push https://gitdv:#abcd1234@github.com/gitdv/test.git master',
    //'rmdir /s /q '. $files_dir . '\\test',//Remove git cloned directory
	);
	// Run the commands for output
	
	foreach($commands AS $command){
		// Run it
		$tmp = shell_exec($command);
		// Output
		$output .= "<span style=\"color: #6BE234;\">\$</span> <span style=\"color: #729FCF;\">{$command}\n</span> >> ";
		$output .= isset($tmp) ? htmlentities(trim($tmp)) . "<br>" : 'DONE<br>';
	}
	// Make it pretty for manual user access (and why not?)
?>
<!DOCTYPE HTML>
<html lang="en-US">
<head>
	<meta charset="UTF-8">
	<title>GIT DEPLOYMENT SCRIPT</title>
</head>
<body style="background-color: #000000; color: #FFFFFF; font-weight: bold; padding: 0 10px;">
<pre>
 ____________________________
|                            |
| Git Deployment Script v0.1 |
|      github.com/riodw 2017 |
|____________________________|

<?php echo $output; ?>
</pre>
</body>
</html>
<?php
//exit;
}

