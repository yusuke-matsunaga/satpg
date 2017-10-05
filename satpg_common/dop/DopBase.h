#ifndef DOPBASE_H
#define DOPBASE_H

/// @file DopBase.h
/// @brief DopBase のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "DetectOp.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class DopBase DopBase.h "DopBase.h"
/// @brief DetectOp の基本的な動作を行なうクラス
//////////////////////////////////////////////////////////////////////
class DopBase :
  public DetectOp
{
public:

  /// @brief コンストラクタ
  /// @param[in] fmgr TpgFaultMgr
  DopBase(TpgFaultMgr& fmgr);

  /// @brief デストラクタ
  virtual
  ~DopBase();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テストパタンが見つかった時の処理
  /// @param[in] f 故障
  /// @param[in] assign_list 値割当のリスト
  virtual
  void
  operator()(const TpgFault* f,
	     const NodeValList& assign_list);

  /// @brief テストパタンが見つかった時の処理
  /// @param[in] f 故障
  /// @param[in] tv テストベクタ
  virtual
  void
  operator()(const TpgFault* f,
	     const TestVector* tv);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // TpgFaultMgr
  TpgFaultMgr& mMgr;

};

END_NAMESPACE_YM_SATPG

#endif // DOPBASE_H
