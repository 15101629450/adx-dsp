<?php

function adx_controllers_get_account_one() {

	$account_id = $_REQUEST['account_id'];
	$res = $GLOBALS['db']->get(
		't_account',
		[
		'account_id',
		'account_name',
		'account_pass',
		'company_name',
		'webim'
		],
		['account_id' => $account_id]
		);

	echo json_encode($res);
}

