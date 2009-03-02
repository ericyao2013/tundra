// For conditions of distribution and use, see copyright notice in license.txt

#include <boost/test/unit_test.hpp>

#include "CoreStdIncludes.h"
#include "Core.h"
#include "Foundation.h"

BOOST_AUTO_TEST_SUITE(test_suite_core)

BOOST_AUTO_TEST_CASE( core_vector3 )
{
    Core::Vector3df test;
    BOOST_CHECK_EQUAL (test, Core::Vector3df(0, 0, 0));

    test = Core::Vector3df(1, 1, 1);
    BOOST_CHECK_EQUAL (test, Core::Vector3D<Core::f32>::UNIT_SCALE_X);

    test = Core::Vector3df(1, 0, 0);
    BOOST_CHECK_EQUAL (test, Core::Vector3D<Core::f32>::UNIT_X);
    BOOST_CHECK_EQUAL (test.getLength(), 1.f);

    test = -test;
    BOOST_CHECK_EQUAL (test, Core::Vector3D<Core::f32>::NEGATIVE_UNIT_X);

    test *= 5.f;
    test.invert();
    BOOST_CHECK_EQUAL (test, Core::Vector3df(5, 0, 0));
    BOOST_CHECK_EQUAL (test.getLengthSQ(), 25.f);

    test.normalize();
    BOOST_CHECK_EQUAL (test.getLength(), 1.f);

    test.set(2, 0, 0);
    Core::Vector3df other;
    BOOST_CHECK_EQUAL (test.getDistanceFrom(other), 2.f);
    BOOST_CHECK_EQUAL (test.getDistanceFromSQ(other), 4.f);

    other.set(2, 2, 0);
    BOOST_CHECK_EQUAL (test.crossProduct(other), Core::Vector3df(0, 0, 4));
    BOOST_CHECK_EQUAL (test.dotProduct(other), 4.f);

    other = test * Core::Vector3D<Core::f32>::UNIT_SCALE_X;
    BOOST_CHECK_EQUAL (test, other);

    test /= 2.f;
    BOOST_CHECK_EQUAL (test, Core::Vector3D<Core::f32>::UNIT_X);

}

BOOST_AUTO_TEST_CASE( core_quaternion )
{
    Core::Quaternion test;
    BOOST_CHECK_EQUAL (test, Core::Quaternion::IDENTITY);

    test.set(1, 2, 3, 1);
    test.normalize();
    Core::Quaternion other = test * Core::Quaternion::IDENTITY;
    BOOST_CHECK_EQUAL (test, other);

    test.makeIdentity();
    BOOST_CHECK_EQUAL (test, Core::Quaternion::IDENTITY);

    test = Core::Quaternion(0, 0, 0, 0);
    BOOST_CHECK_EQUAL (test, Core::Quaternion::ZERO);

    test.makeIdentity();
    test *= 10.f;
    BOOST_CHECK_EQUAL (test, Core::Quaternion(0, 0, 0, 10.f));

    test.makeIdentity();
    Core::Vector3df testVector(1, 2, 3);
    testVector = test * testVector;
    BOOST_CHECK_EQUAL (testVector, Core::Vector3df(1, 2, 3));
}

BOOST_AUTO_TEST_SUITE_END()
