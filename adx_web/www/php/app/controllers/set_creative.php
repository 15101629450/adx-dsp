<?php

function adx_controllers_set_creative () {

	$ad_type = $_REQUEST['ad_type'];
	$account_id = $_REQUEST['account_id'];
	$creative_id = $_REQUEST['creative_id'];
	$creative_name = $_REQUEST['creative_name'];
	$access_url = $_REQUEST['access_url'];
	$display_url = $_REQUEST['display_url'];
	$description = $_REQUEST['description'];
	$other = $_REQUEST['other'];
	$status = $_REQUEST['status'];
	
	if (empty($status)) {

		if ($ad_type == '1') {

			if(empty($creative_name) || ctype_space($creative_name)) {
				die('err');
			}

			$res = $GLOBALS['db']->update(
				't_creative_txt',
				[
				'creative_name' => $creative_name,
				'#update_time' => 'CURDATE()',
				'access_url' => $access_url,
				'display_url' => $display_url,
				'description' => $description,
				'other' => $other
				],
				['and' => ['account_id' => $account_id, 'creative_id' => $creative_id]]
				);

		} else if ($ad_type == '2') {
			$res = $GLOBALS['db']->update(
				't_creative_img',
				[
				'#update_time' => 'CURDATE()',
				'access_url' => $access_url,
				'display_url' => $display_url,
				'other' => $other
				],
				['and' => ['account_id' => $account_id, 'creative_id' => $creative_id]]
				);
		}

	} else {

		if ($ad_type == '1') {
			$res = $GLOBALS['db']->update(
				't_creative_txt',
				[
				'status' => $status
				],
				['and' => ['account_id' => $account_id, 'creative_id' => $creative_id]]
				);

		} else if ($ad_type == '2') {
			$res = $GLOBALS['db']->update(
				't_creative_img',
				[
				'status' => $status
				],
				['and' => ['account_id' => $account_id, 'creative_id' => $creative_id]]
				);
		}
	}

	print_r($_REQUEST);
}




