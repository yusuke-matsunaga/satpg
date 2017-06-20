#ifndef AUXNODEINFO_H
#define AUXNODEINFO_H

/// @file AuxNodeInfo.h
/// @brief AuxNodeInfo のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"
#include "ym/Alloc.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class AuxNodeInfo AuxNodeInfo.h "AuxNodeInfo.h"
/// @brief TpgNode の付加的な情報を保持するクラス
//////////////////////////////////////////////////////////////////////
class AuxNodeInfo
{
public:

  /// @brief コンストラクタ
  AuxNodeInfo();

  /// @brief デストラクタ
  ~AuxNodeInfo();


public:
  //////////////////////////////////////////////////////////////////////
  // 情報を取得する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力の故障を返す．
  /// @param[in] val 故障値 ( 0 / 1 )
  TpgFault*
  output_fault(int val) const;

  /// @brief 入力の故障を返す．
  /// @param[in] pos 入力の位置番号
  /// @param[in] val 故障値 ( 0 / 1 )
  TpgFault*
  input_fault(ymuint pos,
	      int val) const;


public:
  //////////////////////////////////////////////////////////////////////
  // 情報を設定する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化する．
  /// @param[in] ni ファンイン数
  /// @param[in] alloc メモリアロケータ
  void
  init(ymuint ni,
       Alloc& alloc);

  /// @brief 出力の故障を設定する．
  /// @param[in] val 故障値 ( 0 / 1 )
  /// @param[in] f 故障
  void
  set_output_fault(int val,
		   TpgFault* f);

  /// @brief 入力の故障を設定する．
  /// @param[in] ipos 入力位置
  /// @param[in] val 故障値 ( 0 / 1 )
  /// @param[in] f 故障
  void
  set_input_fault(ymuint ipos,
		  int val,
		  TpgFault* f);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力の故障
  TpgFault* mOutputFaults[2];

  /// @brief 入力数
  ymuint mFaninNum;

  /// @brief 入力の故障の配列
  ///
  /// サイズは mFaninNum * 2
  TpgFault** mInputFaults;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief 出力の故障を返す．
// @param[in] val 故障値 ( 0 / 1 )
inline
TpgFault*
AuxNodeInfo::output_fault(int val) const
{
  ASSERT_COND( val == 0 || val == 1 );

  return mOutputFaults[val];
}

// @brief 入力の故障を返す．
// @param[in] pos 入力の位置番号
// @param[in] val 故障値 ( 0 / 1 )
inline
TpgFault*
AuxNodeInfo::input_fault(ymuint pos,
			 int val) const
{
  ASSERT_COND( val == 0 || val == 1 );
  ASSERT_COND( pos < mFaninNum );

  return mInputFaults[(pos * 2) + val];
}

END_NAMESPACE_YM_SATPG

#endif // AUXNODEINFO_H
