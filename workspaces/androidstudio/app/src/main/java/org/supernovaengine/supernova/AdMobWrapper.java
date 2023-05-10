package org.supernovaengine.supernova;

import android.app.Activity;
import android.content.Context;
import android.util.Log;

import androidx.annotation.NonNull;

import com.google.android.gms.ads.AdError;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.FullScreenContentCallback;
import com.google.android.gms.ads.LoadAdError;
import com.google.android.gms.ads.MobileAds;
import com.google.android.gms.ads.initialization.InitializationStatus;
import com.google.android.gms.ads.initialization.OnInitializationCompleteListener;
import com.google.android.gms.ads.interstitial.InterstitialAd;
import com.google.android.gms.ads.interstitial.InterstitialAdLoadCallback;

public class AdMobWrapper {
    private Activity activity;

    private static final String AD_UNIT_ID = "ca-app-pub-3940256099942544/1033173712";

    private InterstitialAd interstitialAd;

    public AdMobWrapper(final Activity activity) {
        this.activity = activity;
    }

    public void initialize(){
        activity.runOnUiThread(new Runnable() {
            @Override public void run() {
                // Initialize the Mobile Ads SDK.
                MobileAds.initialize(activity, new OnInitializationCompleteListener() {
                    @Override
                    public void onInitializationComplete(InitializationStatus initializationStatus) {}
                });
            }
        });
    }

    public void loadInterstitialAd() {
        activity.runOnUiThread(new Runnable() {
            @Override public void run() {

                AdRequest adRequest = new AdRequest.Builder().build();
                InterstitialAd.load(
                        activity,
                        AD_UNIT_ID,
                        adRequest,
                        new InterstitialAdLoadCallback() {
                            @Override
                            public void onAdLoaded(@NonNull InterstitialAd interstitialAd) {
                                // The mInterstitialAd reference will be null until
                                // an ad is loaded.
                                AdMobWrapper.this.interstitialAd = interstitialAd;

                                interstitialAd.setFullScreenContentCallback(
                                        new FullScreenContentCallback() {
                                            @Override
                                            public void onAdDismissedFullScreenContent() {
                                                // Called when fullscreen content is dismissed.
                                                // Make sure to set your reference to null so you don't
                                                // show it a second time.
                                                AdMobWrapper.this.interstitialAd = null;
                                            }

                                            @Override
                                            public void onAdFailedToShowFullScreenContent(AdError adError) {
                                                // Called when fullscreen content failed to show.
                                                // Make sure to set your reference to null so you don't
                                                // show it a second time.
                                                AdMobWrapper.this.interstitialAd = null;
                                                Log.e("Supernova", "The ad failed to show.");
                                            }

                                            @Override
                                            public void onAdShowedFullScreenContent() {
                                                // Called when fullscreen content is shown.
                                            }
                                        });
                            }

                            @Override
                            public void onAdFailedToLoad(@NonNull LoadAdError loadAdError) {
                                // Handle the error
                                Log.e("Supernova", loadAdError.getMessage());
                                interstitialAd = null;
                            }
                        });

            }
        });

    }

    public boolean isInterstitialAdLoaded(){
        return interstitialAd != null;
    }

    public void showInterstitialAd() {
        activity.runOnUiThread(new Runnable() {
            @Override public void run() {

                if (isInterstitialAdLoaded()) {
                    interstitialAd.show(activity);
                }

            }
        });
    }
}
