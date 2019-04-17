<?php

function adx_controllers_get_group_one() {

	$account_id = $_REQUEST['account_id'];
	$group_id = $_REQUEST['group_id'];

	$res = $GLOBALS['db']->get(
		't_group',
		['account_id', 'campaign_id', 'group_id', 'group_name', 'create_time', 'update_time', 'disease_code'],
		['and' => ['account_id' => $account_id, 'group_id' => $group_id]]
		);

	echo json_encode($res);

}


