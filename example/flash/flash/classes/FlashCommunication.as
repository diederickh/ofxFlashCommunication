package {
	import of.*; // ofxFlashCommunication sources.	
	import flash.display.MovieClip;
	import flash.events.MouseEvent;
	import flash.text.TextField;
	import flash.display.StageAlign;
	import flash.display.StageScaleMode;
	
	public class FlashCommunication extends flash.display.MovieClip {
		private var comm:ofxFlashCommunication = null;
	
		public function FlashCommunication():void {
			stage.align = StageAlign.TOP_LEFT;
			stage.scaleMode = StageScaleMode.NO_SCALE;
			
			comm = new ofxFlashCommunication("192.168.0.200",1234);
			comm.addEventListener(
				ofxFlashCommunicationEvent.NEW_DATA
				,onNewData
			);
			this['bt_send'].addEventListener(MouseEvent.CLICK, onSendClick);
		}

		private function onSendClick(oEv:MouseEvent):void {
			comm.send(tf_msg.text);
		}

		private function onNewData(oEv:ofxFlashCommunicationEvent):void {
			log(oEv.getMessage());
		}
		
		private function log(msg:String):void  {
			tf_log.text = msg +"\n" +tf_log.text;
		}
	};
};
