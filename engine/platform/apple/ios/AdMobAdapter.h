// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#import <Foundation/Foundation.h>

@interface AdMobAdapter : NSObject

- (nonnull instancetype)init;

- (void)initializeAdMob:(Boolean)tagForChildDirectedTreatment and:(Boolean)tagForUnderAgeOfConsent;

- (void)setMaxAdContentRating:(int)rating;

- (void)loadInterstitial:(nonnull NSString *)adUnitID;

- (bool)isInterstitialAdLoaded;

- (void)showInterstitial;

@end
