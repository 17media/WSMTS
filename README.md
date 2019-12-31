# WSMTS

- [Installation](#installation)
- [Interface](#interface)
- [Demo](#demo)

## Installation
### Prerequisites
To use the WSMTS for iOS Swift and Objective-C, you need:
- iOS 6.0 or later as the deployment target.
- Xcode 11 or later.
- WSMTS supports armv7, armv7s, arm64, i386 and x86_64 architectures.
- WSMTS supports Bitcode.

### CocoaPods
[CocoaPods](https://cocoapods.org/) is a dependency manager for Cocoa projects. For usage and installation instructions, visit their website. To integrate WSMTS into your Xcode project using CocoaPods, specify it in your _Podfile_:

    pod 'WSMTS', '1.0.1'
    
### Carthage
[Carthage](https://github.com/Carthage/Carthage) is a decentralized dependency manager that builds your dependencies and provides you with binary frameworks. To integrate WSMTS into your Xcode project using Carthage, specify it in your _Cartfile_:

    github "17media/WSMTS" "1.0.1"
    
### Mannually
1. Add _WSMTS.framework_ into your project.
1. Add dependent library `stdc++`.
1. Import header file `#import <WSMTS/WSMTS.h>`

## Interface
### Parameter
1. Please refer `WSMTS.h`.
1. Make sure you call `initWithAppID` before using SDK. This function creates a background thread to run the SDK. 
1. Call `finish` to release the SDK.
1. Passing _0_ into `leaveRoomWithID` would make WSMTS leave all rooms.

## Demo

```objective-c
#import <WSMTS/WSMTS.h>
#import "ViewController.h"

@interface ViewController ()<WSMTSDelegate>

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    // Initialize SDK
    BOOL result = [WSMTS initWithAppID:@"sdk.wekt.net" port:60000 appid:996];
    if (result) {
      	// Setup callback
      	[WSMTS setDelegate:delegate:self]
        // Enter room
        result = [WSMTS enterRoomWithID:1001 authority:@""];
        // Send barrage
        result = [WSMTS sendBarrageWithPriority:self.loginInfo.roomID message:messageData];
    }
}

#pragma - WSMTSDelegate

- (void)wsmtsConnectWithResult:(NSInteger)roomID result:(BOOL)result {
    NSString *msg = [NSString stringWithFormat:@"roomid:%ld connect：%@", (long)roomID, result ? @"Sucess" : @"Failed"];
    [self displayMessage:msg];
    NSLog(@"%@", msg);
}

- (void)wsmtsDisconnectWithRoomID:(NSInteger)roomID {
    NSString *msg = [NSString stringWithFormat:@"disconnect roomid:%ld", (long)roomID];
    [self displayMessage:msg];
    NSLog(@"%@", msg);
}

- (void)wsmtsEnterRoom:(NSInteger)roomID withErrorCode:(NSInteger)errorCode {
    NSString *message;
    switch (errorCode) {
        case 0:
            message = @"sucess";
            break;
        case 1:
            message = @"get room address failed";
            break;
        case 2:
            message = @"socket disconnect failed";
            break;
        case 3:
            message = @"authentication failed";
            break;
		case 4:
			message = @"domain name parsing failed";
            break;
		case 5:
			message = @"incorrect room ID";
            break;
        case 10:
            message = @"others error";
            break;
        default:
            message = @"unknown error";
            break;
    }
    NSString *msg = [NSString stringWithFormat:@"enter roomid:%ld, error code:%ld, description:%@", (long)roomID, (long)errorCode, message];
    [self displayMessage:msg];
    NSLog(@"%@", msg);

}

- (void)wsmtsReceiveBarrage:(NSInteger)roomID message:(NSData *)message {
    NSString *string = [[NSString alloc] initWithData:message encoding:NSUTF8StringEncoding];
    NSString *msg = [NSString stringWithFormat:@"room %ld receive barrage:%@", (long)roomID, string];
    [self displayMessage:msg];
    NSLog(@"%@", msg);
}

- (void)wsmtsReceivePrivateChatWithCID:(NSInteger)roomID cid:(NSString*)cid message:(NSData *)message {
    NSString *string = [[NSString alloc] initWithData:message encoding:NSUTF8StringEncoding];
    NSString *msg = [NSString stringWithFormat:@"room %ld receive private chat:%@, %@", (long)roomID, cid, string];
    [self displayMessage:msg];
    NSLog(@"%@", msg);
}

@end
```
