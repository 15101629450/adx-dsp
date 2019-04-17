<?php

function adx_controllers_get_tree() {

	$ad_type = $_REQUEST['ad_type'];
	$account_id = $_REQUEST['account_id'];


	$res = $GLOBALS['db']->select(
		't_campaign',
		['[>]t_group' => 'campaign_id'],
		['t_campaign.campaign_id', 't_campaign.campaign_name', 't_group.group_id', 't_group.group_name'],
		['and' => ['t_campaign.account_id' => $account_id, 't_campaign.ad_type' => $ad_type]]
		);

	echo json_encode($res);
}


