#ifndef RANDWSACMD_H
#define RANDWSACMD_H

/// @file RandWsaCmd.h
/// @brief RandWsaCmd のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "AtpgCmd.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class RandWsaCmd RandWsaCmd.h "RandWsaCmd.h"
/// @brief 乱数シミュレーションを用いて信号遷移回数を計算するコマンド
//////////////////////////////////////////////////////////////////////
class RandWsaCmd :
  public AtpgCmd
{
public:

  /// @brief コンストラクタ
  RandWsaCmd(AtpgMgr* mgr);

  /// @brief デストラクタ
  ~RandWsaCmd();


protected:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief コマンド処理関数
  virtual
  int
  cmd_proc(TclObjVector& objv);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // fsm モードオプションの解析用オブジェクト
  TclPopt* mPoptFsm;

  // bs モードオプションの解析用オブジェクト
  TclPopt* mPoptBs;

  // count オプションの解析用オブジェクト
  TclPoptInt* mPoptCount;

  // warmup_count オプションの解析用オブジェクト
  TclPoptInt* mPoptWarmup;

  // weighted オプションの解析用オブジェクト
  TclPopt* mPoptWeighted;

};

END_NAMESPACE_YM_SATPG

#endif // RANDWSACMD_H
