<?php

function adx_controllers_get_campaign_one () {

	$account_id = $_REQUEST['account_id'];
	$campaign_id = $_REQUEST['campaign_id'];
	$res = $GLOBALS['db']->get(
		't_campaign',
		['account_id', 'campaign_id', 'campaign_name', 'create_time', 'update_time', 'region_type', 'region_code'],
		['and' => ['account_id' => $account_id, 'campaign_id' => $campaign_id]]
		);

	echo json_encode($res);
}


