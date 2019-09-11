//
//  DataReporterTests.m
//  DataReporterTests
//
//  Created by luojilab on 2018/11/10.
//  Copyright © 2018年 luojilab. All rights reserved.
//

#import <XCTest/XCTest.h>
#import "DataReporterManager.h"



@interface DataReporterTests : XCTestCase

@end

@implementation DataReporterTests

- (void)setUp {
    [super setUp];
    
    [DataReporterManager startMonitorReport];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}


- (void)testWrite {
    NSString *str = @"test";
    NSData *data = [NSData dataWithBytes:[str UTF8String] length:[str length]];
    [DataReporterManager saveData:data];
}

- (void)testBigDataWrite {
    for (NSUInteger i = 0; i < 1000; i++) {
        NSString *str = [NSString stringWithFormat:@"%ld",(long)i];
        NSData *data = [NSData dataWithBytes:[str UTF8String] length:[str length]];
        [DataReporterManager saveData:data];
    }
}

- (void)testNetworkChanged {
    [DataReporterManager reaWaken];
}

- (void)testStop {
    [DataReporterManager stopMonitorReport];
}



@end
