package {
	import of.*; // ofxFlashCommunication sources.
	import flash.display.MovieClip;
	import flash.events.*;
	
	public class ExampleApp extends flash.display.MovieClip {
		private var comm:ofxFlashCommunication = null;
		public function ExampleApp():void {
			comm = new ofxFlashCommunication("localhost",1234);
			comm.addEventListener(ofxFlashCommunicationEvent.NEW_DATA, onNewData);
			trace("Example App");
			this['send_btn'].addEventListener(MouseEvent.CLICK, onSendClick);
		}
		
		private function onSendClick(oEv:MouseEvent):void {
			comm.send(">>> test data");
		}
		
		private function onNewData(oEv:ofxFlashCommunicationEvent):void {
			trace("<<< " +oEv.getMessage());
		}
	}
}
