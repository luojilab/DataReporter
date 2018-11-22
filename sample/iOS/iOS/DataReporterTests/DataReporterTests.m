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
    [DataReporterManager saveData:@"test"];
}

- (void)testBigDataWrite {
    for (NSUInteger i = 0; i < 1000; i++) {
        [DataReporterManager saveData:[NSString stringWithFormat:@"%ld",(long)i]];
    }
}

- (void)testNetworkChanged {
    [DataReporterManager reaWaken];
}

- (void)testStop {
    [DataReporterManager stopMonitorReport];
}



@end
