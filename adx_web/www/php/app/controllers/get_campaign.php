<?php

function adx_controllers_get_campaign () {

	$ad_type = $_REQUEST['ad_type'];
	$account_id = $_REQUEST['account_id'];

	$res = $GLOBALS['db']->select(
		't_campaign',
		[
		'account_id', 
		'campaign_id', 
		'campaign_name', 
		'create_time', 
		'update_time',
		'status'
		],
		['and' => ['account_id' => $account_id, 'ad_type' => $ad_type]]
		);

	echo json_encode($res);
}


