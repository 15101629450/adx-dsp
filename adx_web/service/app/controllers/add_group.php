<?php

function adx_controllers_add_group () {

	$ad_type = $_REQUEST['ad_type'];
	$account_id = $_REQUEST['account_id'];
	$campaign_id = $_REQUEST['campaign_id'];
	$group_name = $_REQUEST['group_name'];

	if(empty($group_name) || ctype_space($group_name)) {
		die('err');
	}

	$code = '';
	foreach ($_REQUEST['disease_code'] as $k=>$v) {
		$code .= $v.',';
	}
	$code = trim($code, ',');

	$res = $GLOBALS['db']->insert(
		't_group',
		[
		'ad_type' => $ad_type,
		'account_id' => $account_id,
		'campaign_id' => $campaign_id,
		'#group_id' => 'UUID()',
		'group_name' => $group_name,
		'#create_time' => 'CURDATE()',
		'#update_time' => 'CURDATE()',
		'disease_code' => $code,
		'status' => 1
		]
		);


	print_r($_REQUEST);
}


