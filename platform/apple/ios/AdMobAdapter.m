//
//  AdMob.m
//  Supernova iOS
//
//  Created by Eduardo Doria Lima on 11/05/23.
//

#import "AdMobAdapter.h"

#import "AppDelegate.h"
#import "ViewController.h"
#import "Renderer.h"

@import GoogleMobileAds;

@interface AdMobAdapter () <GADFullScreenContentDelegate>

@property(nonatomic, strong) GADInterstitialAd *interstitial;

@property(nonatomic, strong) ViewController *rootController;

@end

@implementation AdMobAdapter

- (nonnull instancetype)init {
    
    _rootController = (ViewController*)[[(AppDelegate*) [[UIApplication sharedApplication]delegate] window] rootViewController];

    return self;
}

- (void)initializeAdMob {
    [GADMobileAds.sharedInstance startWithCompletionHandler:nil];
}

- (void)tagForChildDirectedTreatment:(Boolean)enable {
    if (enable){
        [GADMobileAds.sharedInstance.requestConfiguration tagForChildDirectedTreatment:YES];
    }else{
        [GADMobileAds.sharedInstance.requestConfiguration tagForChildDirectedTreatment:NO];
    }
}

- (void)tagForUnderAgeOfConsent:(Boolean)enable {
    if (enable){
        [GADMobileAds.sharedInstance.requestConfiguration tagForUnderAgeOfConsent:YES];
    }else{
        [GADMobileAds.sharedInstance.requestConfiguration tagForUnderAgeOfConsent:NO];
    }
}

- (void)loadInterstitial:(NSString *)adUnitID {
    GADRequest *request = [GADRequest request];
    [GADInterstitialAd
     loadWithAdUnitID:adUnitID
     request:request
     completionHandler:^(GADInterstitialAd *ad, NSError *error) {
        if (error) {
            NSLog(@"Failed to load interstitial ad with error: %@", [error localizedDescription]);
            return;
        }
        self.interstitial = ad;
        self.interstitial.fullScreenContentDelegate = self;
    }];
}

- (bool)isInterstitialAdLoaded {
    return (self.interstitial != nil);
}

- (void)showInterstitial {
    if (self.interstitial) {
        [self.interstitial presentFromRootViewController:_rootController];
    } else {
        NSLog(@"Ad wasn't ready");
    }
}

/// Tells the delegate that the ad failed to present full screen content.
- (void)ad:(nonnull id<GADFullScreenPresentingAd>)ad
didFailToPresentFullScreenContentWithError:(nonnull NSError *)error {
    [Renderer resumeGame];
    NSLog(@"Ad did fail to present full screen content.");
}

/// Tells the delegate that the ad will present full screen content.
- (void)adWillPresentFullScreenContent:(nonnull id<GADFullScreenPresentingAd>)ad {
    [Renderer pauseGame];
    NSLog(@"Ad will present full screen content.");
}

/// Tells the delegate that the ad dismissed full screen content.
- (void)adDidDismissFullScreenContent:(nonnull id<GADFullScreenPresentingAd>)ad {
    [Renderer resumeGame];
   NSLog(@"Ad did dismiss full screen content.");
}

@end
