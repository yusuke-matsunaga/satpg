#ifndef RTPG1CMD_H
#define RTPG1CMD_H

/// @file Rtpg1Cmd.h
/// @brief Rtpg1Cmd のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "AtpgCmd.h"


BEGIN_NAMESPACE_SATPG

//////////////////////////////////////////////////////////////////////
// 故障シミュレーションを実行するコマンド
//////////////////////////////////////////////////////////////////////
class Rtpg1Cmd :
  public AtpgCmd
{
public:

  /// @brief コンストラクタ
  Rtpg1Cmd(AtpgMgr* mgr);

  /// @brief デストラクタ
  virtual
  ~Rtpg1Cmd();


protected:

  // コマンド処理関数
  virtual
  int
  cmd_proc(TclObjVector& objv);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // npat オプションの解析用オブジェクト
  TclPoptInt* mPoptNpat;

  // min_fault オプションの解析用オブジェクト
  TclPoptInt* mPoptMin;

  // max_interval オプションの解析用オブジェクト
  TclPoptInt* mPoptMax;

  // seed オプションの解析用オブジェクト
  TclPoptUint* mPoptSeed;

  // wsa_limit オプションの解析用オブジェクト
  TclPoptDouble* mPoptWsa;

  // mcmc オプションの解析用オブジェクト
  TclPoptUint* mPoptMcmc;

  // file オプションの解析用オブジェクト
  TclPoptStr* mPoptFile;

  // print_stats オプションの解析用オブジェクト
  TclPopt* mPoptPrintStats;

};

END_NAMESPACE_SATPG

#endif // RTPGCMD_H
