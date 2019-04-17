<?php

function adx_controllers_logout () {

	session_unset();
	session_destroy();
}


