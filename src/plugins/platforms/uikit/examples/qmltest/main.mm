//
//  main.m
//  qmltest
//
//  Created by Eike Troll on 18.02.11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#include "qmlapplicationviewer/qmlapplicationviewer.h"

#include <QtGui/QApplication>
#include <QtCore/QtPlugin>

Q_IMPORT_PLUGIN(UIKit)

int main(int argc, char *argv[]) {
    
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
		
	setenv("QT_QPA_PLATFORM","uikit",1);

	QApplication app(argc, argv);
    QmlApplicationViewer viewer;
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
	NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
    viewer.setMainQmlFile(QString::fromUtf8([[resourcePath stringByAppendingPathComponent:@"qml/main.qml"] UTF8String]));
    viewer.showMaximized();
	int retVal = app.exec();
    [pool release];
    return retVal;
}
