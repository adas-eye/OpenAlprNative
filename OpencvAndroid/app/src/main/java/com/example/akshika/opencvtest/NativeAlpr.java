package com.example.akshika.opencvtest;

public class NativeAlpr {
	static {
		System.loadLibrary("nativealpr");
	}

	static NativeAlpr nativeAlpr;

	//TODO CHANGE THIS, I DONT LIKE SINGLETONS...
	public static NativeAlpr getInstance(){

		if(nativeAlpr == null){
			nativeAlpr = new NativeAlpr();
		}
		return nativeAlpr;

	}


	public native String stringFromJNI();


	public native String recognize(String imgFilePath, int topN);


	public native String recognizeWithCountryNRegion(String country, String region,
													 String imgFilePath, int topN);


	public native String recognizeWithCountryRegionNConfig(String country,
														   String region, String imgFilePath, String configFilePath, int topN);

}
