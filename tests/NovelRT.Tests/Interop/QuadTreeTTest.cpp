// Copyright © Matt Jones and Contributors. Licensed under the MIT License (MIT). See LICENCE.md in the repository root for more information.

#include <gtest/gtest.h>
#include <NovelRT.h>
#include "NovelRTInterop/Maths/QuadTree_t.h"


using namespace NovelRT;
using namespace NovelRT::Maths;

static const float TEST_WIDTH = 1920.0f;
static const float TEST_HEIGHT = 1080.0f;

GeoBounds_t getCenteredBounds(float width, float height) {
  auto size = GeoVector2<float>(width, height);
  auto position = GeoVector2<float>(0, 0);
  GeoBounds* result = new GeoBounds(position, size, 0);
  return reinterpret_cast<GeoBounds_t&>(*result);
}

bool checkPointsForEquality(QuadTreePoint_t one, QuadTreePoint_t two) {
  auto left = reinterpret_cast<QuadTreePoint*>(one);
  auto right = reinterpret_cast<QuadTreePoint*>(two);
  return left->getPosition() == right->getPosition();
}

class InteropQuadTreeTest : public testing::Test {
protected:
  QuadTree_t* _quadTree;

  void SetUp() override {
    GeoBounds_t* bounds = new GeoBounds_t();
    *bounds = getCenteredBounds(TEST_WIDTH, TEST_HEIGHT);
    _quadTree = new QuadTree_t();
    *_quadTree = QuadTree_create(*bounds);
  }
};

TEST_F(InteropQuadTreeTest, createCorrectlySetsBounds)
{
  auto treeBounds = getCenteredBounds(TEST_WIDTH, TEST_HEIGHT);
  *_quadTree = QuadTree_create(treeBounds);

  Maths::GeoBounds* expectedBounds = new GeoBounds(GeoVector2<float>(), GeoVector2<float>(), 0);
  GeoBounds_t test = getCenteredBounds(TEST_WIDTH, TEST_HEIGHT);
  *expectedBounds = reinterpret_cast<Maths::GeoBounds&>(test);
  Maths::GeoBounds* cBounds = new GeoBounds(GeoVector2<float>(), GeoVector2<float>(), 0);
  auto bounds = QuadTree_getBounds(*_quadTree);
  *cBounds = reinterpret_cast<const GeoBounds&>(bounds);
  EXPECT_EQ(*cBounds, *expectedBounds);
}

TEST_F(InteropQuadTreeTest, createHasNoPoints)
{
  auto bounds = getCenteredBounds(TEST_WIDTH, TEST_HEIGHT);
  *_quadTree = QuadTree_create(bounds);

  EXPECT_EQ(QuadTree_getPointCount(*_quadTree), 0u);
}

TEST_F(InteropQuadTreeTest, createHasNoChildren)
{
  auto bounds = getCenteredBounds(TEST_WIDTH, TEST_HEIGHT);
  *_quadTree = QuadTree_create(bounds);

  EXPECT_EQ(QuadTree_getTopLeft(*_quadTree), nullptr);
  EXPECT_EQ(QuadTree_getTopRight(*_quadTree), nullptr);
  EXPECT_EQ(QuadTree_getBottomLeft(*_quadTree), nullptr);
  EXPECT_EQ(QuadTree_getBottomRight(*_quadTree), nullptr);
}

TEST_F(InteropQuadTreeTest, getPointReturnsNullForTooLargeIndex)
{
  auto bounds = getCenteredBounds(TEST_WIDTH, TEST_HEIGHT);
  *_quadTree = QuadTree_create(bounds);

  EXPECT_EQ(QuadTree_getPoint(*_quadTree, 0), nullptr);
  EXPECT_EQ(QuadTree_getPoint(*_quadTree, 1), nullptr);
  EXPECT_EQ(QuadTree_getPoint(*_quadTree, 2), nullptr);
  EXPECT_EQ(QuadTree_getPoint(*_quadTree, 3), nullptr);
}

TEST_F(InteropQuadTreeTest, insertOneReturnsTrue) {
  auto bounds = getCenteredBounds(TEST_WIDTH, TEST_HEIGHT);
  *_quadTree = QuadTree_create(bounds);

  auto point0 = QuadTreePoint_createFromFloat(-1.0f, 1.0f);
  EXPECT_EQ(true, QuadTree_tryInsert(*_quadTree, point0));
}

TEST_F(InteropQuadTreeTest, insertFourDoesNotSubdivide) {
  auto bounds = getCenteredBounds(TEST_WIDTH, TEST_HEIGHT);
  *_quadTree = QuadTree_create(bounds);

  auto point0 = QuadTreePoint_createFromFloat(-1.0f, 1.0f);
  EXPECT_TRUE(QuadTree_tryInsert(*_quadTree, point0));

  auto point1 = QuadTreePoint_createFromFloat(1.0f, 1.0f);
  EXPECT_TRUE(QuadTree_tryInsert(*_quadTree, point1));

  auto point2 = QuadTreePoint_createFromFloat(-1.0f, -1.0f);
  EXPECT_TRUE(QuadTree_tryInsert(*_quadTree, point2));

  auto point3 = QuadTreePoint_createFromFloat(1.0f, -1.0f);
  EXPECT_TRUE(QuadTree_tryInsert(*_quadTree, point3));

  ASSERT_EQ(QuadTree_getPointCount(*_quadTree), 4u);

  EXPECT_TRUE(checkPointsForEquality(QuadTree_getPoint(*_quadTree, 0), point0));
  EXPECT_TRUE(checkPointsForEquality(QuadTree_getPoint(*_quadTree, 1), point1));
  EXPECT_TRUE(checkPointsForEquality(QuadTree_getPoint(*_quadTree, 2), point2));
  EXPECT_TRUE(checkPointsForEquality(QuadTree_getPoint(*_quadTree, 3), point3));

  EXPECT_EQ(QuadTree_getTopLeft(*_quadTree), nullptr);
  EXPECT_EQ(QuadTree_getTopRight(*_quadTree), nullptr);
  EXPECT_EQ(QuadTree_getBottomLeft(*_quadTree), nullptr);
  EXPECT_EQ(QuadTree_getBottomRight(*_quadTree), nullptr);
}


TEST_F(InteropQuadTreeTest, insertFiveDoesSubdivideAndPointsAreCorrect) {
  auto bounds = getCenteredBounds(TEST_WIDTH, TEST_HEIGHT);
  *_quadTree = QuadTree_create(bounds);

  auto point0 = QuadTreePoint_createFromFloat(-1.0f, 1.0f);
  EXPECT_TRUE(QuadTree_tryInsert(*_quadTree, point0));

  auto point1 = QuadTreePoint_createFromFloat(1.0f, 1.0f);
  EXPECT_TRUE(QuadTree_tryInsert(*_quadTree, point1));

  auto point2 = QuadTreePoint_createFromFloat(-1.0f, -1.0f);
  EXPECT_TRUE(QuadTree_tryInsert(*_quadTree, point2));

  auto point3 = QuadTreePoint_createFromFloat(1.0f, -1.0f);
  EXPECT_TRUE(QuadTree_tryInsert(*_quadTree, point3));

  auto point4 = QuadTreePoint_createFromFloat(0.0f, 0.0f);
  EXPECT_TRUE(QuadTree_tryInsert(*_quadTree, point4));

  EXPECT_EQ(QuadTree_getPointCount(*_quadTree), 0u);

  EXPECT_EQ(QuadTree_getPointCount(QuadTree_getTopLeft(*_quadTree)), 2u);
  EXPECT_EQ(QuadTree_getPointCount(QuadTree_getTopRight(*_quadTree)), 1u);
  EXPECT_EQ(QuadTree_getPointCount(QuadTree_getBottomLeft(*_quadTree)), 1u);
  EXPECT_EQ(QuadTree_getPointCount(QuadTree_getBottomRight(*_quadTree)), 1u);

  EXPECT_TRUE(checkPointsForEquality(QuadTree_getPoint(QuadTree_getTopLeft(*_quadTree), 0), point0));
  EXPECT_TRUE(checkPointsForEquality(QuadTree_getPoint(QuadTree_getTopRight(*_quadTree), 0), point1));
  EXPECT_TRUE(checkPointsForEquality(QuadTree_getPoint(QuadTree_getBottomLeft(*_quadTree), 0), point2));
  EXPECT_TRUE(checkPointsForEquality(QuadTree_getPoint(QuadTree_getBottomRight(*_quadTree), 0), point3));
  EXPECT_TRUE(checkPointsForEquality(QuadTree_getPoint(QuadTree_getTopLeft(*_quadTree), 1), point4));

}
/*
TEST_F(InteropQuadTreeTest, insertFiveDoesSubdivideAndBoundsAreCorrect) {
  auto point0 = std::make_shared<QuadTreePoint>(-1.0f, 1.0f);
  _quadTree->tryInsert(point0);

  auto point1 = std::make_shared<QuadTreePoint>(1.0f, 1.0f);
  _quadTree->tryInsert(point1);

  auto point2 = std::make_shared<QuadTreePoint>(-1.0f, -1.0f);
  _quadTree->tryInsert(point2);

  auto point3 = std::make_shared<QuadTreePoint>(1.0f, -1.0f);
  _quadTree->tryInsert(point3);

  auto point4 = std::make_shared<QuadTreePoint>(0.0f, 0.0f);
  _quadTree->tryInsert(point4);

  EXPECT_EQ(_quadTree->getPointCount(), 0u);

  EXPECT_EQ(_quadTree->getTopLeft()->getPointCount(), 2u);
  EXPECT_EQ(_quadTree->getTopRight()->getPointCount(), 1u);
  EXPECT_EQ(_quadTree->getBottomLeft()->getPointCount(), 1u);
  EXPECT_EQ(_quadTree->getBottomRight()->getPointCount(), 1u);

  auto expectedSize = GeoVector2(TEST_WIDTH / 2, TEST_HEIGHT / 2);

  EXPECT_EQ(_quadTree->getTopLeft()->getBounds(), GeoBounds(GeoVector2<float>(-TEST_WIDTH / 4, TEST_HEIGHT / 4), expectedSize, 0));
  EXPECT_EQ(_quadTree->getTopRight()->getBounds(), GeoBounds(GeoVector2<float>(TEST_WIDTH / 4, TEST_HEIGHT / 4), expectedSize, 0));
  EXPECT_EQ(_quadTree->getBottomLeft()->getBounds(), GeoBounds(GeoVector2<float>(-TEST_WIDTH / 4, -TEST_HEIGHT / 4), expectedSize, 0));
  EXPECT_EQ(_quadTree->getBottomRight()->getBounds(), GeoBounds(GeoVector2<float>(TEST_WIDTH / 4, -TEST_HEIGHT / 4), expectedSize, 0));
}

TEST_F(InteropQuadTreeTest, insertOutOfBoundsReturnsFalse) {
  auto point0 = std::make_shared<QuadTreePoint>(3840.0f, 2160.0f);
  EXPECT_EQ(false, _quadTree->tryInsert(point0));
}

TEST_F(InteropQuadTreeTest, getIntersectingPointsReturnsAllPoints) {
  auto point0 = std::make_shared<QuadTreePoint>(-1.0f, 1.0f);
  _quadTree->tryInsert(point0);

  auto point1 = std::make_shared<QuadTreePoint>(1.0f, 1.0f);
  _quadTree->tryInsert(point1);

  auto point2 = std::make_shared<QuadTreePoint>(-1.0f, -1.0f);
  _quadTree->tryInsert(point2);

  auto point3 = std::make_shared<QuadTreePoint>(1.0f, -1.0f);
  _quadTree->tryInsert(point3);

  auto point4 = std::make_shared<QuadTreePoint>(0.0f, 0.0f);
  _quadTree->tryInsert(point4);

  auto intersectingPoints = _quadTree->getIntersectingPoints(_quadTree->getBounds());

  EXPECT_EQ(intersectingPoints.size(), 5u);

  EXPECT_EQ(intersectingPoints[0], point0);
  EXPECT_EQ(intersectingPoints[1], point4);
  EXPECT_EQ(intersectingPoints[2], point1);
  EXPECT_EQ(intersectingPoints[3], point2);
  EXPECT_EQ(intersectingPoints[4], point3);
}

TEST_F(InteropQuadTreeTest, removeExistingReturnsTrue) {
  auto point0 = std::make_shared<QuadTreePoint>(-1.0f, 1.0f);
  _quadTree->tryInsert(point0);

  EXPECT_EQ(true, _quadTree->tryRemove(point0));
}

TEST_F(InteropQuadTreeTest, removeNoneExistingReturnsFalse) {
  auto point0 = std::make_shared<QuadTreePoint>(-1.0f, 1.0f);
  EXPECT_EQ(false, _quadTree->tryRemove(point0));
}

TEST_F(InteropQuadTreeTest, removeOneCausesMergeWhenAdjacentPointCountLessThan5) {
  auto point0 = std::make_shared<QuadTreePoint>(-1.0f, 1.0f);
  _quadTree->tryInsert(point0);

  auto point1 = std::make_shared<QuadTreePoint>(1.0f, 1.0f);
  _quadTree->tryInsert(point1);

  auto point2 = std::make_shared<QuadTreePoint>(-1.0f, -1.0f);
  _quadTree->tryInsert(point2);

  auto point3 = std::make_shared<QuadTreePoint>(1.0f, -1.0f);
  _quadTree->tryInsert(point3);

  auto point4 = std::make_shared<QuadTreePoint>(0.0f, 0.0f);
  _quadTree->tryInsert(point4);

  _quadTree->tryRemove(point4);

  EXPECT_EQ(_quadTree->getPointCount(), 4u);

  EXPECT_EQ(_quadTree->getPoint(0), point0);
  EXPECT_EQ(_quadTree->getPoint(1), point1);
  EXPECT_EQ(_quadTree->getPoint(2), point2);
  EXPECT_EQ(_quadTree->getPoint(3), point3);

  EXPECT_EQ(_quadTree->getTopLeft(), nullptr);
  EXPECT_EQ(_quadTree->getTopRight(), nullptr);
  EXPECT_EQ(_quadTree->getBottomLeft(), nullptr);
  EXPECT_EQ(_quadTree->getBottomRight(), nullptr);
}

TEST_F(InteropQuadTreeTest, removeOneDoesNotCauseMergeWhenAdjacentPointCountMoreThan4) {
  auto point0 = std::make_shared<QuadTreePoint>(-1.0f, 1.0f);
  _quadTree->tryInsert(point0);

  auto point1 = std::make_shared<QuadTreePoint>(1.0f, 1.0f);
  _quadTree->tryInsert(point1);

  auto point2 = std::make_shared<QuadTreePoint>(-1.0f, -1.0f);
  _quadTree->tryInsert(point2);

  auto point3 = std::make_shared<QuadTreePoint>(1.0f, -1.0f);
  _quadTree->tryInsert(point3);

  auto point4 = std::make_shared<QuadTreePoint>(0.0f, 0.0f);
  _quadTree->tryInsert(point4);

  auto point5 = std::make_shared<QuadTreePoint>(0.5f, 0.5f);
  _quadTree->tryInsert(point5);

  _quadTree->tryRemove(point5);

  EXPECT_EQ(_quadTree->getPointCount(), 0u);

  EXPECT_EQ(_quadTree->getTopLeft()->getPointCount(), 2u);
  EXPECT_EQ(_quadTree->getTopRight()->getPointCount(), 1u);
  EXPECT_EQ(_quadTree->getBottomLeft()->getPointCount(), 1u);
  EXPECT_EQ(_quadTree->getBottomRight()->getPointCount(), 1u);

  EXPECT_EQ(_quadTree->getTopLeft()->getPoint(0), point0);
  EXPECT_EQ(_quadTree->getTopRight()->getPoint(0), point1);
  EXPECT_EQ(_quadTree->getBottomLeft()->getPoint(0), point2);
  EXPECT_EQ(_quadTree->getBottomRight()->getPoint(0), point3);
  EXPECT_EQ(_quadTree->getTopLeft()->getPoint(1), point4);

  EXPECT_EQ(_quadTree->getTopLeft()->getPoint(0), point0);
  EXPECT_EQ(_quadTree->getTopRight()->getPoint(0), point1);
  EXPECT_EQ(_quadTree->getBottomLeft()->getPoint(0), point2);
  EXPECT_EQ(_quadTree->getBottomRight()->getPoint(0), point3);
  EXPECT_EQ(_quadTree->getTopLeft()->getPoint(1), point4);
}

TEST_F(InteropQuadTreeTest, getIntersectingPointsForQuadTreeBoundsReturnsAll) {
  auto point0 = std::make_shared<QuadTreePoint>(-1.0f, 1.0f);
  _quadTree->tryInsert(point0);

  auto point1 = std::make_shared<QuadTreePoint>(1.0f, 1.0f);
  _quadTree->tryInsert(point1);

  auto point2 = std::make_shared<QuadTreePoint>(-1.0f, -1.0f);
  _quadTree->tryInsert(point2);

  auto point3 = std::make_shared<QuadTreePoint>(1.0f, -1.0f);
  _quadTree->tryInsert(point3);

  auto point4 = std::make_shared<QuadTreePoint>(0.0f, 0.0f);
  _quadTree->tryInsert(point4);

  auto intersectingPoints = _quadTree->getIntersectingPoints(_quadTree->getBounds());
  EXPECT_EQ(intersectingPoints.size(), 5u);

  EXPECT_EQ(intersectingPoints[0], point0);
  EXPECT_EQ(intersectingPoints[1], point4);
  EXPECT_EQ(intersectingPoints[2], point1);
  EXPECT_EQ(intersectingPoints[3], point2);
  EXPECT_EQ(intersectingPoints[4], point3);
}
*/