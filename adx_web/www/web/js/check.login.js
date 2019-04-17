
$(document).ready(function() {
	
	$.post('/check.login.php', {

		user:"lcglcg",
		pass:"123456"
	},
	function(data){

		var code = data['code'];
		if (code == "ok") {
			location.href="/home.html";
		} else {
			location.href="/login.html";
		}
	});
});


