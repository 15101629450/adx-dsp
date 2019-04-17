<?php

function adx_controllers_set_group () {

	$ad_type = $_REQUEST['ad_type'];
	$account_id = $_REQUEST['account_id'];
	$campaign_id = $_REQUEST['campaign_id'];
	$group_id = $_REQUEST['group_id'];
	$group_name = $_REQUEST['group_name'];
	$status = $_REQUEST['status'];

	if (empty($status)) {

		if(empty($group_name) || ctype_space($group_name)) {
			die('err');
		}

		$code = '';
		foreach ($_REQUEST['disease_code'] as $k=>$v) {
			$code .= $v.',';
		}
		$code = trim($code, ',');

		$res = $GLOBALS['db']->update(
			't_group',
			[
			'ad_type' => $ad_type,
			'group_name' => $group_name,
			'#update_time' => 'CURDATE()',
			'disease_code' => $code
			],
			['and' => ['account_id' => $account_id, 'group_id' => $group_id]]
			);

	} else {
		
		$res = $GLOBALS['db']->update(
			't_group',
			[
			'ad_type' => $ad_type,
			'status' => $status
			],
			['and' => ['account_id' => $account_id, 'group_id' => $group_id]]
			);
	}

	print_r($_REQUEST);
}




