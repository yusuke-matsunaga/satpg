#ifndef VALMAP_H
#define VALMAP_H

/// @file ValMap.h
/// @brief ValMap のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"
#include "Val3.h"
#include "ym/SatBool3.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class ValMap ValMap.h "ValMap.h"
/// @brief ノードの値を読みだすためのクラス
//////////////////////////////////////////////////////////////////////
class ValMap
{
public:

  /// @brief デストラクタ
  virtual
  ~ValMap() { }


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ノードの正常値を返す．
  /// @param[in] node 対象のノード
  virtual
  Val3
  gval(const TpgNode* node) const = 0;

  /// @brief ノードの故障値を返す．
  /// @param[in] node 対象のノード
  virtual
  Val3
  fval(const TpgNode* node) const = 0;

  /// @brief ノードの1時刻前の値を返す．
  /// @param[in] node 対象のノード
  virtual
  Val3
  hval(const TpgNode* node) const = 0;

};

END_NAMESPACE_YM_SATPG

#endif // VALMAP_H
