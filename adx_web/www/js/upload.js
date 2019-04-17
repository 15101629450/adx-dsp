

(function( $ ) {

	$.fn.extend({

		upload: function() {

			/* 当前id */
			var id = '#' + $(this).attr('id');

			//迭代配置
			var opt = {};
			$.each( getOption(id),function( key, value ){
				
				opt[ key ] = opt[ key ] || value; 
			});

			/* 创建对象 */
			var webUploader = new WebUploader.Uploader( opt );
			if ( !WebUploader.Uploader.support() ) {
				alert( ' 上传组件不支持您的浏览器！');	
				return false;
			}

			return webUploader;
		}
	});

	function getOption(id) {

		return {
			
			//按钮;
			pick:{
				id: id,
				label:"选择图片"
			},

			//类型限制;
			accept:{
				title:"Images",
				extensions:"gif,jpg,jpeg,bmp,png",
				mimeTypes:"image/*"
			},

			//文件上传方式
			method:"POST",
			//服务器地址;
			server:"/upload",
			//是否已二进制的流的方式发送文件，这样整个上传内容php://input都为文件内容
			sendAsBinary:false,
			// 开起分片上传。 thinkphp的上传类测试分片无效,图片丢失;
			chunked:false,

			//最大上传的文件数量, 总文件大小,单个文件大小(单位字节);
			fileNumLimit:50,
			// fileSizeLimit:5000 * 1024,
			// fileSingleSizeLimit:500 * 1024
		};
	}
	
})( jQuery );



