	package of {
	import flash.system.Security;
	import flash.net.Socket;
	import flash.events.*;
	import flash.errors.IOError;
	import flash.utils.Timer;
	
	public class ofxFlashCommunication extends EventDispatcher{
		private var port_:Number = 1234;
		private var host_:String = "localhost";
		private var socket_:Socket = null;
		private var timer_:Timer;
		private var is_connecting_:Boolean = false;
		
		public function ofxFlashCommunication(sHost:String, nPort:Number):void {
			port_ = nPort;
			host_ = sHost;
			
			Security.loadPolicyFile("xmlsocket://" +sHost +":" + nPort);
			
			socket_ = new Socket();
			socket_.addEventListener(Event.CLOSE, onClose);
			socket_.addEventListener(Event.CONNECT, onConnect);
			socket_.addEventListener(ProgressEvent.SOCKET_DATA, onSocketData);
			socket_.addEventListener(IOErrorEvent.IO_ERROR, onIOError);
			socket_.addEventListener(SecurityErrorEvent.SECURITY_ERROR, onSecurityError);
			connect();
			
			trace("ofxFlashCommunication: " +sHost +"(" +nPort +")");
			timer_ = new Timer(3000, 0);
			timer_.addEventListener(TimerEvent.TIMER, onTimer);
			timer_.start();
		}
		
		// connect to remote host.
		private function connect():void {
			socket_.connect(host_, port_);			
			is_connecting_ = true;
		}
		
		/**
		 * This method is called when we receive some data from the server.
		 * The received data is read as a string ending with a newline.
		 * The sending format is:
		 *
		 * [string_message][0x00]
		 *
		 * This method dispatches a 
		 */
		private function onSocketData(oEv:ProgressEvent):void {
			var data:String = socket_.readUTFBytes(socket_.bytesAvailable-1);
			var tmp:String = socket_.readUTFBytes(1);
			var ev:ofxFlashCommunicationEvent = new ofxFlashCommunicationEvent(
				ofxFlashCommunicationEvent.NEW_DATA
				,{message:data}
			);
			dispatchEvent(ev);
		}
		
		/**
		 * Send data to the flash communication server.
		 *
		 * @param	string			Data you want to send; this should be
		 *							a string w/o line ending.
		 */
		public function send(sData:String):void {
			socket_.writeUTFBytes(sData);
			socket_.writeByte(0x00);
			flush();
		}
		
		// flush data; send to host.
		private function flush():void {
			if(socket_.connected) {
				try {
					socket_.flush();
				}
				catch(oErr:IOError) {
					trace("ERROR: ofxFlashCommunication: cannot connect to server");
					trace(oErr);
				}
			}
			else {
				connect();
			}
		}
		
		private function onConnect(oEv:Event):void {
			is_connecting_ = false;
			trace("ofxFlashCommunication: connected to host");
		}
		
		private function onClose(oEv:Event):void {
			is_connecting_ = false;
			trace("ofxFlashCommunication: connection closed.");
		}
		
		private function onIOError(oEv:Event):void {
			is_connecting_ = false;
			trace("ofxFlashCommunication: io error:" +oEv);
		}
		
		private function onSecurityError(oEv:SecurityErrorEvent):void {
			is_connecting_ = false;
			trace("ofxFlashCommunication: security error: " +oEv);
		}
		
		/**
		 * Connection timer.
		 *
		 * @param	event
		 */
		private function onTimer(oEv:TimerEvent):void {
			if(!socket_.connected && !is_connecting_) {
				connect();
			}
		}
	}
}
