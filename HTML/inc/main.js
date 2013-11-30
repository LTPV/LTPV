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
var colors=new Array();
var streams=new Array();
var min, max;
var tasks = new Array();
var wLeft;
var wRight;
var unit='';
var nameStart="";
var startedVal=0;
var cursor=[NaN, NaN];
var colorAdvanced1 = {};
var colorAdvanced2 = {};
var advanced = 0;
var timelineWidth;
var timelineHeight;
//var ocl_queuedSubmit=[undefined, undefined];
var ocl_queuedSubmit=[undefined, undefined];
var $xml;

function init() {
	var loc = window.location.pathname;
	var dir = loc.substring(0, loc.lastIndexOf('/'));
	
	var filename = getQuerystring('file');
	if(filename=='') {
		window.location = loc+'?file=example.xml';
		//alert('No XML file given. You must provide a file to parse in the address: index.html?file=YourFile\nTrying with "profiling.xml"...');
		throw new Error();
	}
	$xml=$($.parseXML($.ajax({ type: "GET", url: filename, async: false, error:function() { die('Impossible to read '+filename+'.\n* Does this file really exist?\n* With Firefox, you may have to do about:config and switch security.fileuri.strict_origin_policy to false and restart it.\n* With Chrome/Chromium, please close it and launch it with the --allow-file-access-from-files option.'); return ''; }}).responseText));
	
	// get unit
	unit=$xml.children('profiling').children('head').children('unit').text();
	if(unit=='') unit='ns';

	
	// Min and max for tasks
	$xml.find('taskInstance').each(function() {
		x = $(this).find('ocl_queued').text();
		if(x!='') {
			x = parseFloat(x);
			if(x > max || max==undefined) max=x;
			if(x < min || min==undefined) min=x;
		}
		//console.log([min, max]);
		x = $(this).find('ocl_submit').text();
		if(x!='') {
			x = parseFloat(x);
			if(x > max || max==undefined) max=x;
			if(x < min || min==undefined) min=x;
		}
		//console.log([min, max]);
		x = $(this).find('start').text();
		if(x!='') {
			x = parseFloat(x);
			if(x > max || max==undefined) max=x;
			if(x < min || min==undefined) min=x;
		}
		//console.log([min, max]);
		x = $(this).find('end').text();
		if(x!='') {
			x = parseFloat(x);
			if(x > max || max==undefined) max=x;
			if(x < min || min==undefined) min=x;
		}
		//console.log([min, max]);
	});
	
	min = min-0.02*(max-min);
	max = max+0.02*(max-min);
	
	wLeft=min;
	wRight=max;
	
	createColors();
	
	// Creating the tasks tab
	var i = 1;
	$xml.find('task').each(function() {
		var id = $(this).find('id').text();
		var name = $(this).find('name').text();
		if(tasks[id]!=undefined) die('There is more than one task with the id '+id+'');
		tasks[id]=[name, i];
		i +=1;
	});
	
	if(i>colors.length) die('More tasks than colors!');
	
	// Creating the device tab with streams
	var devices = new Array();
	var idDevice = 0;
	var idStreamG = 0;

	streams=new Array(); 
	
	$xml.find('device').each(function() {
		var datas = new Array();
		var i = 0;
		$(this).children('details').children('detail').each(function() {
			datas[i]=new Array();
			datas[i++] = $(this).children('name').text();
			datas[i++] = $(this).children('value').text();
			datas[i++] = String($(this).children('help').text()).replace(/,/gi, '&virgule');
		});
	
		var nameDevice = $(this).children('name').text();
		devices[idDevice] = new Array();
		devices[idDevice]['name']=nameDevice;
		
		
		$("#timelineTitles").append(
			imgMinus(0, 'Hide streams for device ['+idDevice+']'+nameDevice, '.titleStream-'+idDevice) +
			'<span class="nameDevice" title=\'Click to see the '+nameDevice+' device properties\' onclick=\'showInfos("'+nameDevice+'", "'+
			escape(datas) +
			'")\'>['+idDevice+'] ' + nameDevice +
			'</span><br /><div id="device-'+idDevice+'" class="device"></div>');
		
		devices[idDevice]['stream'] = new Array();
		var idStream = 0;
		
		// Entering in streams
		$(this).find('stream').each(function() {
			var nameStream=$(this).children('name').text();
			devices[idDevice]['stream']['name'] = nameStream;

			// Creating tasks
			streams[idStreamG]=new Array();
			var idTaskInstance = 0;
			$(this).find('taskInstance').each(function() {
				streams[idStreamG][idTaskInstance]=new Array();
				streams[idStreamG][idTaskInstance]=[
					$(this).find('idTask').text(),
					$(this).find('name').text(),
					$(this).find('start').text(),
					$(this).find('end').text(),
					$(this).find('ocl_global_work_size').text(),
					$(this).find('ocl_local_work_size').text(),
					$(this).find('ocl_bandwidth').text(),
					$(this).find('ocl_queued').text(),
					$(this).find('ocl_submit').text(),
					$(this).find('ocl_size').text(),
					];
				idTaskInstance += 1;
			}); 

			$('#device-'+idDevice).append('<div class="titleStream titleStream-'+idDevice+'" id="titleStream-'+idStreamG+'" style="display:block;height:30px;">&nbsp;&nbsp;&nbsp;&nbsp;'+nameStream+'</div>');
			idStream  += 1;
			idStreamG += 1;
		});
		idDevice += 1;
		
	});
	createScreen();
	createRAW();
	refreshScreen();
	shortcut.add("F1",function() {
		changeView(1);
	});
	shortcut.add("F2",function() {
		changeView(2);
	});
	shortcut.add("F3",function() {
		$("#details").html("Click on a task to display details"); $("#console").html(""); cursor[0]=NaN; changeView(0);
	});
	shortcut.add("F4",function() {
		refreshScreen();
	});
	shortcut.add("Left",function() {
		changeView(3);
	});
	shortcut.add("Right",function() {
		changeView(4);
	});
	
	
	
	var zoomDone = 1;
	$("#canvasBackground").mousedown(function(a){
		a.preventDefault();
		if (a.which == 1) {
			zoomDone = 0;
			var offsets = document.getElementById('canvasBackground').getBoundingClientRect();
			var top = offsets.top + window.pageYOffset;
			var left = a.pageX;
			$('#divZooming').css('left', ''+(left)+'px');
			$('#divZooming').css('top', ''+(top+15)+'px');
			$('#divZooming').css('height', ''+(timelineHeight-16)+'px');
			$('#divZooming').css('border', '1px black solid');
			$('#divZooming').css('display', 'block');
			$('#divZooming').css('width', '0px');
			var minX = $('#timelineStreams').offset().left;
			var maxX = minX+$('#timelineStreams').outerWidth()-1;
			var width;
			$(document).mousemove(function(e) {
				width = Math.min(Math.max(e.pageX, minX),maxX)-left;
				if(width>0) {
					$('#divZooming').css('left', ''+(left)+'px');
					$('#divZooming').css('width', ''+width+'px');
				}
				else {
					$('#divZooming').css('left', ''+(Math.max(left+width, minX))+'px');
					$('#divZooming').css('width', ''+(-width)+'px');
				}
				if(Math.abs(width) > 20) {
					$('#divZooming').css('background-color', 'rgba(76, 64, 107, 0.7)');
					$('#divZooming').css('background-image', 'linear-gradient(to right, rgba(08, 0, 57, 0.4), rgba(76, 64, 107, 0.3))');
				}
				else {
					$('#divZooming').css('background-color', 'rgba(76, 64, 107, 0.2)');
					$('#divZooming').css('background-image', 'linear-gradient(to right, rgba(18, 0, 67, 0.2), rgba(76, 64, 107, 0.2))');
				}
			});
	
			$(document).mouseup(function(e) {
				if(zoomDone == 0) {
					if(Math.abs(width) > 20) {
						zoomDone = 1;
						var X1 = document.getElementById('canvasBackground').getBoundingClientRect();
						var X2 = document.getElementById('divZooming').getBoundingClientRect();
						var a = (left-X1.left)/X1.width;
						var b = a+width/X1.width;
						if(a > b) {
							var c = a;
							a = b;
							b = c;
						}
						var wLeftN = wLeft+a*(wRight-wLeft);
						var wRightN = wLeft+b*(wRight-wLeft);

						wLeft  = wLeftN;
						wRight = wRightN;
						refreshScreen();
					}
					$('#divZooming').css('display', 'none');
					$(document).unbind("mousemove");
					$(document).unbind("mouseup");
				}
			});
		}
	});
	var date = '';
	var dateGenerated = $xml.children('profiling').children('head').children('date').text();
	if(dateGenerated!='') {
		date +='XML generated: '+dateGenerated+'<br />';
	}
	date += 'Last display refresh: '+getTime();
	$('#date').html(date);
	$('#tempBlock').html('');
}

$(function() {
	$('#tempBlock').append('<div id="showInfos1" style="color:white; font-size:x-large; text-align:center;"><div style="position:relative;top:50%;">Loading...</div></div>');
	document.body.scrollTop = document.documentElement.scrollTop = 0;
setTimeout("init()", 50);
	//$('#tempBlock').html('');
});
