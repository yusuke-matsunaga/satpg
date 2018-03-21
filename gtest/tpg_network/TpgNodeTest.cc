
/// @file TpgNodeTest.cc
/// @brief TpgNodeTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "gtest/gtest.h"
#include "TpgNode.h"
#include "TpgNodeFactory.h"
#include "ym/SimpleAlloc.h"
#include "GateType.h"


BEGIN_NAMESPACE_YM_SATPG

class TpgNodeTest :
public ::testing::Test
{
public:

  // コンストラクタ
  TpgNodeTest();

  // メモリアロケータ
  SimpleAlloc mAlloc;

  // ファクトリ
  TpgNodeFactory mFactory;

  // 入力ノード
  TpgNode* mInputNodeList[5];

};

TpgNodeTest::TpgNodeTest() :
  mFactory(mAlloc)
{
  for ( auto i: { 0, 1, 2, 3, 4 } ) {
    ostringstream buf;
    buf << "Input" << i;
    mInputNodeList[i] = mFactory.make_input(i, string(buf.str()), i, 1);
  }
}

TEST_F(TpgNodeTest, input)
{
  int id = 10;
  string name = "abc";
  int iid = 20;
  int fanout_num = 0;
  auto node = mFactory.make_input(id, name, iid, fanout_num);

  EXPECT_TRUE( node != nullptr );
}

TEST_F(TpgNodeTest, output)
{
  int id = 10;
  string name = "abc";
  int oid = 20;
  auto node = mFactory.make_output(id, name, oid, mInputNodeList[0]);

  EXPECT_TRUE( node != nullptr );
}

TEST_F(TpgNodeTest, const0)
{
  int id = 10;
  string name = "abc";
  auto node = mFactory.make_logic(id, name, GateType::CONST0, vector<TpgNode*>(0), 0);

  EXPECT_TRUE( node != nullptr );
}

TEST_F(TpgNodeTest, const1)
{
  int id = 10;
  string name = "abc";
  auto node = mFactory.make_logic(id, name, GateType::CONST1, vector<TpgNode*>(0), 0);

  EXPECT_TRUE( node != nullptr );
}

END_NAMESPACE_YM_SATPG
