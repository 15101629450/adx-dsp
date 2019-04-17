<?php

function adx_controllers_login () {

	$account_name = $_REQUEST['account_name'];
	$account_pass = $_REQUEST['account_pass'];
	$captcha = $_REQUEST['captcha'];

	if(empty($account_name) || ctype_space($account_name)) {
		die('err');
	}

	$db = $GLOBALS['db']->get(
		't_account',
		['account_id', 'account_name', 'company_name', 'account_pass'],
		['account_name' => $account_name]
		);

	if (md5($db['account_pass']) == $account_pass) {
		
		if ($account_name == 'admin') {

			$res['jump'] = '/manager.html';
			$res['manager_id'] = $db['account_id'];
			$res['manager_name'] = $db['account_name'];
			$_SESSION['manager_id'] = $db['account_id'];

		} else {
			
			$res['jump'] = '/user.html';
			$res['account_id'] = $db['account_id'];
			$res['account_name'] = $db['account_name'];
			$res['company_name'] = $db['company_name'];
			$_SESSION['account_id'] = $db['account_id'];
		}

		$res['code'] = 'ok';

	} else {
		$res['code'] = 'err';

	}

	echo json_encode($res);
}


