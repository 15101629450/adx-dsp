<?php

function adx_controllers_set_campaign () {

	$ad_type = $_REQUEST['ad_type'];
	$account_id = $_REQUEST['account_id'];
	$campaign_id = $_REQUEST['campaign_id'];
	$campaign_name = $_REQUEST['campaign_name'];
	$region_type = $_REQUEST['region_type'];
	$status = $_REQUEST['status'];

	if (empty($status)) {

		if(empty($campaign_name) || ctype_space($campaign_name)) {
			die('err');
		}

		$code = '';
		foreach ($_REQUEST['region_code'] as $k=>$v) {
			$code .= $v.',';
		}
		$code = trim($code, ',');

		$res = $GLOBALS['db']->update(
			't_campaign',
			[
			'ad_type' => $ad_type,
			'campaign_name' => $campaign_name,
			'#update_time' => 'CURDATE()',
			'region_type' => $region_type,
			'region_code' => $code
			],
			['and' => ['account_id' => $account_id, 'campaign_id' => $campaign_id]]
			);

	} else {

		$res = $GLOBALS['db']->update(
			't_campaign',
			[
			'status' => $status
			],
			['and' => ['account_id' => $account_id, 'campaign_id' => $campaign_id]]
			);

	}

	print_r($_REQUEST);
}





