#ifndef DTPGMFFC_H
#define DTPGMFFC_H

/// @file DtpgMFFC.h
/// @brief DtpgMFFC のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "DtpgEngine.h"
#include "ym/HashMap.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class DtpgMFFC DtpgMFFC.h "DtpgMFFC.h"
/// @brief MFFC 単位で DTPG の基本的な処理を行うクラス
//////////////////////////////////////////////////////////////////////
class DtpgMFFC :
  public DtpgEngine
{
public:

  /// @brief コンストラクタ
  /// @param[in] network 対象のネットワーク
  /// @param[in] fault_type 故障の種類
  /// @param[in] just_type Justifier の種類を表す文字列
  /// @param[in] mffc 故障伝搬の起点となる MFFC
  /// @param[in] solver_type SATソルバの実装タイプ
  DtpgMFFC(const TpgNetwork& network,
	   FaultType fault_type,
	   const TpgMFFC& mffc,
	   const string& just_type,
	   const SatSolverType& solver_type = SatSolverType());

  /// @brief デストラクタ
  ~DtpgMFFC();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト生成を行なう．
  /// @param[in] fault 対象の故障
  /// @return 結果を返す．
  DtpgResult
  gen_pattern(const TpgFault* fault);


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief mffc 内の影響が root まで伝搬する条件のCNF式を作る．
  void
  gen_mffc_cnf();

  /// @brief 故障挿入回路のCNFを作る．
  /// @param[in] elem_pos 要素番号
  /// @param[in] ovar ゲートの出力の変数
  void
  inject_fault(int elem_pos,
	       SatVarId ovar);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // FFR の根のリスト
  // [0] は MFFC の根でもある．
  vector<const TpgNode*> mElemArray;

  // 各FFRの根に反転イベントを挿入するための変数
  // サイズは mElemArray.size()
  vector<SatVarId> mElemVarArray;

  // ノード番号をキーにしてFFR番号を入れる連想配列
  HashMap<int, int> mElemPosMap;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

END_NAMESPACE_YM_SATPG

#endif // DTPGMFFC_H
