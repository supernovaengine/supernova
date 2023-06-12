//
//  AdMob.h
//  Supernova iOS
//
//  Created by Eduardo Doria Lima on 11/05/23.
//

#import <Foundation/Foundation.h>

@interface AdMobAdapter : NSObject

- (nonnull instancetype)init;

- (void)initializeAdMob;

- (void)tagForChildDirectedTreatment:(Boolean)enable;

- (void)tagForUnderAgeOfConsent:(Boolean)enable;

- (void)loadInterstitial:(nonnull NSString *)adUnitID;

- (bool)isInterstitialAdLoaded;

- (void)showInterstitial;

@end
