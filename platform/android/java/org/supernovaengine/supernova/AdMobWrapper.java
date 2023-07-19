package org.supernovaengine.supernova;

import android.app.Activity;
import android.content.Context;
import android.provider.Settings;
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
import com.google.android.ump.ConsentDebugSettings;
import com.google.android.ump.ConsentForm;
import com.google.android.ump.ConsentInformation;
import com.google.android.ump.ConsentRequestParameters;
import com.google.android.ump.FormError;
import com.google.android.ump.UserMessagingPlatform;

public class AdMobWrapper {
    private Activity activity;

    private InterstitialAd interstitialAd;

    private ConsentInformation consentInformation;
    private ConsentForm consentForm;

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

                ConsentDebugSettings debugSettings = new ConsentDebugSettings.Builder(activity)
                        .setDebugGeography(ConsentDebugSettings.DebugGeography.DEBUG_GEOGRAPHY_EEA)
                        .addTestDeviceHashedId("TEST-DEVICE-HASHED-ID")
                        .build();

                // Set tag for under age of consent. false means users are not under
                // age.
                ConsentRequestParameters params = new ConsentRequestParameters
                        .Builder()
                        //.setConsentDebugSettings(debugSettings)
                        .setTagForUnderAgeOfConsent(false)
                        .build();

                consentInformation = UserMessagingPlatform.getConsentInformation(activity);
                //consentInformation.reset();
                consentInformation.requestConsentInfoUpdate(
                        activity,
                        params,
                        new ConsentInformation.OnConsentInfoUpdateSuccessListener() {
                            @Override
                            public void onConsentInfoUpdateSuccess() {
                                // The consent information state was updated.
                                // You are now ready to check if a form is available.
                                if (consentInformation.isConsentFormAvailable()) {
                                    //loadForm();
                                }
                            }
                        },
                        new ConsentInformation.OnConsentInfoUpdateFailureListener() {
                            @Override
                            public void onConsentInfoUpdateFailure(FormError formError) {
                                // Handle the error.
                            }
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

                ConsentRequestParameters params = new ConsentRequestParameters
                        .Builder()
                        .setTagForUnderAgeOfConsent(false)
                        .build();
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

    public void loadForm() {
        // Loads a consent form. Must be called on the main thread.
        UserMessagingPlatform.loadConsentForm(
                activity,
                new UserMessagingPlatform.OnConsentFormLoadSuccessListener() {
                    @Override
                    public void onConsentFormLoadSuccess(ConsentForm consentForm) {
                        AdMobWrapper.this.consentForm = consentForm;
                        if (consentInformation.getConsentStatus() == ConsentInformation.ConsentStatus.REQUIRED) {
                            consentForm.show(
                                    activity,
                                    new ConsentForm.OnConsentFormDismissedListener() {
                                        @Override
                                        public void onConsentFormDismissed(FormError formError) {
                                            if (consentInformation.getConsentStatus() == ConsentInformation.ConsentStatus.OBTAINED) {
                                                // App can start requesting ads.
                                            }

                                            // Handle dismissal by reloading form.
                                            loadForm();
                                        }
                                    });
                        }
                    }
                },
                new UserMessagingPlatform.OnConsentFormLoadFailureListener() {
                    @Override
                    public void onConsentFormLoadFailure(FormError formError) {
                        // Handle Error.
                    }
                }
        );
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
                // avoid show ads when Firebase Test Lab
                String testLabSetting = Settings.System.getString(activity.getContentResolver(), "firebase.test.lab");
                if (!"true".equals(testLabSetting)) {

                    if (isInterstitialAdLoaded()) {
                        interstitialAd.show(activity);
                    }

                }
            }
        });
    }
}
