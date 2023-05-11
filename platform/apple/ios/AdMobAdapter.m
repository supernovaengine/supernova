//
//  AdMob.m
//  Supernova iOS
//
//  Created by Eduardo Doria Lima on 11/05/23.
//

#import "AdMobAdapter.h"

#import "AppDelegate.h"

@import GoogleMobileAds;

@interface AdMobAdapter () <GADFullScreenContentDelegate>

@property(nonatomic, strong) GADInterstitialAd *interstitial;

@end

@implementation AdMobAdapter

- (void)initializeAdMob {
    [GADMobileAds.sharedInstance startWithCompletionHandler:nil];
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
    UIViewController *rootController = (UIViewController*)[[(AppDelegate*)
                                                            [[UIApplication sharedApplication]delegate] window] rootViewController];
    if (self.interstitial) {
        [self.interstitial presentFromRootViewController:rootController];
    } else {
        NSLog(@"Ad wasn't ready");
    }
}

/// Tells the delegate that the ad failed to present full screen content.
- (void)ad:(nonnull id<GADFullScreenPresentingAd>)ad
didFailToPresentFullScreenContentWithError:(nonnull NSError *)error {
    NSLog(@"Ad did fail to present full screen content.");
}

/// Tells the delegate that the ad will present full screen content.
- (void)adWillPresentFullScreenContent:(nonnull id<GADFullScreenPresentingAd>)ad {
    NSLog(@"Ad will present full screen content.");
}

/// Tells the delegate that the ad dismissed full screen content.
- (void)adDidDismissFullScreenContent:(nonnull id<GADFullScreenPresentingAd>)ad {
   NSLog(@"Ad did dismiss full screen content.");
}

@end
