function scrollToActive() {
	document.getElementById("active_book").scrollIntoView({behavior: "instant", block: "start"});	
}

function resizeBooksList() {
	var bH = document.getElementById("chapters_list").offsetHeight + "px";
	document.getElementById("books_list").style.bottom = bH;
}

function genInfo(tl_fullname, tl_abbr) {
    var info =
    '<div style="margin-top:70px;color:#404c57;"><center>' +
    '<h2>This html bible uses the ' + tl_fullname + '.<br><br>' +
    '<img src="../anchor_hope.png"></img><br><br>' + 
    'And was generated with SoulAnchor,<br> X11 Bible reader tool</h2><br><br>' +
    'You can <a href="../' + tl_abbr + '.7z">download</a> this translation. ' +
    'And these versions are also available:<br>' +
    '<h3><a href="../index.html">INDEX</a></h3>' +
    '</center></div>';
    document.getElementById("content").innerHTML = info;
}

function start() {
	resizeBooksList();
	scrollToActive();
}
