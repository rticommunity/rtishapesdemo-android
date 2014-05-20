package com.rti.android.shapesdemo.util;

import java.net.Inet6Address;
import java.net.InetAddress;
import java.net.InterfaceAddress;
import java.net.NetworkInterface;
import java.net.UnknownHostException;
import java.util.List;

import android.content.Context;
import android.os.AsyncTask;
import android.preference.PreferenceCategory;
import android.util.AttributeSet;
import android.util.Log;

public class DeviceIpAddressDisplay extends PreferenceCategory {

	public DeviceIpAddressDisplay(Context context, AttributeSet attrs)
	{
		super(context, attrs);
		
		// Can't call IP address API from main thread, must use other thread
		new SetTitleTask().execute(this);
	}
	
	public DeviceIpAddressDisplay(Context context) {
		super(context);
		
		// Can't call IP address API from main thread, must use other thread
		new SetTitleTask().execute(this);
	}
	
	private class SetTitleTask extends AsyncTask<DeviceIpAddressDisplay, Void, Void> {

		@Override
		protected Void doInBackground(DeviceIpAddressDisplay... params) 
		{
			
			DeviceIpAddressDisplay display = params[0];
			String ipAddr = "Unknown";
			
			try {

			
				List<InterfaceAddress> addresses = NetworkInterface.getByName("wlan0").getInterfaceAddresses();
				
				for (InterfaceAddress address : addresses ) {
					// Filter out IPv6 address
					if (address.getAddress() instanceof Inet6Address) {
							continue;
					}
					if (!address.getAddress().isLoopbackAddress()) {
						ipAddr = address.getAddress().getHostAddress();
						break;
					}
				}
				
			} catch (Exception e) {
				// TODO Auto-generated catch block
				Log.e("DeviceIpAddressDisplay", "Could not get local IP address : " + e.getMessage());
			}
			
			display.setTitle("Device IP address: " + ipAddr);
			return null;
		}

	}
}
