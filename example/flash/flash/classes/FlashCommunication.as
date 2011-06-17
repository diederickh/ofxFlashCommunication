package {
	import of.*; // ofxFlashCommunication sources.	
	import flash.display.MovieClip;
	import flash.events.MouseEvent;
	import flash.text.TextField;
	public class FlashCommunication extends flash.display.MovieClip {
		private var comm:ofxFlashCommunication = null;
		public function FlashCommunication():void {
			trace("FlashCommunication");
			///comm = new ofxFlashCommunication("localhost",1234);
			comm = new ofxFlashCommunication("192.168.0.107",1234);
			comm.addEventListener(ofxFlashCommunicationEvent.NEW_DATA, onNewData);
			//this['send_btn'].addEventListener(MouseEvent.CLICK, onSendClick);
			log("ok");
			log("twee");
		}

		private function onSendClick(oEv:MouseEvent):void {
			comm.send(">>> test data");
		}

		private function onNewData(oEv:ofxFlashCommunicationEvent):void {
			trace("<<< " +oEv.getMessage());
			log(oEv.getMessage());
		}
		private function log(msg:String):void  {
			tf_log.text = msg +"\n" +tf_log.text;
		}
	};
};
