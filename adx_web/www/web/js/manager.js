
var current_info = new Array();

$(document).ready(function() {

	current_info['ad_type'] = 1;
	current_info['manager_id'] = localStorage['manager_id'];
	current_info['manager_name'] = localStorage['manager_name'];

	dialog_init();
	get_account();
	$('#add_place').hide();

	function dialog_init() {
		dialog_close();
	}

	function dialog_show() {

		$('#dialog_show_div').show();
		$('#dialog_bg_div').show();
	}

	function dialog_close() {

		$('#___account').hide();
		$('#___place').hide();

		$('#dialog_show_div').hide();
		$('#dialog_bg_div').hide();
	}

	$("#user_info").html(
		'<span>' + current_info['manager_name'] + '</span><br>' +
		'<span>管理员</span><br>' +
		'<a id="logout" href="#">退出</a>');

	$('#nav1').click(function(){

		$('#add_account').show();
		$('#add_place').hide();
		get_account();
	});
	

	$('#nav2').click(function(){

		$('#add_account').hide();
		$('#add_place').show();
		get_place();
	});

	$('#nav3').click(function() {

		alert('暂无此功能');
	});

	$('#logout').click(function(){

		$.post('/action', {
			packet_type:"logout",
			manager_id: current_info["manager_id"]
		});

		localStorage.clear();
		document.location.href  = '/';
	});

	$('#add_account').click(function() {

		$('#___account').show();
		current_info['dialog_type'] = 'add_account';
		dialog_show();
	});

	$('#add_place').click(function() {


		$('#___place').show();
		current_info['dialog_type'] = 'add_place';
		dialog_show();
	});

	function get_account_one (account_id) {

		$.post('/action', {
			packet_type:"get_account_one",
			manager_id :current_info['manager_id'],
			account_id: account_id
		},

		function(data) {

			var account_name = data['account_name'];
			var account_pass = data['account_pass'];
			var company_name = data['company_name'];
			var webim = data['webim'];

			$('#account_name').val(account_name);
			$('#account_pass').val(account_pass);
			$('#company_name').val(company_name);
			$('#webim').val(webim);
		});
	}

	function get_account () {

		$('#data_table').html("");
		var node_tr = $('<tr></tr>');
		node_tr.append('<th>用户名称</th>');
		node_tr.append('<th>公司名称</th>');
		node_tr.append('<th>创建时间</th>');
		node_tr.append('<th>修改时间</th>');
		node_tr.append('<th>管理</th>');
		node_tr.append('<th>状态</th>');

		var node_thead = $('<thead></thead>');
		node_thead.append(node_tr);

		var node_table = $('<table class="tablesorter-blue"></table>');
		node_table.append(node_thead);
		node_table.append('<tbody id="__table"></tbody>');

		$('#data_table').html(node_table);

		$.post('/action', {
			packet_type:"get_account",
			manager_id :current_info['manager_id']
		},

		function(data) {

			for (i in data) {

				var node_row = $('<tr></tr>');
				node_row.val(data[i]["account_id"]);
				node_row.append('<td>' + data[i]["account_name"] + '</td>');
				node_row.append('<td>' + data[i]["company_name"] + '</td>');
				node_row.append('<td>' + data[i]["create_time"] + '</td>');
				node_row.append('<td>' + data[i]["update_time"] + '</td>');
				
				var node_jump = $('<td class="jump"></td>');
				node_jump.append('<a target="_blank" href="/user.html">管理</a>');
				node_jump.val(3);
				node_row.append(node_jump);

				var node_status = $('<td class="status"><img /></td>');
				node_status.val(data[i]['status']);
				node_row.append(node_status);	

				if (data[i]['status'] == 1) {
					node_status.children('img').attr('src', '/images/check_alt.png');
					
				} else {
					node_status.children('img').attr('src', '/images/x_alt.png');
				}

				node_row.children('td').each(function() {

					$(this).click(function() {

						var id = $(this).parent('tr').val();
						current_info["account_id"] = id;

						var type = $(this).val();
						if (type == 1) {

							$(this).val(2);
							$(this).children('img').attr('src', '/images/x_alt.png');
							$.post('/action', {
								packet_type:"set_account",
								manager_id: current_info["manager_id"],
								account_id: id,
								status: 2
							});

						} else if (type == 2) {

							$(this).val(1);
							$(this).children('img').attr('src', '/images/check_alt.png');
							$.post('/action', {
								packet_type:"set_account",
								manager_id: current_info["manager_id"],
								account_id: id,
								status: 1
							});

						} else if (type == 3) {

							$.post('/action', {
								packet_type:"jump",
								manager_id :current_info['manager_id'],
								account_id: id
							},

							function(data) {

								if (data['code'] == 'ok') {

									localStorage['account_id'] = id;
									localStorage['account_name'] = data['account_name'];
									localStorage['company_name'] = data['company_name'];
								}
							});

						} else {

							$('#___account').show();
							current_info['dialog_type'] = 'set_account';
							get_account_one(id);
							dialog_show();
						}
					});
				});

				$('#__table').append(node_row);	
			}
		});
	}

	function get_place () {

		$('#data_table').html("");
		var node_tr = $('<tr></tr>');
		node_tr.append('<th>广告位ID</th>');
		node_tr.append('<th>创建时间</th>');
		node_tr.append('<th>类型</th>');
		node_tr.append('<th>尺寸</th>');
		node_tr.append('<th>所属媒体</th>');
		node_tr.append('<th>描述</th>');

		var node_thead = $('<thead></thead>');
		node_thead.append(node_tr);

		var node_table = $('<table class="tablesorter-blue"></table>');
		node_table.append(node_thead);
		node_table.append('<tbody id="__table"></tbody>');

		$('#data_table').html(node_table);

		$.post('/action', {
			packet_type:"get_place",
			manager_id :current_info['manager_id']
		},

		function(data) {

			for (i in data) {

				if (data[i]["place_type"] == 1)
					var place_type = '文字';
				else if (data[i]["place_type"] == 2)
					var place_type = '图片';
				else if (data[i]["place_type"] == 3)
					var place_type = '品牌';
				else
					var place_type = '其他';

				if (data[i]["place_media"] == 1)
					var place_media = '宝典';
				else
					var place_media = '其他';

				var node_row = $('<tr></tr>');
				node_row.val(data[i]["account_id"]);
				node_row.append('<td>' + data[i]["place_id"] + '</td>');
				node_row.append('<td>' + data[i]["create_time"] + '</td>');
				node_row.append('<td>' + place_type + '</td>');
				node_row.append('<td>' + data[i]["place_size"] + '</td>');
				node_row.append('<td>' +  place_media + '</td>');
				node_row.append('<td>' + data[i]["place_memo"] + '</td>');
				$('#__table').append(node_row);	
			}
		});
	}

	function add_place () {

		var place_type = $('#place_type').val();
		var place_size = $('#place_size').val();
		var place_media = $('#place_media').val();
		var place_memo = $('#place_memo').val();

		$.post('/action', {
			packet_type:"add_place",
			manager_id :current_info['manager_id'],
			place_type: place_type,
			place_size: place_size,
			place_media: place_media,
			place_memo: place_memo
		});
	}

	$('#dialog_button_yes').click(function() {

		if (current_info['dialog_type'] == 'add_account') {

			var account_name = $('#account_name').val();
			var account_pass = $('#account_pass').val();
			var company_name = $('#company_name').val();
			var webim = $('#webim').val();

			$.post('/action', {

				packet_type:"add_account",
				manager_id :current_info['manager_id'],
				account_name :account_name,
				account_pass: account_pass,
				company_name: company_name,
				webim :webim
			});

			dialog_close();
			get_account();

		} else if (current_info['dialog_type'] == 'set_account') {

			var account_name = $('#account_name').val();
			var account_pass = $('#account_pass').val();
			var company_name = $('#company_name').val();
			var webim = $('#webim').val();

			$.post('/action', {

				packet_type:"set_account",
				manager_id :current_info['manager_id'],
				account_id: current_info["account_id"],
				account_name :account_name,
				account_pass: account_pass,
				company_name: company_name,
				webim :webim
			});

			dialog_close();
			get_account();

		} else if (current_info['dialog_type'] == 'add_place') {
			add_place();
			dialog_close();
		}
	});

	$('#dialog_button_no').click(function() {
		dialog_close();
	});

});






