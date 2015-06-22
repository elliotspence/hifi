//
//  ShapeInfoTests.h
//  tests/physics/src
//
//  Created by Andrew Meadows on 2014.11.02
//  Copyright 2014 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#ifndef hifi_ShapeInfoTests_h
#define hifi_ShapeInfoTests_h

#include <QtTest/QtTest>

//// Add additional qtest functionality (the include order is important!)
//#include "BulletTestUtils.h"
//#include "../QTestExtensions.hpp"

// Enable this to manually run testHashCollisions
// (NOT a regular unit test; takes ~17 secs to run on an i7)
#define MANUAL_TEST false

class ShapeInfoTests : public QObject {
    Q_OBJECT
private slots:
    void testHashFunctions();
    void testBoxShape();
    void testSphereShape();
    void testCylinderShape();
    void testCapsuleShape();
};

#endif // hifi_ShapeInfoTests_h
