
$(document).ready(function() {
	
	$('#demo').click(function(){

		$.post('http://127.0.0.1/login.php', {

			user:"lcglcg",
			pass:"123456"
		},
		function(data){
			alert("Data: " + data);
		});
	});
});


