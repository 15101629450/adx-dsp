<?php

function adx_controllers_get_captcha() {

	$captcha = new SimpleCaptcha();



// OPTIONAL Change configuration...
	$captcha->wordsFile = 'words/es.php';
	$captcha->session_var = 'secretword';
	$captcha->imageFormat = 'png';
	$captcha->scale = 3; $captcha->blur = true;
	$captcha->resourcesPath = "/ad_system/www/php/app/library/resources";

/*
if (!empty($_SERVER['HTTP_ACCEPT_LANGUAGE'])) {
    $langs = array('en', 'es');
    $lang  = substr($_SERVER['HTTP_ACCEPT_LANGUAGE'], 0, 2);
    if (in_array($lang, $langs)) {
        $captcha->wordsFile = "words/$lang.php";
    }
}
*/

// Image generation
$captcha->CreateImage();

}

