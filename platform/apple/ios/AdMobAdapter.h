//
//  AdMob.h
//  Supernova iOS
//
//  Created by Eduardo Doria Lima on 11/05/23.
//

#import <Foundation/Foundation.h>

@interface AdMobAdapter : NSObject

- (void)initializeAdMob;

- (void)loadInterstitial:(NSString *)adUnitID;

- (bool)isInterstitialAdLoaded;

- (void)showInterstitial;

@end
