<?php

function adx_controllers_add_creative () {

	$ad_type = $_REQUEST['ad_type'];
	$account_id = $_REQUEST['account_id'];
	$group_id = $_REQUEST['group_id'];
	$creative_name = $_REQUEST['creative_name'];
	$access_url = $_REQUEST['access_url'];
	$display_url = $_REQUEST['display_url'];
	$description = $_REQUEST['description'];
	$other = $_REQUEST['other'];
	$img_id = $_REQUEST['img_id'];
	$img_size = $_REQUEST['img_size'];

	if(empty($creative_name) || ctype_space($creative_name)) {
		die('err');
	}

	if ($ad_type == '1') {

		$res = $GLOBALS['db']->insert(
			't_creative_txt',
			[
			'account_id' => $account_id,
			'group_id' => $group_id,
			'#creative_id' => 'UUID()',
			'creative_name' => $creative_name,
			'#create_time' => 'CURDATE()',
			'#update_time' => 'CURDATE()',
			'access_url' => $access_url,
			'display_url' => $display_url,
			'description' => $description,
			'other' => $other
			]
			);

	} else if ($ad_type == '2') {

		$res = $GLOBALS['db']->insert(
			't_creative_img',
			[
			'account_id' => $account_id,
			'group_id' => $group_id,
			'#creative_id' => 'UUID()',
			'creative_name' => $creative_name,
			'#create_time' => 'CURDATE()',
			'#update_time' => 'CURDATE()',
			'access_url' => $access_url,
			'display_url' => $display_url,
			'img_id' => $img_id,
			'img_size' => $img_size,
			'other' => $other,
			]
			);

	} else if ($ad_type == '3') {

		$res = $GLOBALS['db']->debug()->insert(
			't_creative_brand',
			[
			'account_id' => $account_id,
			'group_id' => $group_id,
			'#creative_id' => 'UUID()',
			'creative_name' => $creative_name,
			'#create_time' => 'CURDATE()',
			'#update_time' => 'CURDATE()',
			'access_url' => $access_url,
			'display_url' => $display_url,
			'other' => $other,
			]
			);
	}

	print_r($_REQUEST);
}


