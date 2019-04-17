<?php

function adx_controllers_jump () {

	$account_id = $_REQUEST['account_id'];
	$_SESSION['account_id'] = $account_id;

	$res = $GLOBALS['db']->get(
		't_account',
		['account_name', 'company_name'],
		['account_id' => $account_id]
		);

	$res['code'] = 'ok';
	echo json_encode($res);
}


