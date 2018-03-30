#ifndef GATELITMAP_H
#define GATELITMAP_H

/// @file GateLitMap.h
/// @brief GateLitMap のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"
#include "ym/SatLiteral.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class GateLitMap GateLitMap.h "GateLitMap.h"
/// @brief make_cnf で用いられるファンクター
///
/// ゲートの入出力に対応するリテラルを返すファンクターオブジェクト
/// このクラスはインターフェイスを定義しただけの純粋仮想基底クラス
//////////////////////////////////////////////////////////////////////
class GateLitMap
{
public:

  /// @brief デストラクタ
  virtual
  ~GateLitMap() { }

  /// @brief 入力数を返す．
  virtual
  int
  input_size() const = 0;

  /// @brief 入力のリテラルを返す．
  /// @param[in] pos 入力位置 ( 0 <= pos < input_size() )
  virtual
  SatLiteral
  input(int pos) const = 0;

  /// @brief 出力のリテラルを返す．
  virtual
  SatLiteral
  output() const = 0;

};

END_NAMESPACE_YM_SATPG

#endif // GATELITMAP_H
