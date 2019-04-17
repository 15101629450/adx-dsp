<?php

function adx_controllers_get_group() {

	$ad_type = $_REQUEST['ad_type'];
	$account_id = $_REQUEST['account_id'];
	$campaign_id = $_REQUEST['campaign_id'];

	$res = $GLOBALS['db']->select(
		't_group',
		[
		'account_id', 
		'campaign_id', 
		'group_id',
		'group_name', 
		'create_time', 
		'update_time',
		'status'
		],
		['and' => ['ad_type' => $ad_type, 'account_id' => $account_id, 'campaign_id' => $campaign_id]]
		);

	echo json_encode($res);
}


