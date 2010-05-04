/* START non link areas where cursor should change to pointing hand */
$('.t_button').mouseover(function() {
    $('.t_button').css('cursor','pointer');
		/*document.getElementById(this.id).style.cursor='pointer';*/
});

/* END non link areas  */
$('#smallA').click(function() {
		$('.content .heading,.content h1, .content h2, .content h3, .content p, .content li, .content table').css('font-size','smaller');
		$('.t_button').removeClass('active')
		$(this).addClass('active')
});

$('#medA').click(function() {
		$('.content .heading').css('font','600 16px/1 Arial');
		$('.content h1').css('font','600 18px/1.2 Arial');
		$('.content h2').css('font','600 16px/1.2 Arial');
		$('.content h3').css('font','600 14px/1.2 Arial');
		$('.content p').css('font','13px/20px Verdana');
		$('.content li').css('font','400 13px/1 Verdana');
		$('.content li').css('line-height','14px');
		$('.content .toc li').css('font', 'normal 10px/1.2 Verdana');
		$('.content table').css('font','13px/1.2 Verdana');
		$('.content .heading').css('font','600 16px/1 Arial');
		$('.content .indexboxcont li').css('font','600 13px/1 Verdana');
		$('.t_button').removeClass('active')
		$(this).addClass('active')
});

$('#bigA').click(function() {
		$('.content .heading,.content h1, .content h2, .content h3, .content p, .content li, .content table').css('font-size','large');
		$('.content .heading,.content h1, .content h2, .content h3, .content p, .content li, .content table').css('line-height','25px');
		$('.t_button').removeClass('active')
		$(this).addClass('active')
});


var lookupCount = 0;
var articleCount = 0;
var exampleCount = 0;
var qturl = ""; // change to  0
function processNokiaData(response){

	var propertyTags = response.getElementsByTagName('page');
	var ulStartElement = "<ul>";
	var ulEndElement = "</ul>";
	 
	for (var i=0; i< propertyTags.length; i++) {
		
		  var full_address_lookup   = "<ul><li><a href='"+qturl+"";
		  var full_address_topic    = "<ul><li><a href='"+qturl+"";
		  var full_address_examples = "<ul><li><a href='"+qturl+"";
		 
		 if(propertyTags[i].getElementsByTagName('pageType')[0].firstChild.nodeValue == 'APIPage'){
			 lookupCount=0;
			 document.getElementById('live001').style.display = "block";
			 // document.getElementById('live001').style.height = "150px";
			 
			 for (var j=0; j< propertyTags[i].getElementsByTagName('pageWords').length; j++){
				    full_address_lookup = full_address_lookup + propertyTags[i].getElementsByTagName('pageUrl')[j].firstChild.nodeValue;
					full_address_lookup =  full_address_lookup + "'>" + propertyTags[i].getElementsByTagName('pageTitle')[0].firstChild.nodeValue
					+ '</a></li></ul>' ;
					
					// var rowlookup = document.getElementById('div001').insertRow(-1);
					// var celllookup = rowlookup.insertCell(-1);
					// celllookup.style.padding="0 0 0 0";
					// lookupCount++;
					// celllookup.innerHTML = full_address_lookup ;
					// document.getElementById('list001').style.display = "none";
					
					// <li>
					//var listElement = "<li><a href='"+ full_address_lookup +"" + '</a></li>';
					
		   		}
			}
		 
			if(propertyTags[i].getElementsByTagName('pageType')[0].firstChild.nodeValue == 'Article'){
				articleCount = 0;
				 document.getElementById('live002').style.display = "block";
				 // document.getElementById('live002').style.height = "150px";
				for (var j=0; j< propertyTags[i].getElementsByTagName('pageWords').length; j++){
					full_address_topic = full_address_topic + propertyTags[i].getElementsByTagName('pageUrl')[j].firstChild.nodeValue;
					full_address_topic =  full_address_topic + "'>" + propertyTags[i].getElementsByTagName('pageTitle')[0].firstChild.nodeValue
					+ '</a></li></ul>';
						// var rowlookup = document.getElementById('div002').insertRow(-1);
						// var celllookup = rowlookup.insertCell(-1);
						// celllookup.style.padding="0 0 0 0";
						// articleCount++;
						// celllookup.innerHTML = full_address_topic;
						// document.getElementById('list002').style.display = "none";
			   		}
			}
			if(propertyTags[i].getElementsByTagName('pageType')[0].firstChild.nodeValue == 'Example'){
				exampleCount = 0;
				 document.getElementById('live003').style.display = "block";
				 // document.getElementById('live003').style.height = "150px";
				for (var j=0; j< propertyTags[i].getElementsByTagName('pageWords').length; j++){
					full_address_examples = full_address_examples + propertyTags[i].getElementsByTagName('pageUrl')[j].firstChild.nodeValue;
					full_address_examples =  full_address_examples + "'>" + propertyTags[i].getElementsByTagName('pageTitle')[0].firstChild.nodeValue
					+ '</a></li></ul>';
						// var rowlookup = document.getElementById('div003').insertRow(-1);
						// var celllookup = rowlookup.insertCell(-1);
						// celllookup.style.padding="0 0 0 0";
						// exampleCount++;
						// celllookup.innerHTML = full_address_examples;
						// document.getElementById('list003').style.display = "none";
			   		}
				}
		
	}	


	 
	if(lookupCount == 0){loadLookupList();}
    if(articleCount == 0){loadArticleList();}
	if(exampleCount == 0){loadExampleList();}
	// reset count variables;
	 lookupCount=0;
	 articleCount = 0;
     exampleCount = 0;
	
}
function removeResults() {
	/*  var resultsTableLookup = document.getElementById('div001');
	var recordslookup = resultsTableLookup.rows.length;
	
	for (var i=(recordslookup-1); i> 0; i--){
		// resultsTableLookup.deleteRow(i); 
					    
	}

	var resultsTableTopic = document.getElementById('div002');
	var recordstopic = resultsTableTopic.rows.length;
	for (var i=(recordstopic-1); i> 0; i--){
		// alert("delete: " + i);
		// resultsTableTopic.deleteRow(i);
	}

	
	var resultsTableexample = document.getElementById('div003');
	var recordsexample = resultsTableexample.rows.length;
	for (var i=(recordsexample-1); i> 0; i--)
		// resultsTableexample.deleteRow(i);
 */
	removeList();	
}
       
function removeList(){
 	// var resultsTableLookuplist = document.getElementById('tbl001');
	// var recordlookuplist = resultsTableLookuplist.rows.length;
	// for (var i=(recordlookuplist-1); i> 0; i--)
		// resultsTableLookuplist.deleteRow(i);

	// var resultsTableArticlelist = document.getElementById('tbl002');
	// var recordArticlelist = resultsTableArticlelist.rows.length;
	// for (var i=(recordArticlelist-1); i> 0; i--)
		// resultsTableArticlelist.deleteRow(i);

	// var resultsTableExamplelist = document.getElementById('tbl003');
	// var recordExamplelist = resultsTableExamplelist.rows.length;
	// for (var i=(recordExamplelist-1); i> 0; i--)
		// resultsTableExamplelist.deleteRow(i);
 }
 
//build regular expression object to find empty string or any number of blank
var blankRE=/^\s*$/;
function CheckEmptyAndLoadList()
{
	var value = document.getElementById('pageType').value; 
	if((blankRE.test(value)) || (value.length < 3))
	{
	//empty inputbox
		loadAllList(); 
		//alert("loadAllList");
        document.getElementById('live001').style.display = "none";
        document.getElementById('live002').style.display = "none";
        document.getElementById('live003').style.display = "none";	
        document.getElementById('list001').style.display = "block";
        document.getElementById('list002').style.display = "block";
        document.getElementById('list003').style.display = "block";
	}else{
		removeList();
		//alert("removeList");
		document.getElementById('live001').style.display = "block";
		document.getElementById('live002').style.display = "block";
		document.getElementById('live003').style.display = "block";
		 
	}
}
function loadAllList(){

	var fullAddressListLookup   = "<ul><li><a href='"+qturl+"modules.html'>All modules</a></li>" 
		+ "<li><a href='"+qturl+"classes.html'>All classes</a></li>"  
		+ "<li><a href='"+qturl+"functions.html'>All functions</a></li>"  
		+ "<li><a href='"+qturl+"platform-specific.html'>Platform specifics</a></li>"  
		+ "</ul>";
	// var rowlistlookup = document.getElementById('tbl001').insertRow(-1);
	// var celllistlookup = rowlistlookup.insertCell(-1);
	// celllistlookup.style.padding="0 0 0 0";
	//celllistlookup.style.width="10px";
	//celllistlookup.style.background = "yellow";
	//celllistlookup.innerHTML = fullAddressListLookup ;


 
  
	var fullAddressListArticle  = "<ul><li><a href='"+qturl+"object.html'>QObject model</a></li>" 
		+ "<li><a href='"+qturl+"eventsandfilters.html'>Events, signals &amp; slots</a></li>"  
		+ "<li><a href='"+qturl+"paintsystem.html'>Graphics &amp; Paint system</a></li>"  
		+ "<li><a href='"+qturl+"declarativeui.html'>Qt Quick</a></li>"  
		+ "<li><a href='"+qturl+"widgets-and-layouts.html'>Widget style &amp; layout</a></li>" 
		+ "</ul>";
	// var rowlistarticle = document.getElementById('tbl002').insertRow(-1);
	// var celllistarticle = rowlistarticle.insertCell(-1);
	// celllistarticle.style.padding="0 0 0 0";
	//celllistarticle.innerHTML = fullAddressListArticle ;

 
	var fullAddressListExample   = "<ul><li><a href='"+qturl+"examples.html'>All examples</a></li>" 
		+ "<li><a href='"+qturl+"tutorials.html'>All tutorials</a></li>"  
		+ "<li><a href='"+qturl+"#'>Qt Quick examples</a></li>"  
		+ "<li><a href='"+qturl+"#'>Desktop examples</a></li>"  
		+ "<li><a href='"+qturl+"#'>Device examples</a></li>"  
		+ "</ul>";
	// var rowlistexample = document.getElementById('tbl003').insertRow(-1);
	// var celllistexample = rowlistexample.insertCell(-1);
	// celllistexample.style.padding="0 0 0 0";
	//celllistexample.innerHTML = fullAddressListExample ;
	
}

function loadLookupList(){
	
	var fullAddressListLookup   = "<ul><li><a href='"+qturl+"modules.html'>All modules</a></li>" 
		+ "<li><a href='"+qturl+"classes.html'>All classes</a></li>"  
		+ "<li><a href='"+qturl+"functions.html'>All functions</a></li>"  
		+ "<li><a href='"+qturl+"platform-specific.html'>Platform specifics</a></li>"  
		+ "</ul>";
	// var rowlistlookup = document.getElementById('tbl001').insertRow(-1);
	// var celllistlookup = rowlistlookup.insertCell(-1);
	// celllistlookup.style.padding="0 0 0 0";
	//celllistlookup.style.width="10px";
	//celllistlookup.style.background = "yellow";
	celllistlookup.innerHTML = fullAddressListLookup ;
	document.getElementById('live001').style.display = "none";
	document.getElementById('list001').style.display = "block";
	//alert("loadLookupList")

}
function loadArticleList(){ 
  
	var fullAddressListArticle  = "<ul><li><a href='"+qturl+"object.html'>QObject model</a></li>" 
		+ "<li><a href='"+qturl+"eventsandfilters.html'>Events, signals &amp; slots</a></li>"  
		+ "<li><a href='"+qturl+"paintsystem.html'>Graphics &amp; Paint system</a></li>"  
		+ "<li><a href='"+qturl+"declarativeui.html'>Qt Quick</a></li>"  
		+ "<li><a href='"+qturl+"widgets-and-layouts.html'>Widget style &amp; layout</a></li>" 
		+ "</ul>";
	// var rowlistarticle = document.getElementById('tbl002').insertRow(-1);
	// var celllistarticle = rowlistarticle.insertCell(-1);
	// celllistarticle.style.padding="0 0 0 0";
	celllistarticle.innerHTML = fullAddressListArticle ;
	document.getElementById('live002').style.display = "none";
	document.getElementById('list002').style.display = "block";
	}

function loadExampleList(){
 
	var fullAddressListExample   = "<ul><li><a href='"+qturl+"examples.html'>All examples</a></li>" 
		+ "<li><a href='"+qturl+"tutorials.html'>All tutorials</a></li>"  
		+ "<li><a href='"+qturl+"#'>Qt Quick examples</a></li>"  
		+ "<li><a href='"+qturl+"#'>Desktop examples</a></li>"  
		+ "<li><a href='"+qturl+"#'>Desktop examples</a></li>"  
		+ "</ul>";
	// var rowlistexample = document.getElementById('tbl003').insertRow(-1);
	// var celllistexample = rowlistexample.insertCell(-1);
	// celllistexample.style.padding="0 0 0 0";
	celllistexample.innerHTML = fullAddressListExample ;
	document.getElementById('live003').style.display = "none";
	document.getElementById('list003').style.display = "block";
	
}


