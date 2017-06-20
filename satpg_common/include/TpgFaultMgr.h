#ifndef TPGFAULTMGR_H
#define TPGFAULTMGR_H

/// @file TpgFaultMgr.h
/// @brief TpgFaultMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.

#include "satpg.h"
#include "FaultStatus.h"
#include "TpgFault.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class TpgFaultMgr TpgFaultMgr.h "TpgFaultMgr.h"
/// @brief 故障管理するすクラス
///
/// 故障には以下の状態がある．
/// - 未検出
/// - 検出済み
/// - 検出不能
/// TpgFaultMgr は個々の故障に対する状態を持つ．
//////////////////////////////////////////////////////////////////////
class TpgFaultMgr
{
public:

  /// @brief コンストラクタ
  /// @param[in] network 対象のネットワーク
  TpgFaultMgr(const TpgNetwork& network);

  /// @brief デストラクタ
  ~TpgFaultMgr();


public:
  //////////////////////////////////////////////////////////////////////
  // read-only のメソッド
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障IDの最大値+1を返す．
  ymuint
  max_fault_id() const;

  /// @brief 故障IDから故障を返す．
  /// @param[in] id 故障ID
  const TpgFault*
  fault(ymuint id) const;

  /// @brief 故障の状態を得る．
  FaultStatus
  status(const TpgFault* fault) const;


public:
  //////////////////////////////////////////////////////////////////////
  // 内容を書き換えるメソッド
  //////////////////////////////////////////////////////////////////////

  /// @brief クリアする．
  ///
  /// 全ての故障の状態が「未検出」になる．
  void
  clear_status();

  /// @brief fault の状態を変更する．
  /// @param[in] fault 対象の故障
  /// @param[in] stat 故障の状態
  void
  set_status(const TpgFault* fault,
	     FaultStatus stat);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障番号の最大値
  ymuint mMaxFaultId;

  // 故障の配列
  // サイズは mMaxFaultId
  vector<const TpgFault*> mFaultArray;

  // 故障の状態を保持する配列
  // サイズは mMaxFaultId
  vector<FaultStatus> mStatusArray;

};

END_NAMESPACE_YM_SATPG

#endif // TPGFAULTMGR_H
