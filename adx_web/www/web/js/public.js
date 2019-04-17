
$(document).ready(function() {

 $(":button").css("background-color","#99BFE6");
 $("table").tablesorter({ theme : 'blue'});


 // get info
 var str = "广西玉林华美整形 <br>广西玉林华美整形 <br> <a href=\"#\">退出</a>";
 $("#user_info").html(str);

// get_guide
// var str = "<a href=\"#\">修改推广计划</a> >> <a href=\"#\">修改推广组</a>";
// $("#data_guide").html(str);





function get_tree_list() {

  var json = [
  {
    "campaign":"计划1",
    "group" : [
    "组1",
    "组2"
    ]
  },

  {
    "campaign":"计划2",
    "group" : [
    "组3",
    "组4"
    ]
  },

  {
    "campaign":"计划3",
    "group" : [
    "组5",
    "组6"
    ]
  }
  ];

  var branches = "";
  for (var i = 0; i < json.length; i++) {

    branches += "<li><span class='folder'>" + json[i].campaign + "</span><ul>";

    for (var j = 0; j < json[i].group.length; j++) {

     branches += "<li><span class='file'>" + json[i].group[j] + "</span></li>";

   }

   branches += "</ul></li>";
 }

 var branches = $(branches).appendTo("#browser");
 $("#browser").treeview({add: branches});
}



});





