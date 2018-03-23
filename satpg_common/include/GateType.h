#ifndef GATETYPE_H
#define GATETYPE_H

/// @file GateType.h
/// @brief GateType の定義ファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.

#include "satpg.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @brief 論理ゲートの種類を表す列挙型
//////////////////////////////////////////////////////////////////////
enum class GateType {
  /// @brief 定数0
  CONST0 = 0,

  /// @brief 定数1
  CONST1 = 1,

  /// @brief 入力
  INPUT  = 2,

  /// @brief バッファ
  BUFF   = 3,

  /// @brief NOT
  NOT    = 4,

  /// @brief AND
  AND    = 5,

  /// @brief NAND
  NAND   = 6,

  /// @brief OR
  OR     = 7,

  /// @brief NOR
  NOR    = 8,

  /// @brief XOR
  XOR    = 9,

  /// @brief XNOR
  XNOR   = 10,
};

/// @brief GateType のストリーム演算子
ostream&
operator<<(ostream& s,
	   GateType gate_type);


//////////////////////////////////////////////////////////////////////
// cython とのインターフェイス用の暫定的な関数
//////////////////////////////////////////////////////////////////////

/// @brief GateType の値を int に変換する．
inline
int
__gate_type_to_int(GateType gate_type)
{
  switch ( gate_type ) {
  case GateType::CONST0: return 0;
  case GateType::CONST1: return 1;
  case GateType::INPUT:  return 2;
  case GateType::BUFF:   return 3;
  case GateType::NOT:    return 4;
  case GateType::AND:    return 5;
  case GateType::NAND:   return 6;
  case GateType::OR:     return 7;
  case GateType::NOR:    return 8;
  case GateType::XOR:    return 9;
  case GateType::XNOR:   return 10;
  }
  ASSERT_NOT_REACHED;
  return 0;
}

/// @brief int の値を GateType に変換する．
inline
GateType
__int_to_gate_type(int val)
{
  switch ( val ) {
  case  0: return GateType::CONST0;
  case  1: return GateType::CONST1;
  case  2: return GateType::INPUT;
  case  3: return GateType::BUFF;
  case  4: return GateType::NOT;
  case  5: return GateType::AND;
  case  6: return GateType::NAND;
  case  7: return GateType::OR;
  case  8: return GateType::NOR;
  case  9: return GateType::XOR;
  case 10: return GateType::XNOR;
  }
  ASSERT_NOT_REACHED;
  return GateType::CONST0;
}

END_NAMESPACE_YM_SATPG

#endif // GATETYPE_H
