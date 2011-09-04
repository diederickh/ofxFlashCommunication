package of {
	import flash.events.Event;
	
	public class ofxFlashCommunicationEvent extends flash.events.Event {
		/**
		 * @param	NEW_DATA			This event is dispatched when we 
		 *							receive new data from the server.
		 */
		public static const NEW_DATA:String = "new_data";
		
		/**
		 * @param	data		General data container for the 
		 *					ofxFlashCommunication lib. When 
		 * 					we receive new data from the server, a 
		 *					ofxFlashCommunicationEvent is dispatched 
		 *					containing the received message.
		 *
		 */
		public var data:Object;
		
		public function ofxFlashCommunicationEvent(
			 sType:String
			,oData:Object
			,bBubbles:Boolean = false
			,bCancelable:Boolean = false
		)
		{
			super(sType, bBubbles, bCancelable);
			data = oData;
		}
		
		public function getMessage():String {
			return data.message;
		}
			
	}
}
