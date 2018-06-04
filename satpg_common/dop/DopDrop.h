﻿#ifndef OPDETECT_H
#define OPDETECT_H

/// @file DopDrop.h
/// @brief DopDrop のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "DetectOp.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class DopDrop DopDrop.h "DopDrop.h"
/// @brief 故障シミュレーションを行なって故障ドロップを行なうクラス
//////////////////////////////////////////////////////////////////////
class DopDrop :
  public DetectOp
{
public:

  /// @brief コンストラクタ
  /// @param[in] fsmgr 故障の状態を管理するクラス
  /// @param[in] fsim 故障シミュレータ
  DopDrop(FaultStatusMgr& fsmgr,
	  Fsim& fsim);

  /// @brief デストラクタ
  virtual
  ~DopDrop();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テストパタンが見つかった時の処理
  /// @param[in] f 故障
  /// @param[in] tv テストベクタ
  void
  operator()(const TpgFault* f,
	     const TestVector& tv) override;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障の状態を管理するクラス
  FaultStatusMgr& mFaultStatusMgr;

  // 故障シミュレータ
  Fsim& mFsim;

};

END_NAMESPACE_YM_SATPG

#endif // OPDETECT_H
