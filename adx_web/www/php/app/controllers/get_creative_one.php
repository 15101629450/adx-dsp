<?php

function adx_controllers_get_creative_one() {

	$ad_type = $_REQUEST['ad_type'];
	$account_id = $_REQUEST['account_id'];
	$creative_id = $_REQUEST['creative_id'];

	if ($ad_type == '1') {

		$res = $GLOBALS['db']->get(
			't_creative_txt',
			[
			'creative_name',
			'access_url',
			'display_url',
			'description',
			'other'
			],
			['and' => ['account_id' => $account_id, 'creative_id' => $creative_id]]
			);

	} else if ($ad_type == '2') {

		$res = $GLOBALS['db']->get(
			't_creative_img',
			[
			'creative_name',
			'access_url',
			'display_url',
			'img_id',
			'other'
			],
			['and' => ['account_id' => $account_id, 'creative_id' => $creative_id]]
			);
	}

	echo json_encode($res);
}


