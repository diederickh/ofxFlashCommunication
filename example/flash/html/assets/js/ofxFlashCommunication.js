var fc = null;
var ofxFlashCommunication = new Class({
	initialize:function() {
		var oo = new Swiff('assets/swf/ofxFlashCommunicationFlash4.swf?' +new Date(), {
			 id:'ofxflashcommunicatio_swf'
			,container: $('ofxflashcommunication_wrapper')
			,width:"100%"
			,height:"100%"
		});

	}
});

window.addEvent('domready',function(){
	fc = new ofxFlashCommunication();
});

