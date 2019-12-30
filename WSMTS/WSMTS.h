//
//  WSMTS.h
//  WSMTS
//
//  Created by linkequn on 2017/4/22.
//  Copyright © 2017年 chinanetcenter. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "global.h"

@protocol WSMTSDelegate<NSObject>
@optional
/**
 *  链接结果回调
 *
 *  @param roomID 房间号
 *  @param result NO表示失败，YES表示成功
 */
- (void)wsmtsConnectWithResult:(NSInteger)roomID result:(BOOL)result;

/**
 *  链接断开回调
 *
 *  @param roomID 房间号
 */
- (void)wsmtsDisconnectWithRoomID:(NSInteger)roomID;

/**
 *  每次调用[WSMTS enterRoomWithID:authority]接口，sdk通过此回调返回进入房间的结果
 *
 *  @param roomID    房间号
 *  @param errorCode 0 成功,1 获取房间网络地址失败,2 socket链接失败,3 鉴权失败,4 域名解析失败 5 房间号有误  10 其他错误
 */
- (void)wsmtsEnterRoom:(NSInteger)roomID withErrorCode:(NSInteger)errorCode;

/**
 *  收到弹幕数据
 *
 *  @param roomID 房间id
 *  @param message 弹幕数据
 */
- (void)wsmtsReceiveBarrage:(NSInteger)roomID message:(nullable NSData *)message;

/**
 *  收到私聊信息
 *
 *  @param roomID 房间id
 *  @param cid     对方cid
 *  @param message 私聊信息
 */
- (void)wsmtsReceivePrivateChatWithCID:(NSInteger)roomID cid:(NSString*_Nullable)cid message:(nullable NSData *)message;

@end

/**
 * MTS: Message Transfer System（信息传输系统）
 */
@interface WSMTS : NSObject

/**
*  初始化SDK
*
*  @param gdm_server     服务器地址	由网速科技提供
*  @param port 			 监听端口	通过协商确定
*  @param appid          客户编号   由网速科技提供
*
*  @return YES 初始化成功， NO 初始化失败
*/
+ (BOOL)initWithAppID:(nonnull NSString *)gdm_server
			port:(NSInteger)port
            appid:(NSInteger)appid;

/**
 *  设置回调
 *
 *  @param delegate 代理
 */
+ (void)setDelegate:(nullable id<WSMTSDelegate>)delegate;

/**
 *  进入房间
 *
 *  @param roomID    房间id
 *  @param authority 鉴权信息 可以不填填写
 *
 *  @return 当参数非法返回false (注：进入房间结果在wsmtsEnterRoom:withErrorCode:返回)
 */
+ (BOOL)enterRoomWithID:(NSInteger)roomID authority:(nonnull NSString *)authority;

/**
 *  发送弹幕消息
 *
 *  @param roomID
 *  @param message 发送字节流数据，服务器只做转发，不解析
 *
 *  @return YES 发送成功， NO 发送失败
 */
+ (BOOL)sendBarrageWithPriority:(NSInteger)roomID message:(nonnull NSData *)message;

/**
 *  发送私聊消息
 *
 *  @param roomID
 *  @param send_cid      发送者cid
 *  @param recv_cid  	 接受者cid
 *  @param message 发送字节流数据，服务器只做转发，不解析
 *
 *  @return YES 发送成功， NO 发送失败
 */
+ (BOOL)sendPrivateChatWithCID:(NSInteger)roomID send_cid:(nonnull NSString *)send_cid recv_cid:(nonnull NSString *)recv_cid message:(nonnull NSData *)message;

/**
 *  离开房间
 *
 *  @param roomID 房间号 当roomID = 0,表示离开已进入的所有房间
 *
 *  @return YES 成功， NO 失败
 */
+ (BOOL)leaveRoomWithID:(NSInteger)roomID;

/**
 *  退出SDK
 */
+ (void)finish;


/**
  版本信息

 @return 版本
 */
+ (NSString*)version;

@end
