/*
 * (C) Copyright 2013 - Thales SA (author: Simon DENEL - Thales Research & Technology)
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the GNU Lesser General Public License
 * (LGPL) version 2.1 which accompanies this distribution, and is available at
 * http://www.gnu.org/licenses/lgpl-2.1.html
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
*/
function nl2br(str, is_xhtml) {
	// return a string where \n are replaced by <br />
    var breakTag = (is_xhtml || typeof is_xhtml === 'undefined') ? '<br />' : '<br>';
    //return (str + '').replace(/([^>\r\n]?)(\r\n|\n\r|\r|\n)/g, '$1' + breakTag + '$2');
	return str.replace(/([^>\r\n]?)(\r\n|\n\r|\r|\n)/g, breakTag);
}

function HTMLEncode(wText){
	if(typeof(wText)!="string") {
		wText=wText.toString();
	}
	wText=wText.replace(/&/g, "&amp;") ;
	wText=wText.replace(/</g, "&lt;") ;
	wText=wText.replace(/>/g, "&gt;") ;
	return wText;
}

function die(msg) {
	// Return for a fatal error
	alert("Fatal error: "+msg);
	$('body').html("Fatal error: "+nl2br(msg));
	throw new Error(msg);
}

function createColors() {
	// Create array containing colors
	var i = 0;

	colors[i++] = [       'grey', [ 80,  80,  80],  [ 10,  10,  10], [100, 100, 100], 'white'];
	colors[i++] = [     'yellow', [235, 235,   0],  [ 50,  50,   0], [255, 255,   0], 'black'];
	colors[i++] = [        'red', [235,   0,   0],  [100,   0,   0], [255,   0,   0], 'black'];
	colors[i++] = [      'green', [  0, 235,   0],  [  0, 100,   0], [  0, 255,   0], 'black'];
	colors[i++] = [       'blue', [  0,   0, 180],  [  0,   0, 100], [  0,   0, 255], 'black'];
	colors[i++] = [    'magenta', [235,   0, 235],  [100,   0, 100], [255,   0, 255], 'black'];
	colors[i++] = [      'azure', [  0, 107, 235],  [  0,  50, 100], [  0, 127, 255], 'black'];
	colors[i++] = ['springGreen', [  0, 235, 107],  [  0, 100,  50], [  0, 255, 127], 'black'];
	colors[i++] = [       'rose', [255,   0, 107],  [100,   0,  50], [255,   0, 255], 'black'];
	colors[i++] = [     'violet', [123,   0, 235],  [ 50,   0, 100], [142,   0, 255], 'black'];
	colors[i++] = [ 'chartreuse', [107, 235,   0],  [ 50, 100,   0], [127, 255,   0], 'black'];
	colors[i++] = [     'orange', [235, 107,   0],  [100,  50,   0], [255, 127,   0], 'black'];
	
	for(i=i; i < 50; i++) { // Creating even more colors
		vec=[Math.round(Math.random()*105+150), Math.round(Math.random()*85+150), Math.round(Math.random()*105+150)];
		colors[i]=['noname', vec, [vec[0]-120, vec[1]-120, vec[2]-120], [vec[0]+20, vec[1]+20, vec[2]+20], 'black'];
		// Try to put the text in white when required
		if(vec[0]+vec[1]+vec[2] < 200) {
			color[i][4]='white';
		}
	}
}

function colorize(obj, p, color, text) {
	// Set the right color for an object, and set the text above.
	// This function uses the canvas notion from HTML5
	// obj: HTML object to colorize (=one task)
	// color: index from colors[]
	// text: text to write above
	
	
	//$('#'+obj).html('<canvas id="canvas-'+obj+'" style="position:absolute;background-color:green; padding:0px;margin:0px;top:0px;left:0px;" height="'+$('#'+obj).css('height')+'" width="'+$('#'+obj).css('width')+'" onMouseOver="colorize(\''+obj+'\', 4, '+color+', \''+text+'\');" onMouseOut="colorize(\''+obj+'\', 2, '+color+', \''+text+'\');"></canvas>');
	
	if(color==undefined) color=0;

	var width = Math.round($('#'+obj+'').width());
	var height = Math.round($('#'+obj+'').height());

	$('#'+obj+'').width  = width;
	$('#'+obj+'').height = height;

	var canvas = $('#'+obj+'')[0];
	canvas.width  = width;
	canvas.height = height;
	var ctx = canvas.getContext('2d');
	
	


	
	var c1=colors[color][1];
	var c2=colors[color][2];
	if(p>2) c1=colors[color][3];
	for(var x = 0; x < (height-1)/2; x++) {
		var r = (1.0*x/height-0.5)*2;
		r = Math.pow(Math.abs(r), p);
		if(r > 1) r = 1;
		r=1-r;
		var red   = Math.round(1.0*c1[0]*r+c2[0]*(1-r));
		var green = Math.round(1.0*c1[1]*r+c2[1]*(1-r));
		var blue  = Math.round(1.0*c1[2]*r+c2[2]*(1-r));
		ctx.fillStyle = "rgb("+red+','+green+','+blue+")";
		ctx.fillRect(0, x, width, 1);
		ctx.fillRect(0, height-1-x, width, 1);
	}

	if(text!=undefined) if(width>10) {
		var size= 15;
		var width2=width;
		while(width2>width-7) {
			size -= 1;
			ctx.font = size+'px Arial';
			ctx.textAlign = 'center';
			ctx.fillStyle = colors[color][4];
			var metrics = ctx.measureText(text);
			width2 = metrics.width;
		}
		if(size>7)
			ctx.fillText(text, width/2, parseInt((height+size)/2));
	}
	/*
	*/
}

function fillInfos(str, color) {
	color=color.split(',');
	colorS1='rgb('+color[0]+', '+color[1]+', '+color[2]+')';
	color[0]=Math.min(255, color[0]+220);
	color[1]=Math.min(255, color[1]+220);
	color[2]=Math.min(255, color[2]+220);
	colorS2='rgb('+color[0]+', '+color[1]+', '+color[2]+')';
	$('#details').html(str);
	$('#details').css('background-color', colorS1);
	setTimeout(function() {
		$('#details').css('background-color', colorS2);
	}, 50);
}

function createTaskInstances() {
	var timelineStreamsAppend = '';
	var timelineStreamsAppendCSS = '';

	for(var idStream = 0; idStream < streams.length; idStream+=1) {
		if($("#titleStream-"+idStream).css("display")!='none') {
			var top = $("#titleStream-"+idStream).offset().top-$("#timelineStreams").offset().top;
			var tOffset = $("#timelineStreams").offset();
			var tWidth  = $("#timelineStreams").width();
			var streamHeight = Math.round($("#titleStream-"+idStream).height());
			if(streams[idStream]!=undefined) for(var i = 0; i < streams[idStream].length; i++) {
				//var x = $(this).offset();
				name       = streams[idStream][i][1];
				start      = streams[idStream][i][2];
				end        = streams[idStream][i][3];
				gws        = ''+streams[idStream][i][4];
				lws        = ''+streams[idStream][i][5];
				bw         = streams[idStream][i][6];
				ocl_queued = streams[idStream][i][7]; if(ocl_queued=='') ocl_queued=-1;
				ocl_submit = streams[idStream][i][8]; if(ocl_submit=='') ocl_submit=-1;
				size       = streams[idStream][i][9];
				//s = zoom*tWidth/(max-min);
				//begin = s*(start-min);
				//ocl_queuedG = s*(ocl_queued-min);
				//ocl_submitG = s*(ocl_submit-min);
				//end = s*(end-min);
				task = tasks[streams[idStream][i][0]];
				duration=end-start;
				str="<strong>"+task[0]+"</strong>";
				strNoHTML="";
				str+="<table class=\\\"taskDetails\\\">";
				str+="<tr class=\\\"advanced\\\"";
				if(advanced==0) str+="style=\\\"display:none;\\\""
				str+="><td class=\\\"tdLeft\\\">Queued at</td><td class=\\\"tdRight\\\">"+ocl_queued+unit+"</td></tr>";
				if(advanced==1) strNoHTML+="Queued at: "+ocl_queued+unit+"\n";
				str+="<tr class=\\\"advanced\\\"";
				if(advanced==0) str+="style=\\\"display:none;\\\""
				str+="><td class=\\\"tdLeft\\\">Submit at</td><td class=\\\"tdRight\\\">"+ocl_submit+unit+"</td></tr>";
				if(advanced==1) strNoHTML+="Submit at: "+ocl_submit+unit+"\n";
				str+="<tr><td class=\\\"tdLeft\\\">Start at</td><td class=\\\"tdRight\\\">"+start+unit+"</td></tr>";
				strNoHTML+="Start at: "+start+unit+"\n";
				str+="<tr><td class=\\\"tdLeft\\\">End at</td><td class=\\\"tdRight\\\">"+end+unit+"</td></tr>";
				strNoHTML+="End at  : "+end+unit+"\n";
				str+="<tr><td class=\\\"tdLeft\\\">Duration</td><td class=\\\"tdRight\\\">"+duration.toFixed(2)+unit+"</td></tr>";
				strNoHTML+="Duration   : "+duration.toFixed(2)+unit+"\n";
				if(gws!='')
					str+="<tr><td class=\\\"tdLeft\\\">(OpenCL)Global_work_size</td><td class=\\\"tdRight\\\">["+gws+"]</td></tr>";
				if(lws!='')
					str+="<tr><td class=\\\"tdLeft\\\">(OpenCL)Local_work_size</td><td class=\\\"tdRight\\\">["+lws+"]</td></tr>";
				if(size!='') {
					if(parseInt(size)>1024) {
						size2 = parseInt(parseFloat(size)/1024);
						size2 = parseInt(parseFloat(size2)/1024)+'MiB ('+size2+'KiB, ';
						size = size2+size+'bytes)';
					}
					else
						size = size+'bytes';
					str+="<tr><td class=\\\"tdLeft\\\">Size</td><td class=\\\"tdRight\\\">"+size+"</td></tr>";
				}
				if(bw!='') {
					str+="<tr><td class=\\\"tdLeft\\\">Bandwidth</td><td class=\\\"tdRight\\\">"+bw+"MiB/s</td></tr>";
				}
				str+="</table>";
				//width = Math.round(end-begin);
				b=$("#timelineStreams").offset().top+15;
				color=colors[task[1]][1];
				
				color='rgb('+Math.round(color[0]/2)+', '+Math.round(color[1]/2)+', '+Math.round(color[2]/2)+')';
				timelineStreamsAppend += '<canvas id="taskInstance-'+idStream+'-'+i+'" class="taskInstance" style="position:absolute;height:'+streamHeight+'px;top:'+top+'px;border:1px black solid;" onMouseOver=\'\' title="" width="2" height="2"></canvas>\
				<script>\
					var timer_taskInstance_'+idStream+'_'+i+' = 0;\
					$("#taskInstance-'+idStream+'-'+i+'").click(function() {\
						cursor[0]='+start+';\
						fillInfos(\"'+str+'\", \"'+colors[task[1]][1]+'\");nameStart=\"<span style=\\\"color:'+color+'\\\">'+task[0]+'.start</span>\";\
						startVal='+start+';\
						$("#tabAdvanced").find("td").css("background-color", "");\
						$("#id_'+streams[idStream][i][0]+'").children(".td1").css("background-color", colorAdvanced1["'+streams[idStream][i][0]+'"]);\
						$("#id_'+streams[idStream][i][0]+'").children(".td2").css("background-color", colorAdvanced2["'+streams[idStream][i][0]+'"]);\
						ocl_queuedSubmit = ['+ocl_queued+','+ocl_submit+'];\
						refreshCanvasBackground();\
						showAdvanced();\
					});\
					$("#taskInstance-'+idStream+'-'+i+'").mouseover(function(e) {\
						cursor[1]='+end+';\
						$("#taskInstanceName").html("'+str+'");\
						$("#taskInstanceName").css("display", "block");\
						$("#taskInstanceName").css("left", ""+(10+e.pageX)+"px");\
						$("#taskInstanceName").css("top", ""+(10+e.pageY)+"px");\
						colorize("taskInstance-'+idStream+'-'+i+'", 4, '+task[1]+', \''+task[0]+'\');\
						if(nameStart!="") {\
							$("#console").html("<strong><span style=\\\"color:'+color+'\\\">'+task[0]+'.end</span>&nbsp;-&nbsp;"+nameStart+"&nbsp;=&nbsp;"+parseFloat('+end+'-startVal).toFixed(2)+unit+"</strong>");\
						}\
						showAdvanced();\
						refreshCanvasBackground();\
					});\
					$("#taskInstance-'+idStream+'-'+i+'").mouseout(function(e) {\
						$("#taskInstanceName").css("display", "none");\
						colorize("taskInstance-'+idStream+'-'+i+'", 2, '+task[1]+', \''+task[0]+'\');\
					});\
					$("#taskInstance-'+idStream+'-'+i+'").dblclick(function(e) {\
						wLeft  = '+Math.max(min, parseFloat(start  )-2*(parseFloat(end)-parseFloat(start)))+';\
						wRight = '+Math.min(max, parseFloat(end    )+2*(parseFloat(end)-parseFloat(start)))+';\
						refreshScreen();\
					});\
				</script>';
			}
		}
	}
	$("#timelineStreams").append(timelineStreamsAppend+'<style>'+timelineStreamsAppendCSS+'</style>');
}

function refreshTaskInstances() {
	$('.taskInstance').css('display', 'none');
	$('.taskInstance').html();

	for(var idStream = 0; idStream < streams.length; idStream+=1) {
		if($("#titleStream-"+idStream).css("display")!='none') {
			var streamLength = streams[idStream].length;
			if(streams[idStream]!=undefined) for(var i = 0; i < streamLength; i++) {
				task = tasks[streams[idStream][i][0]];
		
				var begin  = streams[idStream][i][2];
				var end = streams[idStream][i][3];
				if((begin > wLeft && begin < wRight) || (end > wLeft && end < wRight) || (begin < wLeft && end > wRight)) {
					$('#taskInstance-'+idStream+'-'+i+'').css('display', 'block');

					var left = parseInt((begin-wLeft)/(wRight-wLeft)*timelineWidth);
				
					var cutL = 0;
					if(left < 0) {
						cutL = -left;
						left = 0;
						$('#taskInstance-'+idStream+'-'+i+'').css('border-left', '0px black solid');
					}
					else {
						cutL = 0;
						$('#taskInstance-'+idStream+'-'+i+'').css('border-left', '1px black solid');
					}
					var width = parseInt((end-begin)/(wRight-wLeft)*timelineWidth-cutL);
					$('#taskInstance-'+idStream+'-'+i+'').css('left', left+'px');
					var top = $("#titleStream-"+idStream).offset().top-$("#timelineStreams").offset().top;
					$('#taskInstance-'+idStream+'-'+i+'').css('top', top+'px');
					var right = left+width;
					var cutR;
					if(right > timelineWidth) {
						cutR = right-timelineWidth;
						width = timelineWidth-left;
						$('#taskInstance-'+idStream+'-'+i+'').css('border-right', '0px black solid');
					}
					else {
						cutR = 0;
						$('#taskInstance-'+idStream+'-'+i+'').css('border-right', '1px black solid');
					}
					$('#taskInstance-'+idStream+'-'+i+'').css('width', width+'px');
					$('#taskInstance-'+idStream+'-'+i+'').css('background-color', 'red');
					colorize('taskInstance-'+idStream+'-'+i+'', 2, task[1], task[0]);
				}
			}
		}
	}
}

function imgMinus(pos, title, sel) {
	str='<img src="inc/minus.png" class="plusminus" title="'+title+'" onclick=\'';
	if(pos==0) {
		str+='$("'+sel+'").toggle();';
	}
	else if(pos==1) {
		str+='$(this).parent().css("height", parseInt($(this).parent().css("height")) > 20 ? "20px" : "100px");'
	}
	str += 'refreshScreen();if($(this).attr("src")=="inc/minus.png") $(this).attr("src", "inc/plus.png"); else $(this).attr("src", "inc/minus.png");\'/>';
	return str;
}

function showInfos(name, obj) {
	var datas="";
	var X = unescape(obj).split(',');

	var text="<h2 style='text-align:center;'>"+name+"</h2>";
	if(X.length>2) {
		text+="<table>";
		for(var i = 0; i < X.length; i+=3) {
			help = String(X[i+2]).replace(/\&virgule/gi, ',')
			text+='<tr><td valign=top class="tdLeft">'+X[i+0]+'</td><td valign=top class="tdRight">'+X[i+1]+'<br /><span style="font-style:italic; color:blue;">'+nl2br(HTMLEncode(help))+'</span></td></tr>';
		}
		text+="</table>";
	}
	else {
		text+="<div style='text-align:left;'><strong style='color:orange'>No information to display for this device.</strong></div>";
	}
	$('#tempBlock').append('<div id="showInfos1"></div>');
	$('#tempBlock').append('<div id="showInfos2"><a onclick="hideInfos()" style="color:blue;cursor:pointer;">[close]</a><br />'+text+'</div>');
}

function hideInfos() {
	$('#tempBlock').html('');
}

var changeViewRequests = 0;
function changeView(choice) {
	changeViewRequests+=1;
	if(changeViewRequests==1) {
		changeView2(choice);
	}
}

function changeView2(choice) {
	if(changeViewRequests>0) {
		var tWidth = (wRight-wLeft)/2;
		var center = wLeft+tWidth;
		wLeftL = wLeft;
		wRightL = wRight;
		if(choice==1) { // zoom in
			wLeft  = Math.max(min, center-tWidth*1.5);
			wRight = Math.min(max, center+tWidth*1.5);
		}
		else if(choice==2) { // zoom out
			wLeft  = Math.max(min, center-tWidth*0.7);
			wRight = Math.min(max, center+tWidth*0.7);
		}
		else if(choice==3) { // to the left
			wLeft  = Math.max(min, wLeft-tWidth/4);
			wRight = wLeft+2*tWidth;
		}
		else if(choice==4) { // to the right
			wRight = Math.min(max, wRight+tWidth/4);
			wLeft  = wRight-2*tWidth;
		}
		else { // 0-> reinit
			wLeft  = min;
			wRight = max;
		}
		if(wLeftL != wLeft || wRightL != wRight) refreshScreen();
		window.setTimeout("changeViewRequests = 0; changeView2("+choice+");", 200);
		
	}
}

function createScreen() {
	$('#timelineStreams').css('margin', '0px');
	var p1 = parseInt($("#timelineTitles").css('padding-left'))+parseInt($("#timelineTitles").css('padding-right'));
	var p2 = parseInt($("#timelineTitles").css('padding-top'))+parseInt($("#timelineTitles").css('padding-bottom'));
	l = $("#timelineTitles").offset().left + $("#timelineTitles").width() + p1;
	
	$('#timelineStreams').css('left', l);
	$('#timelineStreams').css('top', $("#timelineTitles").offset().top);
	$('#timelineStreams').css('width', $("#timeline").width()-($("#timelineTitles").width()+p1));
	$('#timelineStreams').css('height', $("#timelineTitles").height()+p2);
	
	timelineWidth = parseInt($('#timelineStreams').css('width'));
	timelineHeight = parseInt($('#timelineStreams').css('height'));
	$("#timelineStreams").html('<canvas id="canvasBackground" style="width:'+(timelineWidth)+'px;height:'+timelineHeight+'px;" width="'+timelineWidth+'px" height="'+timelineHeight+'px" onContextMenu="changeView(1);return false;"></canvas>');// onClick="changeView(2);"
	createTaskInstances();
}

function refreshScreen() {
	// How much canvas? Each will measure at most 1000px
	refreshCanvasBackground();
	refreshTaskInstances();
}

function refreshCanvasBackground() {
	var ctx = document.getElementById('canvasBackground').getContext('2d');
	ctx.fillStyle = "rgb(0, 0, 0)";
	ctx.clearRect(0, 0, timelineWidth, timelineHeight);
	ctx.fillRect(0, 15, timelineWidth, 1);
	
	var pas = 0.001;
	var diff=wRight-wLeft;
	while(diff/pas > 50) {
		pas*=10;
	}
	
	scaleMin = pas*Math.ceil(wLeft/pas);
	scaleMax = pas*Math.ceil(wRight/pas);
	var N = (scaleMax-scaleMin)/pas;



	var i = 0;
	for(scale = scaleMin; scale<scaleMax; scale+=pas) {
		var off=(scale-wLeft)/(wRight-wLeft)*timelineWidth;
		if(scale%(5*pas)==0 || N<10) {
			ctx.fillStyle = "rgb(0, 0, 0)";
			ctx.fillRect(off, 15, 1, timelineHeight);
			ctx.font = '12px Arial';
			ctx.textAlign = 'center';
			ctx.fillText(''+scale+unit, off+2, 12);
		}
		else {
			ctx.fillStyle = "rgb(100, 100, 100)";
			ctx.fillRect(off, 15, 1, timelineHeight);
		}
	}
	
	// A and B
	if(cursor[0]!=NaN) {
		var h=15;
		var w=15;
		var offsetY=17;
		for(k=0; k <2; k+=1) {
			if(k==0) {
				Name='A';
				aLeft =0;
				aRight=1;
				posR=-w-w/2;
			}
			else {
				Name='B';
				aLeft =1;
				aRight=0;
				posR=w-w/2;
			}
			var Xv = cursor[k];
			if(Xv > wLeft && Xv < wRight) {
				Xv = Math.round((Xv-wLeft)/(wRight-wLeft)*timelineWidth);
				for(var i=0; i<w/2; i+=1) {
					if(i>0)
						ctx.fillStyle = "rgb("+(255-10*i)+", 0, 0)";
					else
						ctx.fillStyle = "rgb(0,0,0)";
					ctx.fillRect(Xv-aLeft*i+posR, offsetY+  i, w+aLeft*i+aRight*i, 1);
					ctx.fillRect(Xv-aLeft*i+posR, offsetY+w-i, w+aLeft*i+aRight*i, 1);
					ctx.fillStyle = "rgb(0,0,0)";
					ctx.fillRect(Xv-aLeft*i+posR                   , offsetY+  i, 1, 1);
					ctx.fillRect(Xv-aLeft*i+posR                   , offsetY+w-i, 1, 1);
					ctx.fillRect(Xv-aLeft*i+posR+w+aLeft*i+aRight*i, offsetY+  i, 1, 1);
					ctx.fillRect(Xv-aLeft*i+posR+w+aLeft*i+aRight*i, offsetY+w-i, 1, 1);
				}
				ctx.fillStyle = "rgb(255,255,255)";
				ctx.font = 'bold 14px Arial';
				ctx.textAlign = 'center';
				ctx.fillText(Name, Xv+w/2+posR, offsetY+h-2);
			
				ctx.fillStyle = "rgb(120,0,0)";
				ctx.fillRect(Xv-aLeft*w/2+posR+aRight*(w+aLeft*w/2+aRight*w/2), offsetY, 1, timelineHeight-offsetY);
			}
		}
	}
	
	// Q and S (ocl_queued and ocl_submit)
	if(ocl_queuedSubmit[0]!=undefined && advanced==1 && ocl_queuedSubmit[0]!="") {
		var h=15;
		var w=15;
		var offsetY=35;
		for(k=0; k <2; k+=1) {
			if(k==0) {
				Name='Q';
				aLeft =0;
				aRight=1;
				posR=-w-w/2;
			}
			else {
				Name='S';
				aLeft =1;
				aRight=0;
				posR=w-w/2;
			}
			var Xv = ocl_queuedSubmit[k];
			if(Xv > wLeft && Xv < wRight) {
				Xv = Math.round((Xv-wLeft)/(wRight-wLeft)*timelineWidth);
				for(var i=0; i<w/2; i+=1) {
					if(i>0)
						ctx.fillStyle = "rgb(0,0,"+(255-10*i)+")";
					else
						ctx.fillStyle = "rgb(0,0,0)";
					ctx.fillRect(Xv-aLeft*i+posR, offsetY+  i, w+aLeft*i+aRight*i, 1);
					ctx.fillRect(Xv-aLeft*i+posR, offsetY+w-i, w+aLeft*i+aRight*i, 1);
					ctx.fillStyle = "rgb(0,0,0)";
					ctx.fillRect(Xv-aLeft*i+posR                   , offsetY+  i, 1, 1);
					ctx.fillRect(Xv-aLeft*i+posR                   , offsetY+w-i, 1, 1);
					ctx.fillRect(Xv-aLeft*i+posR+w+aLeft*i+aRight*i, offsetY+  i, 1, 1);
					ctx.fillRect(Xv-aLeft*i+posR+w+aLeft*i+aRight*i, offsetY+w-i, 1, 1);
				}
				ctx.fillStyle = "rgb(255,255,255)";
				ctx.font = 'bold 14px Arial';
				ctx.textAlign = 'center';
				ctx.fillText(Name, Xv+w/2+posR, offsetY+h-2);
			
				ctx.fillStyle = "rgb(0,0,120)";
				ctx.fillRect(Xv-aLeft*w/2+posR+aRight*(w+aLeft*w/2+aRight*w/2), offsetY, 1, timelineHeight-offsetY);
			}
		}
	}
	
	// Scroll bar
	var w = parseInt((wRight-wLeft)/(max-min)*timelineWidth);
	var l = parseInt((wLeft-min)/(max-min)*timelineWidth);
	if(w!=timelineWidth) {
		ctx.fillStyle = "rgb(40,40,40)";
		ctx.fillRect(l, timelineHeight-10, 1, 10);
		ctx.fillRect(l+w-1, timelineHeight-10, 1, 10);
		ctx.fillRect(l+1, timelineHeight-10, w-1, 1);
		ctx.fillRect(l+1, timelineHeight-1, w-1, 1);

		ctx.fillStyle = "rgb(150,150,150)";
		ctx.fillRect(l+1, timelineHeight-9, w-2, 8);
	}

}

$(window).resize(function() {
	createScreen(); refreshScreen();
});

function showAdvanced() {
	if($('#isAdvanced').is(':checked')) {
		advanced=1;
		$('.advanced').show();
	}
	else {
		advanced=0;
		$('.advanced').hide();
	}
}

function createRAW() { // Fill the raw div with profiling informations
	var output='';
	// Unit?
	unit=$xml.children('profiling').children('head').children('unit').text();
	if(unit=='') unit='undefined';
	output+='time unit: <strong>'+unit+'</strong><br />';
	
	// Instanciated tasks
	output+='<table class="sortable" id="tabAdvanced"><thead><tr><th>task name</th><th>Instanciation name</th><th>stream</th><th class="sorttable_numeric" id="queuedAdvanced">(OpenCL)queued</th><th class="sorttable_numeric">(OpenCL)submit</th><th class="sorttable_numeric">start</th><th class="sorttable_numeric">end</th><th>(OpenCL)global work size</th><th>(OpenCL)local work size</th><th class="sorttable_numeric">bandwidth (MiB/s)</th><th class="sorttable_numeric">size (bytes)</th><th>size</th></tr></thead><tbody>';
	var tabTaskName = [];
	$xml.find('task').each(function() {
		var id = $(this).find('id').text();
		var name = $(this).find('name').text();
		if(name=='') name='<span style="color:#303030">N/A</span>';
		tabTaskName[id] = name;
	});
	
	// Devices
	$xml.find('device').each(function() {
		var device=$(this).children('name').text();
		$(this).find('stream').each(function() {
			var stream=$(this).children('name').text();
			$(this).find('taskInstance').each(function() {
				var Id=$(this).children('idTask').text();
				color=colors[tasks[Id][1]][1];
				for(i=0; i<3; i+=1)
					color[i]=Math.round((color[i]+255)/2);
				color1='rgb('+(color[0]-15)+', '+(color[1]-15)+', '+(color[2]-15)+')';
				color2='rgb('+color[0]+', '+color[1]+', '+color[2]+')';
				style=[];
				colorAdvanced1[Id]=color1;
				colorAdvanced2[Id]=color2;
				
				output+='<tr id="id_'+Id+'">';
				
				
				var taskInstanceName = $(this).children('name').text();
				
				var start=$(this).children('start').text();
				var end  =$(this).children(  'end').text();
				
				var bandwidth=$(this).children('bandwidth').text();
				if(bandwidth=='') bandwidth='<span style="color:#303030">N/A</span>';
				//else bandwidth += 'MiB/s';
				
				var size1=$(this).children('size').text();
				if(size1=='') {
					size1='<span style="color:#303030">N/A</span>';
					size2='<span style="color:#303030">N/A</span>';
				}
				else {
					if(parseInt(size1)>1024) {
						size2I = parseInt(parseFloat(size1)/1024);
						size2 = size2I+'KiB';
						if(size2I>1024) {
							size2 = parseInt(parseFloat(size2)/1024)+'MiB ('+size2+')';
						}
					}
					else
						size2 = size1+'B';
				}
				
				
				
				// Optionnal informations
				var ocl_queued=$(this).children('ocl_queued').text();
				if(ocl_queued=='') ocl_queued='<span style="color:#303030">N/A</span>';
				var ocl_submit=$(this).children('ocl_submit').text();
				if(ocl_queued=='') ocl_queued='<span style="color:#303030">N/A</span>';
				
				
				var gws=$(this).children('ocl_global_work_size').text();
				if(gws=='') gws='<span style="color:#303030">N/A</span>';
				else gws='['+gws+']';
				
				var lws=$(this).children('ocl_local_work_size').text();
				if(lws=='') lws='<span style="color:#303030">N/A</span>';
				else lws='['+lws+']';
				
				//var i=0;
				output+='<td class="td1">'+tabTaskName[Id]+'</td>';
				output+='<td class="td2">'+taskInstanceName+'</td>';
				output+='<td class="td1">'+device+'.'+stream+'</td>';
				output+='<td class="td2">'+ocl_queued+'</td>';
				output+='<td class="td2">'+ocl_submit+'</td>';
				output+='<td class="td1">'+start+'</td>';
				output+='<td class="td2">'+end+'</td>';
				output+='<td class="td1">'+gws+'</td>';
				output+='<td class="td2">'+lws+'</td>';
				output+='<td class="td1">'+bandwidth+'</td>';
				output+='<td class="td2">'+size1+'</td>';
				output+='<td class="td1">'+size2+'</td>';
				//output+='<td>'+$(this).children('ocl_queued').text()+'</td>';
				//][ocl_submit='+$(this).children('ocl_submit').text()+'][start='+$(this).children('start').text()+'][end='+$(this).children('end').text()+'][global_work_size='+$(this).children('global_work_size').text()+'][ocl_local_work_size='+$(this).children('ocl_local_work_size').text()+']';
				output+='</tr>';
			});
		});
	});
	output+='</tbody><tfoot></tfoot></table>';
	$('#raw').append(output);

	var a= document.getElementById("tabAdvanced");
	sorttable.makeSortable(a);
	//var a= document.getElementById("ocl_queuedAdvanced"); sorttable.innerSortFunction.apply(a, []);

	
	//var myTH = document.getElementsByTagName("th")[0];
	//sorttable.innerSortFunction.apply(myTH, []);
	
	showAdvanced();
}

function getTime(){ 
	var myDate = new Date();
	var time = myDate.getTime();
	var hour = myDate.getHours();
	var minute = myDate.getMinutes();
	var second = myDate.getSeconds();
	var date = myDate.getDate();
	var dayT = ['Sunday', 'Monday', 'Thursday', 'Wednesday', 'Tuesday', 'Friday', 'Saturday'];
	var monthT = ['January', 'February', 'March', 'April', 'May', 'June', 'July', 'August', 'September', 'October', 'November', 'December'];
	
	
	if (hour < 10) { hour = "0" + hour; } 
	if (minute < 10) { minute = "0" + minute; } 
	if (second < 10) { second = "0" + second; } 
	if (date < 10) { date = "0" + date; } 
	var theDate = dayT[myDate.getDay()]+", "+monthT[myDate.getMonth()]+" "+date+"<sup>th</sup> "+myDate.getFullYear()+" at <b>"+ hour + ":" + minute + ":" + second+"</b>"; 
	return theDate;
}
