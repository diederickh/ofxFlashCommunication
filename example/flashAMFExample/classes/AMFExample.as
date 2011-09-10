package {
	/**
	 * This file contains a very basic example on how to use flash remoting
	 * to send/receive some trivial data to a openFrameworks application using 
	 * AMF.
	 * 
	 * Note that this code purely shows an example on how to send/receive 
	 * data to/from flash using AMF. For clarity I'm recreating a AMF instance
	 * for each call. 
	 *
	 * @author	Diederick Huijbers <info#roxlu.com>, www.roxlu.com
	 */
	import flash.events.IOErrorEvent;
	import flash.events.NetStatusEvent;
	import flash.events.SecurityErrorEvent;
	import flash.events.Event;
	import flash.events.MouseEvent;
	import flash.net.NetConnection;
	import flash.net.Responder;
	import flash.display.MovieClip;
	import flash.text.TextField;
	
	public class AMFExample extends flash.display.MovieClip {
		public function AMFExample():void {
			var me:AMFExample = this;
			
			this['bt_send'].addEventListener(MouseEvent.CLICK, function(ev:MouseEvent):void {
				
				// some validation.
				if(me['tf_target_uri'].text == "") {
					me.log("No target URI set.");
					return;
				}
				if(me['tf_host'].text == "") {
					me.log("No host given");
					return;
				}
				if(me['tf_port'].text == "") {
					me.log("No port set");
					return;
				}
				
				// create a new netconnection and add some event handlers.
				var amf_con:flash.net.NetConnection = new NetConnection();
				amf_con.addEventListener(
					NetStatusEvent.NET_STATUS
					,function(ev:NetStatusEvent) {
						me.log(ev.toString());
					}
				);
				amf_con.addEventListener(
					IOErrorEvent.IO_ERROR
					,function(ev:IOErrorEvent):void {
						me.log(ev.toString());
					}
				);
					
				amf_con.addEventListener(
					SecurityErrorEvent.SECURITY_ERROR
					,function(ev:SecurityErrorEvent) {
						me.log(ev.toString());
					}
				);
				
				// create the url on which the ofxAMFServer is running.
				var amf_url = 'http://' +me['tf_host'].text +":" +me['tf_port'].text;
				amf_con.connect(amf_url);
				
				// create dataobject to send.
				var data_obj:Object = {data:me['tf_data'].text};
				
				// empty the message....
				me['tf_data'].text = "";
				me.log(">> calling: " +me['tf_target_uri'].text);
				
				// and make the call!
				amf_con.call(
					me['tf_target_uri'].text
					,new Responder(
						function(e:*):void{
							if(typeof(e) == "object") {
								var logstr:String = "";
								for (var el:* in e) {
									logstr += "result[" +el +"] = " +e[el] +", ";
								}
								me.log(logstr);
							}
						}
						,function(e:*):void{
							me.log("Error calling: "+me['tf_target_ur'].text);
						}
					)
					,data_obj
				);
				
			});
			
		}
		
		private function log(msg:String):void {
			var d:Date = new Date();
			msg = d.getHours() +":" +d.getMinutes() +"\t" +msg;
			var curr_text:String = this['tf_log'].text;
			var lines:Array = curr_text.split("\n");
			var stop_at:int = Math.max(0, lines.length-10);
			var new_text:String = "";
			for(var i = lines.length-1; i >= stop_at; --i) {
					new_text = lines[i] +"\n";
			}
			this['tf_log'].text = msg +"\n" +new_text;	
		}
	}
}
