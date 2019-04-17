<?php

function adx_controllers_get_account() {

	$res = $GLOBALS['db']->select(
		't_account',
		[
		'account_id', 
		'account_name', 
		'company_name',
		'create_time', 
		'update_time',
		'status'
		],
		['account_id[!]' => 'bd1f53a0-63c0-11e6-bb80-17d3ddd7a09b']
		);

	echo json_encode($res);
}

