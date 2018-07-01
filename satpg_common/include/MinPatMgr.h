#ifndef MINPATMGR_H
#define MINPATMGR_H

/// @file MinPatMgr.h
/// @brief MinPatMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "satpg.h"
#include "Fsim.h"


BEGIN_NAMESPACE_YM_SATPG

class IdList;

//////////////////////////////////////////////////////////////////////
/// @class MinPatMgr MinPatMgr.h "MinPatMgr.h"
/// @brief テストセット最小化を行うクラス
//////////////////////////////////////////////////////////////////////
class MinPatMgr
{
public:

  /// @brief コンストラクタ
  MinPatMgr();

  /// @brief デストラクタ
  ~MinPatMgr();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化する．
  /// @param[in] fault_list 故障のリスト
  /// @param[in] tv_list テストパタンのリスト
  /// @param[in] network 対象のネットワーク
  /// @param[in] fault_type 故障の種類
  void
  init(const vector<const TpgFault*>& fault_list,
       const vector<TestVector>& tv_list,
       const TpgNetwork& network,
       FaultType fault_type);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障シミュレーションを行い被覆表を作る．
  /// @param[in] network 対象のネットワーク
  /// @param[in] fault_type 故障の種類
  ///
  /// 結果は mElemList に格納される．
  void
  gen_covering_matrix(const TpgNetwork& network,
		      FaultType fault_type);

  /// @brief テストパタンの衝突リストを作る．
  void
  gen_conflict_list();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象の故障リスト
  vector<const TpgFault*> mFaultList;

  // 元のテストパタンのリスト
  vector<TestVector> mOrigTvList;

  // covering matrix の要素
  vector<pair<int, int>> mElemList;

  // 衝突するパタンのリストの対のリスト
  // サイズはテストパタンのビット長 x 2
  vector<IdList*> mConflictPairList;

  // テストパタンごとの衝突リストのリスト
  vector<vector<IdList*>> mConflictListArray;

};

END_NAMESPACE_YM_SATPG

#endif // MINPATMGR_H
