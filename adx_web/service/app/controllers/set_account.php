<?php

function adx_controllers_set_account() {

	$account_id = $_REQUEST['account_id'];
	$account_name = $_REQUEST['account_name'];
	$account_pass = $_REQUEST['account_pass'];
	$company_name = $_REQUEST['company_name'];
	$webim = $_REQUEST['webim'];
	$status = $_REQUEST['status'];

	if (empty($status)) {

		if(empty($account_name) || ctype_space($account_name)) {
			die('err');
		}

		$res = $GLOBALS['db']->update(
			't_account',
			[
			'account_name' => $account_name,
			'account_pass' => $account_pass,
			'company_name' => $company_name,
			'webim' => $webim,
			'#update_time' => 'CURDATE()',
			],
			['account_id' => $account_id]
			);

	} else {

		$res = $GLOBALS['db']->update(
			't_account',
			[
			'status' => $status
			],
			['account_id' => $account_id]
			);
	}

	print_r($_REQUEST);
}

