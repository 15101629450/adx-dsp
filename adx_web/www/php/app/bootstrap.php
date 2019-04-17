<?php

class bootstrap extends adx_bootstrap {

	function init() {

		session_start();
		header('Content-type: application/json');
	}

	function login_check() {

		if (!($_REQUEST['packet_type'] == 'login')) {

			if ($_REQUEST['manager_id'] == 'bd1f53a0-63c0-11e6-bb80-17d3ddd7a09b') {

				if ($_REQUEST['manager_id'] != $_SESSION['manager_id'])
					die('err');

			} else {

				if (empty($_SESSION['account_id']))
					die('err');

				if ($_SESSION['account_id'] != $_REQUEST['account_id'])
					die('err');
			}
		}
	}

	function db_init() {

		$GLOBALS['db'] = new medoo([
			'database_type' => 'mysql',
			'database_name' => $this->conf['mysql.base'],
			'server' => $this->conf['mysql.host'],
			'username' => $this->conf['mysql.user'],
			'password' => $this->conf['mysql.pass'],
			'charset' => 'utf8'
			]);

	}
}



