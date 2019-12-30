//
//  WSMTS.m
//  WSMTS
//
//  Created by linkequn on 2017/4/22.
//  Copyright © 2017年 chinanetcenter. All rights reserved.
//

#import "WSMTS.h"
#import "WSMTSDefine.h"
#import "mts_lib.h"

@implementation WSMTS

static id<WSMTSDelegate> sDelegate;

static void _Con_Ballback(int room_id, bool bRet) {
    if ([[WSMTS delegate] respondsToSelector:@selector(wsmtsConnectWithResult:result:)]) {
        [[WSMTS delegate] wsmtsConnectWithResult:room_id result:bRet ? YES : NO];
    }
}

static void _Discon_Callback(int room_id) {
    if ([[WSMTS delegate] respondsToSelector:@selector(wsmtsDisconnectWithRoomID:)]) {
        [[WSMTS delegate] wsmtsDisconnectWithRoomID:room_id];
    }
}

static void _Enter_Room_Callback(int room_id, int err_code) {
    if ([[WSMTS delegate] respondsToSelector:@selector(wsmtsEnterRoom:withErrorCode:)]) {
        [[WSMTS delegate] wsmtsEnterRoom:room_id withErrorCode:err_code];
    }
}

static void _Barrage_Callback(int room_id, const char *szbuf, int nsize) {
    if ([[WSMTS delegate] respondsToSelector:@selector(wsmtsReceiveBarrage:message:)]) {
        NSData *data = [NSData dataWithBytes:szbuf length:nsize];
        [[WSMTS delegate] wsmtsReceiveBarrage:room_id message:data];
    }
}

static void _Chat_Callback(int room_id, const char* tg_cid,const char* szbuf,int nsize) {
    if ([[WSMTS delegate] respondsToSelector:@selector(wsmtsReceivePrivateChatWithCID:cid:message:)]) {
        NSData *data = [NSData dataWithBytes:szbuf length:nsize];
        NSString *stringID = [NSString stringWithCString:tg_cid  encoding:NSUTF8StringEncoding];
        [[WSMTS delegate] wsmtsReceivePrivateChatWithCID:room_id cid:stringID message:data];
    }
}

+ (id<WSMTSDelegate>)delegate {
    id<WSMTSDelegate> result;

    @synchronized(self) {
        result = sDelegate;
    }
    return result;
}

+ (void)setDelegate:(id<WSMTSDelegate>)newValue {
    @synchronized(self) {
        sDelegate = newValue;
    }
}

+ (BOOL)initWithAppID:(nonnull NSString *)gdm_server
            port:(NSInteger)port
            appid:(NSInteger)appid {
    bool result = init(gdm_server.UTF8String, (int)port, (int)appid);
    if (result) {
        set_callback(_Con_Ballback, _Discon_Callback, _Enter_Room_Callback, _Barrage_Callback,
                     _Chat_Callback);
    }
    NSLog(@"Init WSMTS(%@) %@.", __WSMTS_SDK_VERSION__, result ? @"Success" : @"Fail");
    return result ? YES : NO;
}

+ (BOOL)enterRoomWithID:(NSInteger)roomID authority:(NSString *)authority {
    return enter_room((int)roomID, [authority cStringUsingEncoding:NSUTF8StringEncoding]);
}

+ (BOOL)sendBarrageWithPriority:(NSInteger)roomID message:(nonnull NSData *)message {
    return send_barrage((int)roomID, (const char *)message.bytes, (int)message.length);
}

+ (BOOL)sendPrivateChatWithCID:(NSInteger)roomID send_cid:(nonnull NSString *)send_cid recv_cid:(nonnull NSString *)recv_cid message:(nonnull NSData *)message {
    return send_private_chat((int)roomID, send_cid.UTF8String, recv_cid.UTF8String, (const char *)message.bytes, (int)message.length);
}

+ (BOOL)leaveRoomWithID:(NSInteger)roomID {
    return leave_room((int)roomID);
}

+ (void)finish {
    finish();
}

/**
 获取当前版本号
 
 @return 版本号
 */
+ (NSString*)version
{
    const char *c = get_mts_sdk_version();
    NSString *ret = [NSString stringWithCString:c encoding:NSUTF8StringEncoding];
    return ret;
}


@end
