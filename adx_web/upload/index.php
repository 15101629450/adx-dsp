<?php

require('medoo.php');

function upload_file()
{

	header('Access-Control-Allow-Origin:*');

	if (empty($_FILES))
		die("error:1");

	if ($_FILES["file"]["error"] || !is_uploaded_file($_FILES["file"]["tmp_name"]))
		die("error:2");

	if (!$in = @fopen($_FILES["file"]["tmp_name"], "rb"))
		die("error:3");

	$id = uniqid();
	// $id = str_replace('-', '', uuid_create());
	$path = "/ad_system/images/{$id[0]}/{$id[1]}/{$id[2]}/{$id[3]}/{$id}.jpg";

	if (!$out = @fopen($path, "wb"))
		die("error:4");

	while ($buff = fread($in, 4096))
		fwrite($out, $buff);

	fclose($out);
	fclose($in);

	$img_arr = getimagesize($path);
	$img_size = $img_arr['0'] . 'X' . $img_arr[1];

	$res['img_id'] = $id;
	$res['img_size'] = $img_size;
	echo json_encode($res);
}

upload_file();

