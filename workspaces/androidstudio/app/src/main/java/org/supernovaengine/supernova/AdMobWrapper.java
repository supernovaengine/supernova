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
import com.google.android.gms.ads.RequestConfiguration;
import com.google.android.gms.ads.initialization.InitializationStatus;
import com.google.android.gms.ads.initialization.OnInitializationCompleteListener;
import com.google.android.gms.ads.interstitial.InterstitialAd;
import com.google.android.gms.ads.interstitial.InterstitialAdLoadCallback;

public class AdMobWrapper {
    private Activity activity;

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

    public void tagForChildDirectedTreatment(boolean enable){
        activity.runOnUiThread(new Runnable() {
            @Override public void run() {
                int tagValue;
                if (enable) {
                    tagValue = RequestConfiguration.TAG_FOR_CHILD_DIRECTED_TREATMENT_TRUE;
                }else{
                    tagValue = RequestConfiguration.TAG_FOR_CHILD_DIRECTED_TREATMENT_FALSE;
                }

                RequestConfiguration requestConfiguration = MobileAds.getRequestConfiguration()
                        .toBuilder()
                        .setTagForChildDirectedTreatment(tagValue)
                        .build();
                MobileAds.setRequestConfiguration(requestConfiguration);
            }
        });
    }

    public void tagForUnderAgeOfConsent(boolean enable){
        activity.runOnUiThread(new Runnable() {
            @Override public void run() {
                int tagValue;
                if (enable) {
                    tagValue = RequestConfiguration.TAG_FOR_UNDER_AGE_OF_CONSENT_TRUE;
                }else{
                    tagValue = RequestConfiguration.TAG_FOR_UNDER_AGE_OF_CONSENT_FALSE;
                }

                RequestConfiguration requestConfiguration = MobileAds.getRequestConfiguration()
                        .toBuilder()
                        .setTagForUnderAgeOfConsent(tagValue)
                        .build();
                MobileAds.setRequestConfiguration(requestConfiguration);
            }
        });
    }

    public void loadInterstitialAd(String adUnitID) {
        activity.runOnUiThread(new Runnable() {
            @Override public void run() {

                AdRequest adRequest = new AdRequest.Builder().build();
                InterstitialAd.load(
                        activity,
                        adUnitID,
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
