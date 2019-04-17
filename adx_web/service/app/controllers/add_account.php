<?php

function adx_controllers_add_account() {

	$account_name = $_REQUEST['account_name'];
	$account_pass = $_REQUEST['account_pass'];
	$company_name = $_REQUEST['company_name'];
	$webim = $_REQUEST['webim'];

	if(empty($account_name) || ctype_space($account_name)) {
		die('err');
	}

	$res = $GLOBALS['db']->insert(
		't_account',
		[
		'#account_id' => 'UUID()',
		'account_name' => $account_name,
		'account_pass' => $account_pass,
		'company_name' => $company_name,
		'webim' => $webim,
		'#create_time' => 'CURDATE()',
		'#update_time' => 'CURDATE()',
		]
		);
}

