
$(document).ready(function() {

	function login() {

		$.post('/action', {

			packet_type:'login',
			account_name: $('#account_name').val(),
			account_pass: $.md5($('#account_pass').val()),
			captcha: $('#captcha').val()
		},

		function(data) {

			if (data['code'] == 'ok') {

				localStorage['account_id'] = data['account_id'];
				localStorage['account_name'] = data['account_name'];
				localStorage['company_name'] = data['company_name'];
				localStorage['manager_id'] = data['manager_id'];
				localStorage['manager_name'] = data['manager_name'];
				document.location.href = data['jump'];

			} else {

				alert('用户名/密码 错误!');
			}
		});
	}

	$('#login_button').click(function() {

		login();
	});

	$(document).keypress(function(e) {

		if(e.which == 13) { 
			
			login();
		} 
	}); 

});


