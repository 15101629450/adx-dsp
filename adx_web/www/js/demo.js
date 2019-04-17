


$(document).ready(function() {



	var upload = $('#demo').upload();


	upload.on('fileQueued', function( file ) {

		$('#list').append('<div>' + file['name'] + '</div>');
	});

	/* 绑定发送按钮 */
	$('#dialog_button_yes').click(function(){

		upload.upload();
	});
	

});

