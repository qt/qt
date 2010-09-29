// Removing search results
function hideSearchResults() {
  $('#resultdialog').removeClass('active');
	$("#resultlist").removeClass().addClass('all');
	$("#resultlinks").removeClass().addClass('all');
	$("#searchcount").removeClass().addClass('all');
}
// Closing search result box
$('#resultclose').click(function(e) {
  e.preventDefault();
  hideSearchResults();
});

$(document.body).click(function() {
});

/* START non link areas where cursor should change to pointing hand */
$('.t_button').mouseover(function() {
    $('.t_button').css('cursor','pointer');
});
/* END non link areas  */
// Font size small
$('#smallA').click(function() {
		$('.mainContent .heading,.mainContent h1, .mainContent h2, .mainContent h3, .mainContent p, .mainContent li, .mainContent table').css('font-size','smaller');
		$('.t_button').removeClass('active')
		$(this).addClass('active')
});

// Font size reset
$('#medA').click(function() {
		$('.mainContent .heading').css('font','600 16px/1 Arial');
		$('.mainContent h1').css('font','600 18px/1.2 Arial');
		$('.mainContent h2').css('font','600 16px/1.2 Arial');
		$('.mainContent h3').css('font','600 14px/1.2 Arial');
		$('.mainContent p').css('font','13px/20px Verdana');
		$('.mainContent li').css('font','400 13px/1 Verdana');
		$('.mainContent li').css('line-height','14px');
		$('.mainContent .toc li').css('font', 'normal 10px/1.2 Verdana');
		$('.mainContent table').css('font','13px/1.2 Verdana');
		$('.mainContent .heading').css('font','600 16px/1 Arial');
		$('.mainContent .indexboxcont li').css('font','600 13px/1 Verdana');
		$('.t_button').removeClass('active')
		$(this).addClass('active')
});

// Font size large
$('#bigA').click(function() {
		$('.mainContent .heading,.mainContent h1, .mainContent h2, .mainContent h3, .mainContent p, .mainContent li, .mainContent table').css('font-size','large');
		$('.mainContent .heading,.mainContent h1, .mainContent h2, .mainContent h3, .mainContent p, .mainContent li, .mainContent table').css('line-height','25px');
		$('.t_button').removeClass('active')
		$(this).addClass('active')
});

// Show page content after closing feedback box
$('.feedclose').click(function() {
	$('.bd').show();
	$('.hd').show();
	$('.footer').show();
	$('#feedbackBox').hide();
	$('#blurpage').hide();
});

// Hide page content and show feedback box
$('.feedback').click(function() {
	$('.bd').hide();
	$('.hd').hide();
	$('.footer').hide();
	$('#feedbackBox').show();
	$('#blurpage').show();
});
// Setting URL - in this case relative to root
var qturl = "";

// Process data sent back from the server. The data is structured as a XML.
/*
XML structure handled by function processNokiaData()
<page> - container for each page returned
<pageWords/> - contains keywords
<pageTitle/> - contains page title/header content 
<pageUrl/> - contains page URL - URL relative to root
<pageType> - contains page type - APIPage/Article/Example
</page>
*/


function processNokiaData(response){
	var propertyTags = response.getElementsByTagName('page');
	
	var apiCount = 0;
	var articleCount = 0;
	var exampleCount = 0;
	
	var full_li_element;

	$('#resultlist li').remove();


 	for (var i=0; i<propertyTags.length; i++) {
		for (var j=0; j< propertyTags[i].getElementsByTagName('pageWords').length; j++) {
			full_li_element = '<li';
      if (propertyTags[j].getElementsByTagName('pageType')[0].firstChild.nodeValue == 'APIPage') {
      	full_li_element += ' class="api"';
      	apiCount++;
      }
      else if (propertyTags[j].getElementsByTagName('pageType')[0].firstChild.nodeValue == 'Article') {
      	full_li_element += ' class="article"';
      	articleCount++;
      }
      else if (propertyTags[j].getElementsByTagName('pageType')[0].firstChild.nodeValue == 'Example') {
      	full_li_element += ' class="example"';
      	exampleCount++;
      }
			full_li_element += '><a href="'+qturl;
      full_li_element += propertyTags[i].getElementsByTagName('pageUrl')[j].firstChild.nodeValue;
      full_li_element += '">' + propertyTags[i].getElementsByTagName('pageTitle')[0].firstChild.nodeValue + '</a></li>';
      $('#resultlist').append(full_li_element);
    }
	}

	if (propertyTags.length > 0) {
	  $('#resultdialog').addClass('active');
	  $('#resultcount').html(propertyTags.length);
	  $('#apicount').html(apiCount);
	  $('#articlecount').html(articleCount);
	  $('#examplecount').html(exampleCount);
  }

  // Filtering results in display
	$('p#resultlinks a').click(function(e) {
  	e.preventDefault();
	// Displays API ref pages
		if (this.id == "showapiresults") {
			$("#resultlist").removeClass().addClass('api');
			$("#resultlinks").removeClass().addClass('api');
			$("#searchcount").removeClass().addClass('api');
		}
	// Displays Articles
		else if (this.id == "showarticleresults") {
			$("#resultlist").removeClass().addClass('article');
			$("#resultlinks").removeClass().addClass('article');
			$("#searchcount").removeClass().addClass('article');
		}
	// Displays Examples
		if (this.id == "showexampleresults") {
			$("#resultlist").removeClass().addClass('example');
			$("#resultlinks").removeClass().addClass('example');
			$("#searchcount").removeClass().addClass('example');
		}
	// Displays All
		if (this.id == "showallresults") {
			$("#resultlist").removeClass().addClass('all');
			$("#resultlinks").removeClass().addClass('all');
			$("#searchcount").removeClass().addClass('all');
		}
	});
}

//build regular expression object to find empty string or any number of blank
var blankRE=/^\s*$/;
function CheckEmptyAndLoadList()
{
	var pageUrl = window.location.href;
	var pageVal = $('title').html();
	$('#feedUrl').remove();
	$('#pageVal').remove();
	$('.menuAlert').remove();
	$('#feedform').append('<input id="feedUrl" name="feedUrl" value="'+pageUrl+'" style="display:none;">');
	$('#feedform').append('<input id="pageVal" name="pageVal" value="'+pageVal+'" style="display:none;">');
	$('.liveResult').remove();
    $('.defaultLink').css('display','block');
	var value = document.getElementById('pageType').value; 
	if((blankRE.test(value)) || (value.length < 3))
	{



	 $('.defaultLink').css('display','block');

	$('#resultdialog').removeClass('active');
	$('#resultlist li').remove();
	}else{

	}
}

// Loads on doc ready - prepares search 
	$(document).ready(function () {


	var pageTitle = $('title').html();
          var currentString = $('#pageType').val() ;
		  if(currentString.length < 1){
			$('.defaultLink').css('display','block');
      	   		CheckEmptyAndLoadList();			
		  }

        $('#pageType').keyup(function () {
          var searchString = $('#pageType').val() ;
          if ((searchString == null) || (searchString.length < 3)) {
				$('#pageType').removeClass('loading');
				 $('.liveResult').remove(); 
				 $('.searching').remove(); 
      	   		CheckEmptyAndLoadList();
				$('.report').remove();

				return;
		   }
            if (this.timer) clearTimeout(this.timer);
            this.timer = setTimeout(function () {
				$('#pageType').addClass('loading');
				$('.searching').remove(); 

               $.ajax({
                contentType: "application/x-www-form-urlencoded",
                url: 'http://' + location.host + '/nokiasearch/GetDataServlet',
                data: 'searchString='+searchString,
                dataType:'xml',
				type: 'post',	 
                success: function (response, textStatus) {

				$('.liveResult').remove(); 
				$('.searching').remove(); 
				$('#pageType').removeClass('loading');

                processNokiaData(response);

 }     
              });
            }, 500);
        });
      }); 
