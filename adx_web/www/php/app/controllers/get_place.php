<?php

function adx_controllers_get_place() {

	$res = $GLOBALS['db']->select(
		't_place',
		[
		'place_id',
		'create_time',
		'update_time',
		'place_type',
		'place_size',
		'place_media',
		'place_memo'
		]
		);

	echo json_encode($res);
}

