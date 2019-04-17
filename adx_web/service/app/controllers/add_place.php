<?php

function adx_controllers_add_place() {

	$place_type = $_REQUEST['place_type'];
	$place_size = $_REQUEST['place_size'];
	$place_media = $_REQUEST['place_media'];
	$place_memo = $_REQUEST['place_memo'];

	$sql = "INSERT INTO t_place (place_id, create_time, update_time, place_type, place_size, place_media, place_memo) VALUES (";
	$sql = $sql . "CONCAT('H', MD5(REPLACE(UUID(), '-', ''))),";
	$sql = $sql . "CURDATE(), CURDATE(),";
	$sql = $sql . "{$place_type},'{$place_size}','{$place_media}','{$place_memo}')";

	$res = $GLOBALS['db']->query($sql);
	print_r($_REQUEST);

}

