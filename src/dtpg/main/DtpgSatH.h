﻿#ifndef DTPGSATH_H
#define DTPGSATH_H

/// @file DtpgSatH.h
/// @brief DtpgSatH のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012-2014, 2015 Yusuke Matsunaga
/// All rights reserved.


#include "DtpgSat.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class DtpgSatH DtpgSatH.h "DtpgSatH.h"
/// @brief MFFC と FFR の構造を考慮した階層的な DtpgSat
//////////////////////////////////////////////////////////////////////
class DtpgSatH :
  public DtpgSat
{
public:

  /// @brief コンストラクタ
  /// @param[in] sat_type SATソルバの種類を表す文字列
  /// @param[in] sat_option SATソルバに渡すオプション文字列
  /// @param[in] sat_outp SATソルバ用の出力ストリーム
  /// @param[in] bt バックトレーサー
  /// @param[in] dop パタンが求められた時に実行されるファンクタ
  /// @param[in] uop 検出不能と判定された時に実行されるファンクタ
  DtpgSatH(const string& sat_type,
	   const string& sat_option,
	   ostream* sat_outp,
	   BackTracer& bt,
	   DetectOp& dop,
	   UntestOp& uop);

  /// @brief デストラクタ
  virtual
  ~DtpgSatH();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト生成を行なう．
  /// @param[in] network 対象のネットワーク
  /// @param[in] fmgr 故障マネージャ
  /// @param[in] fsim 故障シミュレータ
  /// @param[in] fault_list 対象の故障リスト
  /// @param[out] stats 結果を格納する構造体
  virtual
  void
  run(TpgNetwork& network,
      FaultMgr& fmgr,
      Fsim& fsim,
      const vector<const TpgFault*>& fault_list,
      DtpgStats& stats);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

};

END_NAMESPACE_YM_SATPG

#endif // DTPGSATH_H
