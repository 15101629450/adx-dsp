<?php

function adx_controllers_add_campaign () {

	$ad_type = $_REQUEST['ad_type'];
	$account_id = $_REQUEST['account_id'];
	$campaign_name = $_REQUEST['campaign_name'];
	$region_type = $_REQUEST['region_type'];

	if(empty($campaign_name) || ctype_space($campaign_name)) {
		die('err');
	}

	$code = '';
	foreach ($_REQUEST['region_code'] as $k=>$v) {
		$code .= $v.',';
	}
	$code = trim($code, ',');

	$res = $GLOBALS['db']->insert(
		't_campaign',
		[
		'ad_type' => $ad_type,
		'account_id' => $account_id,
		'#campaign_id' => 'UUID()',
		'campaign_name' => $campaign_name,
		'#create_time' => 'CURDATE()',
		'#update_time' => 'CURDATE()',
		'region_type' => $region_type,
		'region_code' => $code
		]
		);
}


