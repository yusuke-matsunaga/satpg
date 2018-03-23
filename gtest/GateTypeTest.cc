
/// @file GateTypeTest.cc
/// @brief GateTypeTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "gtest/gtest.h"
#include "GateType.h"


BEGIN_NAMESPACE_YM_SATPG

TEST(GateTypeTest, gate_type_to_int)
{
  EXPECT_EQ( 0, __gate_type_to_int(GateType::CONST0) );
  EXPECT_EQ( 1, __gate_type_to_int(GateType::CONST1) );
  EXPECT_EQ( 2, __gate_type_to_int(GateType::INPUT) );
  EXPECT_EQ( 3, __gate_type_to_int(GateType::BUFF) );
  EXPECT_EQ( 4, __gate_type_to_int(GateType::NOT) );
  EXPECT_EQ( 5, __gate_type_to_int(GateType::AND) );
  EXPECT_EQ( 6, __gate_type_to_int(GateType::NAND) );
  EXPECT_EQ( 7, __gate_type_to_int(GateType::OR) );
  EXPECT_EQ( 8, __gate_type_to_int(GateType::NOR) );
  EXPECT_EQ( 9, __gate_type_to_int(GateType::XOR) );
  EXPECT_EQ(10, __gate_type_to_int(GateType::XNOR) );
}

TEST(GateTypeTest, int_to_gate_type)
{
  EXPECT_EQ( GateType::CONST0, __int_to_gate_type(0) );
  EXPECT_EQ( GateType::CONST1, __int_to_gate_type(1) );
  EXPECT_EQ( GateType::INPUT,  __int_to_gate_type(2) );
  EXPECT_EQ( GateType::BUFF,   __int_to_gate_type(3) );
  EXPECT_EQ( GateType::NOT,    __int_to_gate_type(4) );
  EXPECT_EQ( GateType::AND,    __int_to_gate_type(5) );
  EXPECT_EQ( GateType::NAND,   __int_to_gate_type(6) );
  EXPECT_EQ( GateType::OR,     __int_to_gate_type(7) );
  EXPECT_EQ( GateType::NOR,    __int_to_gate_type(8) );
  EXPECT_EQ( GateType::XOR,    __int_to_gate_type(9) );
  EXPECT_EQ( GateType::XNOR,   __int_to_gate_type(10) );
}

TEST(GateTypeTest, loop)
{
  for ( auto i: { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 } ) {
    EXPECT_EQ( i, __gate_type_to_int(__int_to_gate_type(i)) );
  }
}

END_NAMESPACE_YM_SATPG
