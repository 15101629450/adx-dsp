<?php

function adx_controllers_get_creative() {

	$ad_type = $_REQUEST['ad_type'];
	$account_id = $_REQUEST['account_id'];
	$group_id = $_REQUEST['group_id'];

	if ($ad_type == '1') {

		$res = $GLOBALS['db']->select(
			't_creative_txt',
			[
			'creative_id',
			'creative_name',
			'create_time',
			'update_time',
			'status'
			],
			['and' => ['account_id' => $account_id, 'group_id' => $group_id]]
			);

	} else  if ($ad_type == '2') {

		$res = $GLOBALS['db']->select(
			't_creative_img',
			[
			'creative_id',
			'img_id',
			'create_time',
			'update_time',
			'status'
			],
			['and' => ['account_id' => $account_id, 'group_id' => $group_id]]
			);
	}

	echo json_encode($res);
}


